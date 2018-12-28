/***************************Copyright BestFu 2014-05-14*************************
文	件：	USARTx.h
说	明：	串口寄存器配置头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.08.20 
修　改：	暂无
*******************************************************************************/
#ifndef __USARTx_H
#define __USARTx_H

#include "BF_type.h"

#define USART		(1)		//串口编号

#if 	(USART == 1)
	#define UART		USART1		//串口寄存器
	#define UART_CLK	(1<<14)		//串口时钟
	
	#define UART_PORT		GPIO_A	//串口端口
	#define UART_TX_PIN		pin_9	//串口发送管脚
	#define UART_RX_PIN		pin_10	//串口接收管脚
	
#elif	(USART == 2)
	#define UART		USART2		//串口寄存器
	#define UART_CLK	(1<<17)		//串口时钟
	
	#define UART_PORT		GPIO_A	//串口端口
	#define UART_TX_PIN		pin_2	//串口发送管脚
	#define UART_RX_PIN		pin_3	//串口接收管脚
	
#endif

typedef struct
{
	u16 bound;
	u8 	bit;	//	有效数据位
	u8	check;	//0 不效验，1 寄效验， 2 偶效验
	u8 	stop;
}UARTCfg_t;		//串口配置结构体



void USARTx_Init(u32 pclk2, u32 bound);
u8 USARTx_Send(u8 *data, u16 len);
u8 USARTx_Write(u8 c);

#endif	//USART_x end 
/**************************Copyright BestFu 2014-05-14*************************/
