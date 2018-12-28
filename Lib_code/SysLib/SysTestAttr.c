/***************************Copyright BestFu 2014-05-14*************************
文	件：    SysTestAttr.c
说	明：    系统测试属性
编	译：    Keil uVision4 V4.54.0.0
版	本：    v2.0
编	写：    Unarty
日	期：    2014.07.03 
修　改：	暂无
*******************************************************************************/
#include "SysTestAttr.h"
#include "SI4432.h"
#include "USART.h"
#include "cmdProcess.h"
#include "Test.h"
#include "Thread.h"
#include "Upload.h"

void SI4432Test_Fail(void);

/*******************************************************************************
函 数 名:  	SI4432Test_Attr 
功能说明:  	433无线测试属性
参    数:  	*pdata-输入参数地址，pData[0]为单元号，
                                pData[1]为参数长度
                                pData[2]为参数值(0为关闭，非0为开启)
返 回 值:  1表示成功，0表示失败
*******************************************************************************/
MsgResult_t SI4432Test_Attr(UnitPara_t *pData)
{
	if (Test != TEST_STATE)		//不处于测试状态
	{
		return CMD_EXE_ERR;
	}
	
	if ((u32)send_433.fun == (u32)&UART_Send)	//目标发送处理函数为串口函数
	{
		ProcessSend_Fun(&Si4432_Send);
		Thread_Login(ONCEDELAY, 0, 1200, &SI4432Test_Fail);	//注册防止失败函数		
	}
	else
	{
		ProcessSend_Fun(&UART_Send);			//更换目标发送处理函数
		Si4432_Idle();	//将无线模块置于发送状态，使其不接收其他外部信号
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	SI4432_Test
功能说明:  	433模块测试
参    数:  	无
返 回 值:  	无
*******************************************************************************/
void SI4432Test_Fail(void)
{
	Upload_Fault(WIFI_433);
	ProcessSend_Fun(&UART_Send); //将发送处理返回到串口函数
}

/**************************Copyright BestFu 2014-05-14*************************/
