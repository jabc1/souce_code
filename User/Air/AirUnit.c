/***************************Copyright BestFu 2014-05-14*************************
文	件：	AirUnit.c
说	明：	空调逻辑层原代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.09.17 
修　改：	暂无
*******************************************************************************/
#include "AirUnit.h"
#include "UnitCfg.h"
#include "Thread.h"
#include "DeviceRelate.h"
#include "MSGPackage.h"
#include "AirCondition.h"
#include "Polling.h"
#include "config.h"
#include "Test.h"
#include "usart.h"	
AirStatus_t gAirStatus;		//空调状态 

static void Air_CmdSend(void);
//static void Air_StatusSave(void);
//static void Air_StatusCheck(void);


/*****************************************************************************
函 数 名:  	AirUnit_Init
功能说明:  	空调单元初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_UnitInit(void)
{
	EEPROM_Read(CORRECTION_TEMPERATURE_ADDR, sizeof(gAirCondition.Correction_temperature), (u8*)&gAirCondition.Correction_temperature);
	EEPROM_Read(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//获取上次空调状态
	EEPROM_Read(RS485_PARA_ADDR, sizeof(Air_Para_t), (u8*)&gAirConfigPara);
	
//	gAirConfigPara.Air_type = Air_RS485_Gateway;//测试用
//	gBindObject.ObjectType = 2;
	
	if(gAirStatus.SetMaxTemp > MAX_TEMP )
	{
		gAirStatus.SetMaxTemp = MAX_TEMP;
		EEPROM_Write(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//存储最新状态
	}
	if(gAirStatus.SetMinTemp < MIN_TEMP || gAirStatus.SetMinTemp == 0xff)
	{
		gAirStatus.SetMinTemp = MIN_TEMP;
		EEPROM_Write(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//存储最新状态
	}

	if(gAirStatus.SetMaxTemp < gAirStatus.SetMinTemp)
	{
		gAirStatus.SetMaxTemp = MAX_TEMP;
		gAirStatus.SetMinTemp = MIN_TEMP;
		EEPROM_Write(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//存储最新状态
	}
	
	Air_CmdDelaySend(1, 1000);	//发送默认指令：空调关闭
}

/*****************************************************************************
函 数 名:  	Air_BrandGet
功能说明:  	空调品牌获取
参    数:  	len:	长度
			data:	内容
返 回 值:  	品牌占用空间长度
*****************************************************************************/
u8 Air_BrandGet(u8 *data)
{
	u8 i;
	AIR_LibNameGet(AIR_EXPLAIN, &i, data);
	return i;
}

/*****************************************************************************
函 数 名:  	Air_CmdDelaySend
功能说明:  	空调指令延时发送
参    数:  	time:	延时时间，单位：ms
			cnt:	发送次数
返 回 值:  	无
*****************************************************************************/
void Air_CmdDelaySend(u8 cnt, u32 timer)
{
	if (cnt > 5 || 0 == cnt)
	{
		cnt = 5;
	}
	Thread_Login(MANY, cnt, timer, Air_CmdSend);	
}

/*****************************************************************************
函 数 名:  	Air_CmdSend
功能说明:  	空调指令发送
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_CmdSend(void)
{
	switch(gBindObject.ObjectType)
	{
		case 0x00:   //万能红外
			Air_InfraredCondition_SendData();
			break;  
		case 0x01:   //485转发控制
			Air_485TranspondCondition_SendData();
			break;
		case 0x02:   //无关联设备 通过自身的485协议控制
			Air_485DirectCondition_SendData();
			break;
		default :break;
	}
//	Air_StatusSave();   //状态保存		
	Upload();	
}

///*****************************************************************************
//函 数 名:  	Air_StatusSave
//功能说明:  	空调状态保存
//参    数:  	无
//返 回 值:  	无
//*****************************************************************************/
//static void Air_StatusSave(void)
//{
//	Thread_Login(ONCEDELAY, 0, 10*60000, &Air_StatusCheck);		//10分钟后确认状态，并做保存
//}

/*****************************************************************************
函 数 名:  	Air_StatusCheck
功能说明:  	空调状态核对
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_StatusCheck(void)
{
	AirStatus_t airStatus;
	
	EEPROM_Read(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&airStatus);	//获取存储的空调状态
	if (1 == memcmp(&airStatus, &gAirStatus, sizeof(AirStatus_t)))	//核对状态改变
	{
		EEPROM_Write(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//存储最新状态
	}
}

/*****************************************************************************
函 数 名:  	Air_485TranspondCondition_SendData
功能说明:  	绑定对象为485中转控制器空调命令转发
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_485TranspondCondition_SendData(void)
{
	if(1 == gDeviceRelate.UseFlag)                      //使能状态
    {
		u8 pData[10];
		pData[0] = gBindObject.Addr; 
		pData[1] = gAirStatus.power; 
		pData[2] = gAirStatus.temp ;
		pData[3] = gAirStatus.mode ;
		pData[4] = gAirStatus.speed;
		pData[5] = gAirStatus.wind ;
		
		//              目录类型/组/场景          目标ID       组、场景号   单元号   属性号   数据长度   数据  
		//void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
		
		Msg_Send((ObjectType_t)gDeviceRelate.RelateType , *(u32*)&(gDeviceRelate.ObjectID ),*(u16*)&gDeviceRelate.AssistID, \
				 gDeviceRelate.UnitNum , 0x51 , 6 , pData);
    }
}

/*****************************************************************************
函 数 名:  	Air_485DirectCondition_SendData
功能说明:  	绑定对象为485中转控制器空调命令转发
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_485DirectCondition_SendData(void)
{
//	if(1 == gDeviceRelate.UseFlag)                      //使能状态
	{
	//调用周旺提供接口
		if((Air_Slave == gAirConfigPara.Air_type) || (Air_Control == gAirConfigPara.Air_type))//温控面板，与网关一起工作,直接控制空调
		{
			Device_Polling();
		}
//		Air_InfraredCondition_SendData();
	}
}

/*****************************************************************************
函 数 名:  	Air_InfraredCondition_SendData
功能说明:  	绑定对象为万能红外控制空调时命令转发
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_InfraredCondition_SendData(void)
{
	if(1 == gDeviceRelate.UseFlag)                      //使能状态
  {
		u8 pData[10];   
		pData[0] = gAirStatus.power; 
		pData[1] = gAirStatus.temp ;
		pData[2] = gAirStatus.mode ;
		pData[3] = gAirStatus.speed ;
		pData[4] = gAirStatus.wind ;
		
		//              目录类型/组/场景          目标ID       组、场景号   单元号   属性号   数据长度   数据  
		//void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
		
		Msg_Send((ObjectType_t)gDeviceRelate.RelateType , *(u32*)&(gDeviceRelate.ObjectID ),*(u16*)&gDeviceRelate.AssistID, \
				 gDeviceRelate.UnitNum , 0x50 , 5 , pData);
  }
}


/*****************************************************************************
函 数 名:  	CheckBindAddr
功能说明:  	绑定地址检测
参    数:  	Addr 待检测地址
返 回 值:  	地址是否合法
*****************************************************************************/
u8 CheckBindAddr(u8 Addr)
{
	if(Addr == 0 || Addr > 64) return FALSE ;

	else return TRUE ;

}
void Air_run(void)
{
	if((check_flag & 0x01) && (0 == _CheckTime((u32 *)&UPST)))		//增加测试EEPROM，RS485，看门狗
	{
		//if(abst(Time_Get(),Recv_time) > 10)//大于10ms,判定modbus帧结束
		{
			if(Air_Slave == gAirConfigPara.Air_type) 		//与网关控制空调
			{
				Recv_Finish_Flag |= 0x02;
			}
			else if(Air_Control == gAirConfigPara.Air_type)		//直接控制空调
			{
				Recv_Finish_Flag |= 0x04;
			}
			else																						//网关
			{
				Recv_Finish_Flag |= 0x01;
			}
			check_flag &= 0xFE;
			recv_cnt = 0;
				Test_485_EEPROM_WATCHDOG();
		}
//				Thread_Login(MANY, 2, 500, &Test_485_EEPROM_WATCHDOG);
	}
	if(0x02 == gBindObject.ObjectType)
	{
		if(Air_Slave == gAirConfigPara.Air_type) 		//与网关控制空调
		{
			MS_Device_Polling();
		}
		else if(Air_Control == gAirConfigPara.Air_type)		//直接控制空调
		{
			Recv_Device_Ack(Recv_Communication_Data);
			Device_Polling_Send();
		}
		else
		{}
//					check_flag &= 0xFE;
	}
}
/**************************Copyright BestFu 2014-05-14*************************/
