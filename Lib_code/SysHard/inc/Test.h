/***************************Copyright BestFu 2014-05-14*************************
文	件：	Test.h
说	明：	测试函数头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#ifndef __TEST_H
#define __TEST_H

#include "BF_type.h"
#define TEST_EEPROM_ADDR (0X6300)
#define TEST_STATE		(0x5a5a5a5a)

extern u32 Test;
extern volatile u8 check_flag;
void Test_Init(void);
void Test_485_EEPROM_WATCHDOG(void);
void Test_EEPROM(void);
void Run_Test_485_EEPROM_WATCHDOG(void);
#endif		//Test.h end 
/**************************Copyright BestFu 2014-05-14*************************/
