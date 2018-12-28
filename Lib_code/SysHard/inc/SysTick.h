/***************************Copyright BestFu 2014-05-14*************************
文	件：	SysTick.h
说	明：	系统滴答
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-12-20
修　改：	暂无
*******************************************************************************/
#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "BF_type.h"

#define TIME_MAX        (400000000) //最大时间值

typedef struct {
	u32 _1ms;
}TimeFlag;

void SysTick_Init(u32 sysClock); //系统滴答初始化
u32 Time_Get(void);             //获取当前时间
void UpdateRandomTime(void);
u32 GetRandomTime(void);
#endif
/**************************Copyright BestFu 2014-05-14*************************/
