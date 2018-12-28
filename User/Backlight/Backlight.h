/*****************************************************************************
	文件： Backlight.h
	说明： 背光控制头文件
	编译： Keil uVision4 V4.54.0.0
	版本： v2.0
	编写： Unarty
	日期： 2014-11-14
*****************************************************************************/
#ifndef __BACKLIGHT_H
#define __BACKLIGHT_H

#define BACKLIGHT_PORT	(GPIO_A)
#define BACKLIGHT_PIN	(pin_1)

#define BACKLIGHT_ON	GPIOx_Set(BACKLIGHT_PORT, BACKLIGHT_PIN)
#define BACKLIGHT_OFF	GPIOx_Rst(BACKLIGHT_PORT, BACKLIGHT_PIN)
#define BACKLIGHT_RD	GPIOx_Get(BACKLIGHT_PORT, BACKLIGHT_PIN)

#define BACKLIGHT_ON_TIME	(10000)		//背光点亮时间 单位：ms

void Backlight_Init(void);
void Backlight_On(void);
void Backlight_Off(void);
void Backlight_Run(void);
void Backlight_Instruct(void);


#endif 
/**************************Copyright BestFu 2014-05-14*************************/
