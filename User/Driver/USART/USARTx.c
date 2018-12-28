/***************************Copyright BestFu 2014-05-14*************************
文	件：	USARTx.c
说	明：	串口寄存器配置文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.08.20 
修　改：	暂无
*******************************************************************************/
#include "USARTx.h"
#include "GPIO.h"
#include "Sys.h"
#include "fifo.h"


/*******************************************************************************
函 数 名:  	USARTx_Init
功能说明:  	串口1初始化函数
参    数:  	pclk2:	PCLK2时钟频率(Mhz)
			bound:	波特率
返 回 值:  	无
*******************************************************************************/
void USARTx_Init(u32 pclk2, u32 bound)
{  	
#if 	(USART == 1)	
	RCC->APB2RSTR 	|= UART_CLK;   	//复位串口
	RCC->APB2RSTR 	&= ~(UART_CLK);	//停止复位 
	
	RCC->APB2ENR  	|= UART_CLK;  	//使能串口时钟 
#elif	(USART == 2)
	RCC->APB1ENR	|= UART_CLK;  	//使能串口2时钟 
	RCC->APB1RSTR	|= UART_CLK;   	//复位串口2
	RCC->APB1RSTR	&= ~(UART_CLK);	//停止复位
#endif
	GPIOx_Cfg(UART_PORT, UART_TX_PIN, AF_PP_50M);  //配置串口管脚	
	GPIOx_Cfg(UART_PORT, UART_RX_PIN, IN_UPDOWN);  //配置串口管脚	
	  
 	UART->BRR = (pclk2*1000000)/(bound); 	// 波特率设置	 
	UART->CR1|=(1<<13)|(1<<3)|(1<<2); 		//(UE)使能usart1 (TE)使能发送 (RE)使能接收//8bit数据 1bit停止,无校验位.
	UART->CR1|=(1<<5);  					//(RXNEIE)接收中断使能 	
	
#if 	(USART == 1)		  				   
	MY_NVIC_Init( 2, 2, USART1_IRQn, 2);
#elif	(USART == 2) 		     
	MY_NVIC_Init( 2, 2, USART2_IRQn, 2);
#endif
}

/*******************************************************************************
函 数 名：	USARTx_WriteBit
功能说明：	串口写入一个字节数据 
参	  数：	ch:	要写入的数据
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/ 
u8 USARTx_Write(u8 c)
{   
	u16 t;
	
	for (t = 0; ((USART1->SR&0X40) == 0)&&(t < 20000); t++)   //等待数据发送完毕
	{}	
	if (t < 20000)  //未超时
	{
		USART1->DR = (u8)c;
		return TRUE;
	}
	      
	return FALSE;
}
 	
/*******************************************************************************
函 数 名：	USARTx_Send
功能说明：	串口数据发送
参	  数：	*data: 要发送的数内容
			len :	数据长度
返 回 值：	发送结果 TRUE/FALSE
*******************************************************************************/ 
u8 USARTx_Send(u8 *data, u16 len)
{
	while (len--)
	{
		if (!USARTx_Write(*data++))
		{
			return FALSE;
		}
	}

	return TRUE;
}

#if 	(USART == 1)
/*******************************************************************************
函 数 名:  	USART1_IRQHandler
功能说明:  	串口1中断数据接收函数
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void USART1_IRQHandler(void)
{	
	if(UART->SR&(1<<5))//接收到数据
	{	 
//		fifo_putc(&MusicRcveCmdfifo, UART->DR);
	}
} 

#elif	(USART == 2) 
/*******************************************************************************
函 数 名:  	USART1_IRQHandler
功能说明:  	串口1中断数据接收函数
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void USART2_IRQHandler(void)
{
	u8 i;
	
	if(UART->SR&(1<<5))//接收到数据
	{	 
		i = UART->DR;
	}
} 
#endif

/**************************Copyright BestFu 2014-05-14*************************/	
