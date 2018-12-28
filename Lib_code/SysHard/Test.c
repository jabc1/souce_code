/***************************Copyright BestFu 2014-05-14*************************
文	件：	Test.c
说	明：	测试函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#include "BF_type.h"
#include "Test.h"
#include "cmdProcess.h"
#include "USART.h"
#include "SysHard.h"
#include "GPIO.h"
#include "delay.h"
#include "SI4432.h"
#include "Fifo.h"
#include "Thread.h"
#include "DeviceRelate.h"
#include "wdg.h"
#include "Polling.h"

u32 Test;
//const u8 Eeprom_test[4] = {'t','e','s','t'};
const u8 Uart_OK_Output[] = {'u','a','r','t','_','O','K','\r','\n'};
//const u8 EEPROM_OK_Output[] = {'E','E','P','R','O','M','_','O','K','\r','\n'};
//u8 recv_test_cmd[100];
volatile u8 check_flag = 0;
/*******************************************************************************
函 数 名：	Test_Init
功能说明： 	测初始化函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Test_Init(void)
{
	TEST_KEY_INIT;
	
	if (!TEST_KEY_VAL)
	{
		delay_ms(10);
		if (!TEST_KEY_VAL) //检测输入按键
		{
			Test = TEST_STATE;
			UART_Init(36);
			ProcessSend_Fun(&UART_Send); //切换信号处理函数
			Si4432_Idle();;	//将无线模块置于发送状态，使其不接收其他外部信号
			return;
		}
	}
	
	Test = 0;
}
/*******************************************************************************
函 数 名：	Test_485_EEPROM_WATCHDOG
功能说明： 	测初RS485
参	  数： 	无
返 回 值：	无
*******************************************************************************/

void Test_485_EEPROM_WATCHDOG(void)
{ 
	u8 search_cnt;
/*
	for(search_cnt = 0; search_cnt < RECV_DATA_LEN; search_cnt++)
	{
		if('b' == recv_test_cmd[search_cnt])
		{
			if(('e' == recv_test_cmd[search_cnt+1]) && ('s' == recv_test_cmd[search_cnt+2]) && ('t' == recv_test_cmd[search_cnt+3]) &&\
				('f' == recv_test_cmd[search_cnt+4]) && ('u' == recv_test_cmd[search_cnt+5]) && ('-' == recv_test_cmd[search_cnt+6]) &&\
				('t' == recv_test_cmd[search_cnt+7]) && ('e' == recv_test_cmd[search_cnt+8]) && ('s' == recv_test_cmd[search_cnt+9]) &&\
				('t' == recv_test_cmd[search_cnt+10])
			)
			{
				recv_test_cmd[search_cnt] = 0;
				Thread_Login(MANY, 2, 500, &Run_Test_485_EEPROM_WATCHDOG);
			}
		}
	}
*/

	for(search_cnt = 0; search_cnt < RECV_DATA_LEN; search_cnt++)
	{
		if('b' == Recv_Communication_Data[search_cnt])
		{
			if(('e' == Recv_Communication_Data[search_cnt+1]) && ('s' == Recv_Communication_Data[search_cnt+2]) && ('t' == Recv_Communication_Data[search_cnt+3]) &&\
				('f' == Recv_Communication_Data[search_cnt+4]) && ('u' == Recv_Communication_Data[search_cnt+5]) && ('-' == Recv_Communication_Data[search_cnt+6]) &&\
				('t' == Recv_Communication_Data[search_cnt+7]) && ('e' == Recv_Communication_Data[search_cnt+8]) && ('s' == Recv_Communication_Data[search_cnt+9]) &&\
				('t' == Recv_Communication_Data[search_cnt+10])
			)
			{
				Recv_Communication_Data[search_cnt] = 0;
				Thread_Login(ONCEDELAY, 0, 500, &Run_Test_485_EEPROM_WATCHDOG);
			}
		}
	}
}


void Run_Test_485_EEPROM_WATCHDOG(void)
{
	UART_Send((u8*)&Uart_OK_Output,sizeof(Uart_OK_Output),0);
//	Test_EEPROM();
//	Thread_Logout(Test_485_EEPROM_WATCHDOG);
//	WDG_FeedOFF();
}

/*******************************************************************************
函 数 名：	Test_Init
功能说明： 	测初始化函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
/*
void Test_EEPROM(void)
{
	u8 recv_test[4];
	EEPROM_Write(TEST_EEPROM_ADDR, sizeof(Eeprom_test), (u8*)&Eeprom_test);
	EEPROM_Read(TEST_EEPROM_ADDR, sizeof(Eeprom_test), (u8*)&recv_test);
	if(('t' == recv_test[0]) && ('e' == recv_test[1]) && ('s' == recv_test[2]) && ('t' == recv_test[3]))
	{
		UART_Send((u8*)&EEPROM_OK_Output,sizeof(EEPROM_OK_Output),0);
	}
}
*/

/**************************Copyright BestFu 2014-05-14*************************/
