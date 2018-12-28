/***************************Copyright BestFu 2014-05-14*************************
文	件：	Upload.c
说	明：	设备主动上报相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#include "Upload.h"
#include "MsgPackage.h"
#include "UnitShare_Attr.h"
#include "Thread.h"
#include "SysTick.h"

static u8 Fault;

void Upload_Attr(void);
static void FaultMsg(void);
#ifndef   STM32L151
static void Upload_Idle(void);
#endif 
/*******************************************************************************
函 数 名：	Upload_Fault
功能说明： 	主动异常上报
参	  数： 	upload: 异常情况具体见异常枚举歌词
返 回 值：	无
*******************************************************************************/
void Upload_Fault(Upload_t upload)
{
	u32 time;
	
	Fault = (u8)upload;
	if (upload == USER_DATA_RESET)
	{
		time = 1000;
	}
	else
	{
		time = 5;
	}
#ifndef   STM32L151 		//不是低功耗产品
	if(upload == RESTART)
		Upload_Idle();		//空闲时主动上报，防止无线死机
#endif 
	Thread_Login(ONCEDELAY, 0, time, &FaultMsg);
}

/*******************************************************************************
函 数 名：	FaultMsg
功能说明： 	异常消息封装
参	  数： 	upload: 异常情况具体见异常枚举歌词
返 回 值：	无
*******************************************************************************/
void FaultMsg(void)
{
	Msg_Upload(0, 0xFF, 1, (u8*)&Fault);
}

/*******************************************************************************
函 数 名：	Upload
功能说明： 	设备属性状态上报
参	  数： 	无
返 回 值：	无
注    意:	2.5s-5.5s 离散度10ms-3000ms
*******************************************************************************/
void Upload(void)
{
	u32 time = 2500 + (Time_Get()%300)*10;
	Thread_Login(ONCEDELAY, 0, time, &Upload_Attr);
}
#ifndef   STM32L151 		//不是低功耗产品
/*******************************************************************************
函 数 名：	Upload_Idle
功能说明： 	空闲时间，设备属性状态上报，防止死机
参	  数： 	无
返 回 值：	无
注    意:	1h-2h 离散度1s-3600s
*******************************************************************************/
void Upload_Idle(void)
{
	u32 time = 3600000 + (GetRandomTime()%3600)*1000;
	Thread_Login(ONCEDELAY , 0, time, &Upload_Attr);
}
#endif 
/*******************************************************************************
函 数 名：	Upload_Attr
功能说明： 	设备属性状态上报
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Upload_Attr(void)
{
	u8 data[200];
#ifndef   STM32L151 		//不是低功耗产品
					Upload_Idle();		//空闲时主动上报，防止无线死机
#endif 
	(*(UnitPara_t*)&data).len = 0; //数据长度等于零
	data[3] = Get_ManyUnit_Attr((UnitPara_t*)data, &data[2], &data[4]);
	data[2]++; //数据长度加1（通信成功与否标识）
	Msg_UploadUnit(0, 0x00, data[2], &data[3]);	
}

/**************************Copyright BestFu 2014-05-14*************************/
