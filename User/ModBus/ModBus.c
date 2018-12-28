/*****************************************************************************
	文件： ModBus.c
	说明： ModBus通信协议
	编译： Keil uVision4 V4.54.0.0
	版本： v2.0
	编写： Unarty
	日期： 2014.11.24
*****************************************************************************/
#include "ModBus.h"
#include "USARTx.h"
#include "delay.h"
#include "Fifo.h"
#include "Thread.h"
#include "UnitCfg.h"

#include "Upload.h"

#define MUSIC_HEAD1			(0x7F)		//指令头1
#define MUSIC_HEAD2			(0x7F)		//指令头2
#define MUSIC_HEAD3			(0x7F)		//指令头3
#define MUSIC_HEAD4			(0x7E)		//指令头4
#define MUSIC_HEAD_CRC		(MUSIC_HEAD1 + MUSIC_HEAD2 + MUSIC_HEAD3 + MUSIC_HEAD4)	//指令头CRC校验值

#define MUSIC_CRC_SIZE		(2)			//音乐指令校验码大小, 单位：字节

static const u8 MusicCmdHead[] = {MUSIC_HEAD1, MUSIC_HEAD2, MUSIC_HEAD3, MUSIC_HEAD4};	//音乐通信指令头
static ModBusCmd_t MusicUploadCmd;	//音乐上报指令内容
static u8 MusicUploadCmdFlag;		//音乐上报指令标记
static u8 MusicWaitAckFlag;			//音乐等待应答标记

ModBusCmd_t gMusicExeCmd;			//音乐执行指令内容

u8 MusicRcveCmdBuf[256]; 	//音乐指令接收缓冲区
FIFO_t MusicRcveCmdfifo;		//音乐接收指令队列

MusicStatus_t	gMusicStatus;		//音乐状态记录值


static u8 Music_CmdGet(ModBusCmd_t *pMusicCmd);
static u32 Music_CRCGet(u8 *pData, u8 len);
static void Music_CmdProcess(void);
static void Music_Save(void);
static void Music_StatusSyn(void);


/*******************************************************************************
函 数 名:  	ModBus_Init
功能说明:  	音乐单元初始化函数
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void ModBus(void)
{
	MusicUploadCmdFlag = FALSE;
	MusicWaitAckFlag = FALSE;
	USARTx_Init(72, 115200);
	USARTx_Send("Uart 1232", 10);
	fifo_Init(&MusicRcveCmdfifo, MusicRcveCmdBuf, sizeof(MusicRcveCmdBuf));	//初始化音乐指令接收队列
	Thread_Login(FOREVER, 0, 800, &Music_CmdProcess);						//注册锁指令处理线程
	Thread_Login(ONCEDELAY, 0, 2000, &Music_StatusSyn);						//1秒钟后同步音乐状态
}

/*******************************************************************************
函 数 名:  	Music_StatusSyn
功能说明:  	音乐状态同步
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void Music_StatusSyn(void)
{
	ModBusCmd_t musicCmd;
	
	EEPROM_Read(MUSIC_EEPROM_ADDR, sizeof(MusicStatus_t), (u8*)&gMusicStatus);
	if (gMusicStatus.status > 10 || 0 == gMusicStatus.menu)	//如果获取的音乐状态异常
	{
		gMusicStatus.mode = 2;		//顺序播放
		gMusicStatus.menu = 1;		//播放曲目 1
		gMusicStatus.volume = 40;	//播放音量 40%
		Music_StatusSave(100);			//保存音乐状态
	}
	
	musicCmd.cmd 	= PLAY_MODE_CMD;	//音乐播放模式
	musicCmd.sCmd 	= 0;
	musicCmd.len 	= 1;
	musicCmd.data[0] = gMusicStatus.mode;
	Music_CmdSend(&musicCmd);
	
	musicCmd.cmd 	= PLAY_VOLUME_CMD;	//播放音量命令
	musicCmd.sCmd 	= 0;
	musicCmd.len 	= 1;
	musicCmd.data[0] = gMusicStatus.volume;
	Music_CmdSend(&musicCmd);
}

/*******************************************************************************
函 数 名:  	Music_StatusSave
功能说明:  	音乐状态保存
参    数:  	time:	延时时间
返 回 值:  	无
*******************************************************************************/
void Music_StatusSave(u32 time)
{
	Thread_Login(ONCEDELAY, 0, time, Music_Save);
}

/*******************************************************************************
函 数 名:  	Music_Save
功能说明:  	音乐状态保存
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void Music_Save(void)
{
	EEPROM_Write(MUSIC_EEPROM_ADDR, sizeof(MusicStatus_t), (u8*)&gMusicStatus);
}

/*******************************************************************************
函 数 名:  	Music_CmdSend
功能说明:  	音乐指令发送
参    数:  	pMusicCmd: 要发送的锁指令内容
返 回 值:  	无
*******************************************************************************/
void Music_CmdSend(ModBusCmd_t *pMusicCmd)
{
	u16 crc;
	u8 	cmdLen;
	
	cmdLen = (pMusicCmd->len + 3);				//通信指令总长度
	crc = Music_CRCGet((u8*)pMusicCmd, cmdLen);	//获取指令校验码
	Data_MAXMIN((u8*)&crc, sizeof(crc));			//大小端对齐转换
	
	USARTx_Send((u8*)MusicCmdHead, sizeof(MusicCmdHead));		//发送指令头
	USARTx_Send((u8*)pMusicCmd, cmdLen);						//发送指令内容
	USARTx_Send((u8*)&crc, sizeof(crc));					//发送指令校验码
}

/*******************************************************************************
函 数 名:  	Music_CmdExe
功能说明:  	音乐指令执行
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void Music_CmdExe(void)
{
	Music_CmdSend(&gMusicExeCmd);	//发送音乐当前要执行的指令
	MusicWaitAckFlag = TRUE;		//后台等待指令应答
}


/*******************************************************************************
函 数 名:  	Music_CmdProcess
功能说明:  	音乐指令处理
参    数:  	无
返 回 值:  	无
注    意：	音乐上报一些锁状态属性，函数解悉上报数据实时修改音乐的状态，并调用上
			报。
*******************************************************************************/
void Music_CmdProcess(void)
{
	static u32 ackWaitTimeValue;	
//	Upload_Fault(WIFI_433);		//测试433发送功率
	
	if ((TRUE == MusicUploadCmdFlag)					//有未处理指令
		||(TRUE == Music_CmdGet(&MusicUploadCmd))		//如果获取到音乐上报指令
		)
	{
		MusicUploadCmdFlag = FALSE;
		if (MusicUploadCmd.cmd == GET_AUDIOCOUNT_CMD)	//音乐主动上报当前曲目
		{
			Data_MAXMIN(MusicUploadCmd.data, MusicUploadCmd.len);
			gMusicStatus.menu = *(u32*)&MusicUploadCmd.data[0];
		}
		else if (TRUE == MusicWaitAckFlag)	//有执行指令等待设备应答
		{
			if (MusicUploadCmd.cmd == gMusicExeCmd.cmd)	//上报指令内容为当前等待应答的指令。
			{
				MusicWaitAckFlag = FALSE;	//指令执行成功，取消应答等待
				ackWaitTimeValue = 0;
			}
		}
	}
	else if ((TRUE == MusicWaitAckFlag)//有执行指令等待设备应答
			&& (!(ackWaitTimeValue++%2)) // 等待回应超2s
			)
	{
		MusicWaitAckFlag = FALSE;
		if (ackWaitTimeValue <  8)	//等待回应时间没有超8s
		{
			Music_CmdExe();	//再次发送指令
		}
		else
		{
			ackWaitTimeValue = 0;
		}
	}
}

/*******************************************************************************
函 数 名:  	Music_CmdAckGet
功能说明:  	获取音乐指令应答
参    数:  	pMusicCmd: 	要发送的音乐指令内容
			pOvertime:	超时时间
返 回 值:  	无
*******************************************************************************/
u8 Music_CmdAckGet(ModBusCmd_t *pMusicCmd, u16 *pOvertime)
{
	u8 cmd = pMusicCmd->cmd;
	
	while ((*pOvertime))              
	{
		(*pOvertime)--;
		delay_ms(1);
		if (TRUE == Music_CmdGet(pMusicCmd))		//如果获取到音乐上报指令
		{
			if (pMusicCmd->cmd == cmd)	//是该指令应答
			{
				return TRUE;
			}
			else
			{
				MusicUploadCmdFlag = TRUE;
				memcpy(&MusicUploadCmd, pMusicCmd, sizeof(ModBusCmd_t));	//指令内容转移到后期处理
			}
		}
	}
	
	return FALSE;
}

/*******************************************************************************
函 数 名:  	Data_MAXMIN
功能说明:  	数据大小端对齐方式转换
参    数:  	data:	要转换的数据 
			size:	要转换的数据大小
返 回 值:  	无
*******************************************************************************/
void Data_MAXMIN(u8 *data, u8 size)
{
	u32 i, j;

	for (i = 0, j = size, size >>= 1; i < size; i++)
	{
		j--;
		data[i]	^= data[j];
		data[j] ^= data[i];
		data[i] ^= data[j];
	}
}

/*******************************************************************************
函 数 名:  	Music_CmdGet
功能说明:  	获取音乐反馈指令
参    数:  	pMusicCmd: 	要发送的音乐指令内容
返 回 值:  	FALSE(未获取到有效指令)/TRUE(获取到有效指令，内容在*pMusicCmd中)
*******************************************************************************/
u8 Music_CmdGet(ModBusCmd_t *pMusicCmd)
{
	u32 i;
	
	i = fifo_find(&MusicRcveCmdfifo, (u8*)MusicCmdHead, sizeof(MusicCmdHead));	//队列中查找头
	MusicRcveCmdfifo.front = i;												//移除队列无效数据
	ERRR(fifo_empty(&MusicRcveCmdfifo) == TRUE, goto ERR1);        			//FIFO为空
	
	i = sizeof(MusicCmdHead) + 3;
	ERRR(i > fifo_validSize(&MusicRcveCmdfifo), goto ERR1); 					//指令数据未满

/*指令转送中0D指令CRC长度有问题*/	
//	i += MusicRcveCmdfifo.data[(MusicRcveCmdfifo.front + i - 1)%MusicRcveCmdfifo.size]
//		 + MUSIC_CRC_SIZE;		//指令总长度
/*先不使用CRC校验*/
	i += MusicRcveCmdfifo.data[(MusicRcveCmdfifo.front + i - 1)%MusicRcveCmdfifo.size];

	ERRR(i > MusicRcveCmdfifo.size, goto ERR2);		//指令长度错误
	ERRR(i > fifo_validSize(&MusicRcveCmdfifo), goto ERR1); 		//队列有效数据长度错误
	
	ERRR(FALSE == fifo_gets(&MusicRcveCmdfifo, (u8*)pMusicCmd, sizeof(MusicCmdHead)), goto ERR1); //指令头出队
	ERRR(FALSE == fifo_gets(&MusicRcveCmdfifo, (u8*)pMusicCmd, i - sizeof(MusicCmdHead)), goto ERR1); //指令内容出队
	
	return TRUE;
	
ERR2: MusicRcveCmdfifo.front = (MusicRcveCmdfifo.front+sizeof(MusicCmdHead))%MusicRcveCmdfifo.size; //数据错误将头移出
ERR1: return FALSE;
}

/*******************************************************************************
函 数 名:  	Music_CRCGet
功能说明:  	音乐取锁指令CRC校验值
参    数:  	pData:	指令内容
			len:	指令长度
返 回 值:  	CRC值
注	  意：	CRC为数据校验和
*******************************************************************************/
u32 Music_CRCGet(u8 *pData, u8 len)
{
	u32 crc;
	
	for (crc = MUSIC_HEAD_CRC; len > 0;)	
	{
		len--;
		crc += pData[len];	//数据区内容校验和
	}
	
	return crc;
}

/**************************Copyright BestFu 2014-05-14*************************/	
