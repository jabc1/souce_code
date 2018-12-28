/*****************************************************************************
	文件： PowerCtr.c
	说明： 音乐电源控制实现函数头文件
	编译： Keil uVision4 V4.54.0.0
	版本： v2.0
	编写： Unarty
	日期： 2014-04-17
*****************************************************************************/
#ifndef __POWERCTR_H
#define __POWERCTR_H

#include "BF_type.h"

#define POWERCTR_PORT 	GPIO_A
#define POWERCTR_PIN	pin_11

void PowerCtr_Init(void);
void PowerCtr_ON(void);
void PowerCtr_OFF(void);

#endif //powerCtr.h end 
/**************************Copyright BestFu 2014-05-14*************************/	
