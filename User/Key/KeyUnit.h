/***************************Copyright BestFu 2014-05-14*************************
文	件：	KeyUnit.h
说	明：	按键单元功能函数头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.08.28
修　改：	暂无
*******************************************************************************/
#ifndef __KEYUNIT_H
#define __KEYUNIT_H

#include "BF_type.h"

#define UP		(KEY1_PIN)
#define DOWN	(KEY3_PIN)
#define MENU	(KEY2_PIN)
#define KEY_DELAY_SAVE_TIME	(20000)
extern volatile u32 EEprom_Save_UPST;

void Save_EEprom_Time_Update(void);
void KeyUnit_Init(void);
void Save_EEprom_OverTime(void);
#endif //keyUnit.h end 
/**************************Copyright BestFu 2014-05-14*************************/
