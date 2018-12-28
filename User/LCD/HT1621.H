#ifndef _HT1621_H_
#define _HT1621_H_

#include "stm32f10x.h"

#include "BF_type.h"
#include "GPIO.h"
#include "delay.h"

//端口配置
#define GPIO_HT1621_CS		GPIOB
#define PORT_HT1621_CS		GPIO_B
#define PIN_HT1621_CS		(1<<0)

//#define GPIO_HT1621_RD		GPIOA
//#define PORT_HT1621_RD		GPIO_A
//#define PIN_HT1621_RD		    (1<<11)

#define GPIO_HT1621_WR		GPIOA
#define PORT_HT1621_WR		GPIO_A
#define PIN_HT1621_WR		(1<<7)

#define GPIO_HT1621_DATA	GPIOA
#define PORT_HT1621_DATA	GPIO_A
#define PIN_HT1621_DATA		(1<<6)

#define HT1621_DATA_1		GPIO_HT1621_DATA->ODR |= PIN_HT1621_DATA
#define HT1621_DATA_0		GPIO_HT1621_DATA->ODR &=~PIN_HT1621_DATA
//#define HT1621_RD_1			GPIO_HT1621_RD->ODR   |= PIN_HT1621_RD
//#define HT1621_RD_0			GPIO_HT1621_RD->ODR   &=~PIN_HT1621_RD
#define HT1621_WR_1			GPIO_HT1621_WR->ODR   |= PIN_HT1621_WR
#define HT1621_WR_0			GPIO_HT1621_WR->ODR   &=~PIN_HT1621_WR
#define HT1621_CS_1			GPIO_HT1621_CS->ODR   |= PIN_HT1621_CS
#define HT1621_CS_0			GPIO_HT1621_CS->ODR   &=~PIN_HT1621_CS


extern const u8 DisSegTap[20];

extern void HT1621_Init(void);
extern void HT1621_DispNum(u8 Seat , u8 Num);
extern void HT1621_DispPoint(u8 point_1 , u8 point_2);
extern void HT1621_lowPower(void);

extern void HT1621_SendDisdata(u8 Address, u8 Disdata);
#endif

