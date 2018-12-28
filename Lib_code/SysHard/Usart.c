/***************************Copyright BestFu 2014-05-14*************************
文	件：	UART.c
说	明：	串口操作相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#include "sys.h"
#include "usart.h"	 
#include "Bf_type.h"
#include "cmdProcess.h"
#include "gpio.h"
#include "Fifo.h"
#include "Test.h"
#include "SysTick.h"
#include "Polling.h"
/*******************************************************************************
*******************************************************************************/ 
extern u8 recv_test_cmd[100];
volatile u8 recv_cnt = 0;
u32 Recv_time;
/*******************************************************************************
函 数 名：	UART_Init
功能说明：	串口初始化
参	  数：	pclk2： 时钟频率（单位M）
			bound： 通信波特率
返 回 值：	无
*******************************************************************************/ 
void UART_Init(u32 pclk2)
{  
	UARTCfg_t uartCfg;
	
	UART_CfgGet(&uartCfg);
	
	if (FALSE == UART_CfgCheck(&uartCfg))	//如果配置数据异常
	{
		UART_CfgDefault(&uartCfg);
	}  
#if 	(USART == 1)	
	RCC->APB2RSTR 	|= UART_CLK;   	//复位串口
	RCC->APB2RSTR 	&= ~(UART_CLK);	//停止复位 
	RCC->APB2ENR  	|= UART_CLK;  	//使能串口时钟 
#elif	(USART == 2)
	RCC->APB1RSTR	|= UART_CLK;   	//复位串口2
	RCC->APB1RSTR	&= ~(UART_CLK);	//停止复位
	RCC->APB1ENR	|= UART_CLK;  	//使能串口2时钟 
#endif
	GPIOx_Cfg(UART_PORT, UART_TX_PIN, AF_PP_50M);  //配置串口管脚	
	GPIOx_Cfg(UART_PORT, UART_RX_PIN, IN_UPDOWN);  //配置串口管脚	
	  
 	UART->BRR = (pclk2*1000000)/(uartCfg.bound); 	// 波特率设置	
	switch (uartCfg.check)	//校验位
	{
		case 2: UART->CR1 |= (1<<9);	//使能校验控制 / 奇校验
				break;
		case 1: UART->CR1 |= (1<<10);			//使能校验控制、偶校验
				UART->CR1 |= (1<<12);			//9个数据位 一个停止位
				break;
		default: break;							//无校验
	}
	switch (uartCfg.dataBit) //有效数据位
	{
		case 9:	UART->CR1 |= (1<<12);		//9个数据位 一个停止位
				break;
		case 8:	
		default : break;
	}
	switch (uartCfg.stopBit)	//数据停止位 
	{
		case 3:	UART->CR2 |= (3<<12);  //1.5
			break;
		case 2: UART->CR2 |= (2<<12);	//2
			break;
		case 1: 						//1
		default:  break;
	}
				
	UART->CR1|=(1<<13)|(1<<3)|(1<<2); 		//(UE)使能usart1 (TE)使能发送 (RE)使能接收
	UART->CR1|=(1<<5);  					//(RXNEIE)接收中断使能 	
	
#if 	(USART == 1)		  				   
	MY_NVIC_Init( 2, 2, USART1_IRQn, 2);
#elif	(USART == 2) 		     
	MY_NVIC_Init( 2, 2, USART2_IRQn, 2);
#endif	


}
 	



/*******************************************************************************
函 数 名:  	RS232_CfgDefault
功能说明:  	串口配置默认值 
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void UART_CfgDefault(UARTCfg_t *cfg)
{
	cfg->bound = 9600;
	cfg->check = 0;
	cfg->dataBit = 8;
	cfg->stopBit = 1;
	UART_CfgSave(cfg);
}

/*******************************************************************************
函 数 名：	UART_Send
功能说明：	串口数据发送
参	  数：	*data: 要发送的数内容
			len :	数据长度
返 回 值：	发送结果 TRUE/FALSE
*******************************************************************************/ 
u8 UART_Send(u8 *data, u8 len, u8 level)
{
	while (len--)
	{
		if (!UART_Write(*data++))
		{
			UART_Init(36);
		}
	}

	return TRUE;
}

/*******************************************************************************
函 数 名：	UART_WriteBit
功能说明：	串口写入一个字节数据 
参	  数：	ch:	要写入的数据
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/ 
u8 UART_Write(u8 c)
{   
	u32 t;
	for (t = 0; ((USART2->SR&0X40) == 0)&&(t < 20000); t++)   //µÈ´ýÊý¾Ý·¢ËÍÍê±Ï
	{}	
		
	if (t < 20000)  //未超时
	{
		USART2->DR = (u8)c;
		return TRUE;
	}
	      
	return FALSE;
}
/*******************************************************************************
函 数 名:  	UART_CfgCheck
功能说明:  	串口配置数据核对
参    数:  	cfg:	目标配置值
返 回 值:  	FALSE/TRUE
*******************************************************************************/
u8 UART_CfgCheck(UARTCfg_t *cfg)
{
	switch (cfg->bound)
	{
		case 2400:
		case 4800:
		case 9600:
		case 14400:
		case 19200:
		case 38400:
		case 43000:
		case 56000:
		case 115200:
		case 128000:
		case 256000:
			break;
		default : return FALSE;
	}
	switch (cfg->check)
	{
		case 0:
		case 1:
		case 2:
			break;
		default : return FALSE;
	}
	switch (cfg->dataBit)
	{
		case 8:
		case 9:
			break;
		default : return FALSE;
	}
	switch (cfg->stopBit)
	{
		case 1:
		case 2:
		case 3:
			break;
		default : return FALSE;
	}
	return TRUE;
}

/*******************************************************************************
函 数 名:  	UART_CfgSave
功能说明:  	串口配置存储
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void UART_CfgSave(UARTCfg_t *cfg)
{
	EEPROM_Write(UART_CFG_ADDR, sizeof(UARTCfg_t), (u8*)cfg);
}
/*******************************************************************************
函 数 名:  	RS232_CfgDefault
功能说明:  	串口配置默认值 
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void RS232_CfgDefault(UARTCfg_t *cfg)
{
	cfg->bound = 9600;
	cfg->check = 0;
	cfg->dataBit = 8;
	cfg->stopBit = 1;
	UART_CfgSave(cfg);
}
/*******************************************************************************
函 数 名:  	UART_CfgGet
功能说明:  	串口配置数据获取
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void UART_CfgGet(UARTCfg_t *cfg)
{
	EEPROM_Read(UART_CFG_ADDR, sizeof(UARTCfg_t), (u8*)cfg);
}

#if 	(USART == 1)	
/*******************************************************************************
函 数 名：	USART1_IRQHandler
功能说明：	串口中断处理
参	  数：	无
返 回 值：	无
*******************************************************************************/ 
void USART1_IRQHandler(void)
{
	if(UART->SR&(1<<5))//接收到数据
	{	 
//		fifo_putc(&rcv433fifo, USART1->DR);
	}

}
#elif  (USART == 2)
/*******************************************************************************
函 数 名：	USART1_IRQHandler
功能说明：	串口中断处理
参	  数：	无
返 回 值：	无
*******************************************************************************/ 
void USART2_IRQHandler(void)
{
//	u8 temp;
	if(UART->SR&(1<<5))//接收到数据
	{	 
//		UART_Write( USART2->DR);
//		fifo_putc(&rcv433fifo, USART2->DR);
		/*
		recv_test_cmd[recv_cnt] = USART2->DR;
		if((recv_test_cmd[recv_cnt] == 'b') || (recv_test_cmd[recv_cnt] == 'B'))
		{
			check_flag |= 0x01;
		}
		recv_cnt++;
		if(recv_cnt >= 100)
		{
			recv_cnt = 0;
		}
		*/
		if(recv_cnt < RECV_DATA_LEN)
		{
			Recv_Communication_Data[recv_cnt++] = USART2->DR;
		}
//		Recv_time = Time_Get();
		_SetTimeing_ms((u32*)&UPST,10);
		check_flag |= 0x01;
	}
}
#endif

/**************************Copyright BestFu 2014-05-14*************************/
