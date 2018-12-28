/***************************Copyright BestFu 2014-05-14*************************
文	件：	GPIO.h
说	明：	GPIO端口配置文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-08-09
修　改：	暂无
*******************************************************************************/
#ifndef __GPIO_H
#define __GPIO_H


#include "BF_type.h"


typedef enum{
	GPIO_A = 0,
	GPIO_B = 1,
	GPIO_C = 2,
	GPIO_D = 3,
	GPIO_E = 4,
	GPIO_F = 5,
	GPIO_G = 6
}GPIO_Port;  //GPIO端口定义

typedef enum{
	pin_0  = (1<<0),
	pin_1  = (1<<1),
	pin_2  = (1<<2),
	pin_3  = (1<<3),
	pin_4  = (1<<4),
	pin_5  = (1<<5),
	pin_6  = (1<<6),
	pin_7  = (1<<7),
	pin_8  = (1<<8),
	pin_9  = (1<<9),
	pin_10 = (1<<10),
	pin_11 = (1<<11),
	pin_12 = (1<<12),
	pin_13 = (1<<13),
	pin_14 = (1<<14),
	pin_15 = (1<<15)
}GPIO_Pin;  //GPIO引脚定义

typedef enum{
	IN_   		= 0x0,	 //模拟输入模式
	IN_UPDOWN   = 0x8,	 //上/下拉输入模式
	IN_FLOATING = 0x4,	 //浮空输入
	OUT_PP_10M  = 0x1,
	OUT_PP_2M   = 0x2,	 //推挽输出
	OUT_PP_50M  = 0x3,
	OUT_OP_10M  = 0x5,
	OUT_OP_2M   = 0x6,	 //开漏输出
	OUT_OP_50M  = 0x7,
	AF_PP_10M   = 0x9,
	AF_PP_2M    = 0xa,	 //复用推挽输出
	AF_PP_50M   = 0xb,
	AF_OP_10M   = 0xd,
	AF_OP_2M    = 0xe,	 //复用开漏输出
	AF_OP_50M   = 0xf,
	
}GPIO_Mode;  //GPIO模式定义

#define GPIO_BASE (APB2PERIPH_BASE + 0x0800) //GPIO端口基址值


void GPIOx_Cfg(GPIO_Port GPIOx, u16 pin, GPIO_Mode mode);    //GPIO配置
u16 GPIOx_Set(GPIO_Port GPIOx, u16 pin);                  //GPIO引脚输出电平置1
void GPIOx_Rst(GPIO_Port GPIOx, u16 pin);                    //GPIO引脚输出电平置0
u16 GPIOx_Get(GPIO_Port GPIOx, u16 pin);                 	//获取GPIO引脚输入电平
void GPIOx_Rvrs(GPIO_Port GPIOx, u16 pin);					//GPIO引脚电平取反
//INT32U GPIOx_Lock(GPIO_Port GPIOx, u16 pin, INT8U sta);    //给相应GPIO引脚配置上锁/去锁


#endif //GPIO.h end
/**************************Copyright BestFu 2014-05-14*************************/
