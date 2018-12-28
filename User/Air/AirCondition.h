/***************************Copyright BestFu 2014-05-14*************************
文	件：	AirCondition.h
说	明：	空调外界条件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.11.17 
修　改：	暂无
*******************************************************************************/
#ifndef __AIRCONDITION_H
#define __AIRCONDITION_H

#include "BF_type.h"
#include "stdint.h"
#include "Polling.h"

typedef struct 
{
//	u16 temp;	//外界温度
	int16_t temp;
	int8_t Correction_temperature;
	u8 timer;	//运行时间
}AirCondition_t;	//空调外界条件

extern AirCondition_t gAirCondition;

void AirCondition_Init(void);
void AirCondition_Temp(void);
void AirCondition_TimerSet(u8 val);

#endif		//AirAttr.h end
/**************************Copyright BestFu 2014-05-14*************************/
