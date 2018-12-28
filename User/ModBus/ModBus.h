/*****************************************************************************
	文件： ModBus.h
	说明： ModBus通信协议定义
	编译： Keil uVision4 V4.54.0.0
	版本： v2.0
	编写： Unarty
	日期： 2014.11.24	
*****************************************************************************/
#ifndef __MODBUS_H
#define __MODBUS_H

#include "BF_type.h"

#define MUSIC_EEPROM_ADDR	(0x5000)	//音乐数据EEPROM存储地址

#define MUSIC_OVERTIME		(1200)	//音乐播放器最大超时时间

#define MUSIC_MAX_PARA_LEN	201	//音乐通信最大数据长度

//数据大端对齐
#pragma pack(1)
typedef struct
{
	u8 cmd; 	//(1~127, 128~255为异常指令， 如：0x0F(Send), 异常 0x8F(ACK)
	
}ModBusCmd_t;	//音乐播放当前状态
#pragma pack()
typedef enum
{
    STRENGTH_DATA	 	= 0x02,	//音乐强度数据
    MUSIC_PLAY        	= 0x1F,	//音乐播放
    MUSIC_PAUSE       	= 0x24,	//音乐暂停
    MUSIC_STOP	        = 0x20, //音乐停止
    MUSIC_UP			= 0x5C,	//上一首
    MUSIC_DOWN			= 0x5D,	//下一首
    VOLUME_ADD   		= 0x18,	//音量加
    VOLUME_SUB   		= 0x19,	//音量减
    MUSIC_MUTE        	= 0x17,	//静音
    MUSIC_SRC			= 0x31,	//音源
    MUSIC_MENU     		= 0x32,	//音乐曲目
    FILE_COUNT	  		= 0x01,	//文件总数
    VOLUME_RW			= 0x00,	//音量直接读写
    FILE_UPDATA	  		= 0x00,	//文件更新
}MusicPara_t;	//音乐指令参数

typedef enum
{
	PLAY_ERR_CMD		= 0x00,		//错误播放命令
	PLAY_CTRL_CMD     	= 0x0D,  	//播放控制指令
	DIRVE_FIND_CMD  	= 0x12,  	//设备查询命令
	FILE_COUNT_CMD		= 0x0F,  	//获取文件总数命令
	PLAY_POINTER_CMD    = 0x11, 	//指定播放命令
	PLAY_MODE_CMD	  	= 0x13,  	//播放模式命令 
	PLAY_VOLUME_CMD    	= 0x14,  	//播放音量命令
	GET_VOLUME_CMD     	= 0x20,  	//音量获取命令
	PLAY_TIMER_CMD    	= 0x15,  	//播放定时指令
    GET_STRENG_CMD   	= 0x16,		//获取音强指令(用于音乐中喷泉）
    GET_FREP_CMD     	= 0x17,		//获取各音频强度指令(用于音乐中喷泉）
    GET_FILEMENU_CMD 	= 0x18, 	//获取文件列表命令
    FILE_ABORT_CMD   	= 0x19,		//文件列表传输结束指令 
    FILE_MENU_CMD	   	= 0x30,	  	//文件列表传输指令
    FILE_TIMER_CMD   	= 0x21,		//文件最新修改时间属性
    GET_AUDIOCOUNT_CMD 	= 0x31,		//当前音乐曲目
	PLAY_RATE_CMD		= 0x1a,		//播放进度指令 2014.11.20 Unarty Add

    PLAY_MUTE_ACK_CMD   = 0x17,		//播放静音反馈命令
    PLAY_SRC_ACK_CMD    = 0x31,  	//音源切换反馈命令	
}MusicPlayCmd_t;  //音乐播放器指令

extern MusicStatus_t 	gMusicStatus;;		//
extern MusicCmd_t 		gMusicExeCmd;		//音乐当前要执行的指令 

void ModBus(void);
void Data_MAXMIN(u8 *data, u8 size);
void Music_CmdExe(void);
void Music_CmdSend(MusicCmd_t *pMusicCmd);
u8   Music_CmdAckGet(MusicCmd_t *pMusicCmd, u16 *pOvertime);
void Music_StatusSave(u32 time);

#endif
/**************************Copyright BestFu 2014-05-14*************************/	
