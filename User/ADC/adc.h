/***************************Copyright BestFu 2014-05-14*************************
文	件：	ADC.c
说	明：	模数转换函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.12.22 
修　改：	暂无
*******************************************************************************/
#ifndef __ADC_H
#define __ADC_H	

#include "BF_type.h"
							  
	   									   
void Adc_Init(void); 				//ADC通道初始化
u16  Get_Adc(u8 ch); 				//获得某个通道值 
#endif 
/**************************Copyright BestFu 2014-05-14*************************/
