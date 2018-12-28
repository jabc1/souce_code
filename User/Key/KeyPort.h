/***************************Copyright BestFu 2014-05-14*************************
文	件：	KeyPort.h
说	明：	键端口配置处理头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.11.14
修　改：	暂无
*******************************************************************************/
#ifndef __KEYPORT_H
#define __KEYPORT_H

#include "BF_type.h"
#include "GPIO.h"


#define KEY_LONG_CNT	(80)	//按键长按事件计数次数
#define KEY_LONG_SPACE	(15)	//长按时间间隔
#define KEY_SHORT_CNT	(1)		//短按计数时长

#define KEY_LONG_EVENT		(0x80)	//长按标记
#define KEY_SHORT_EVENT		(0x40)	//短按标记

#define KEY_DEFAULT_VALUE	(0x00)	//按键默认值

#define POWER			(1<<0)		//电源按键
#define MODE			(1<<1)		//模式
#define SPEED			(1<<2)		//风速
#define ADD				(1<<3)		//加
#define SUB				(1<<4)		//减
#define TIMER			(1<<5)		//定时

typedef struct
{
	GPIO_Port port;
	u16 	  pin;
}KeyPort_t;	//按键端口结构体
	

void KeyPort_Init(void);
u32  KeyPort_ValueGet(void);
u8 	 KeyPort_Scan(void);
void Clr_Flag(void);
#endif //keyPort.h end 
/**************************Copyright BestFu 2014-05-14*************************/
