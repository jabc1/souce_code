/***************************Copyright BestFu 2014-05-14*************************
文	件：	WDG.h
说	明：	看门狗相关头文件 
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.05.14 
修　改：	暂无
*******************************************************************************/
#ifndef __WDG_H
#define __WDG_H

#include "BF_type.h"


void WDG_Init(u8 prer,u16 rlr);
void WDG_Feed(void);
void WDG_FeedOFF(void);

#endif	//Wdg.h end
/**************************Copyright BestFu 2014-05-14*************************/
