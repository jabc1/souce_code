/***************************Copyright BestFu 2014-05-14*************************
文	件：	UART.h
说	明：	串口操作相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#ifndef __USART_H
#define __USART_H

#include "BF_type.h"
#define UART_CFG_ADDR		(0x4000)	//串口配置存储地址

#define USART		(2)		//串口编号

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
	u32 bound;
	u8	check;
	u8 	dataBit;
	u8	stopBit;
}UARTCfg_t;
/******************************************************************************/
extern u32 Recv_time;
extern volatile u8 recv_cnt;
/******************************************************************************/
void UART_Init(u32 pclk2);
u8 UART_Write(u8 c);
u8 UART_Send(u8 *data, u8 len, u8 level);

extern u8 UART_CfgCheck(UARTCfg_t *cfg);
extern void UART_CfgSave(UARTCfg_t *cfg);
void UART_CfgGet(UARTCfg_t *cfg);
void UART_CfgDefault(UARTCfg_t *cfg);

#endif	   
/**************************Copyright BestFu 2014-05-14*************************/
