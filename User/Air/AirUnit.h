/***************************Copyright BestFu 2014-05-14*************************
文	件：	AirUnit.h
说	明：	空调逻辑层头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.09.17 
修　改：	暂无
*******************************************************************************/
#ifndef __AIRUNIT_H
#define __AIRUNIT_H

#include "BF_type.h"

#define AIR_STATUS_ADDR		(0x3A00)		//空调状态存储地址

#define AIR_OFF				(0x00)			//空调关
#define AIR_ON				(0x01)			//空调开


#define MAX_TEMP (32)
#define MIN_TEMP (16)
typedef struct 
{
	u8 power;	//空调电源
	u8 temp;	//空调温度
	u8 mode; 	//空调模式
	u8 wind;	//空调风向
	u8 speed;	//空调风速
	u8 SetMaxTemp;  //可设置的最高温度
	u8 SetMinTemp;  //可设置的最低温度
}AirStatus_t;	//空调状态属性


/******************************************************************************/
extern AirStatus_t gAirStatus;
/******************************************************************************/
void Air_UnitInit(void);
void Air_CmdDelaySend(u8 cnt, u32 timer);
extern u8 Air_BrandGet(u8 *data);
extern void Air_485TranspondCondition_SendData(void);
extern void Air_InfraredCondition_SendData(void);
extern void Air_485DirectCondition_SendData(void);
extern u8 CheckBindAddr(u8 Addr);
extern void Air_StatusCheck(void);
extern void Air_run(void);
#endif		//AirUnit.h end
/**************************Copyright BestFu 2014-05-14*************************/
