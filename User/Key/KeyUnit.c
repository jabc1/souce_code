/***************************Copyright BestFu 2014-05-14*************************
文	件：	KeyUnit.c
说	明：	按键单元功能函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.08.28
修　改：	暂无
*******************************************************************************/
#include "KeyUnit.h"
#include "KeyPort.h"
#include "Thread.h"
#include "AirCondition.h"
#include "AirAttr.h"
#include "Polling.h"
#include "AirUnit.h"
/******************************************************************************/
static void Key_process(void);
volatile u32 EEprom_Save_UPST = 0;
static u8 Key_Press_Flag = 0;
/*******************************************************************************
函 数 名：	KeyUnit_Init
功能说明： 	按键单元初始化
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void KeyUnit_Init(void)
{
	KeyPort_Init();
	if(Air_RS485_Gateway != gAirConfigPara.Air_type)
	{
		Thread_Login(FOREVER, 0, 10, &Key_process);
	}
}
	
/*******************************************************************************
函 数 名：	key_Process
功能说明： 	按键处理
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Key_process(void)
{
	static u8 keyStatus = 0;
	u8 i;
	
	if ((i = KeyPort_Scan()) != 0)	//有按键事件
	{
		u8 data[4];
		
		data[0]	= 0x01;	//单元号
		data[1] = 0x00;	//命令号
		data[2]	= 0x00;	//参数长度
		
		switch (i)	//清除长按标识
		{
			case POWER: data[1]	= 0x03;	//电源开关属性	
						Save_EEprom_Time_Update();
						break;
			case MODE:	data[1] = 0x0B;	//模式自加属性
						Save_EEprom_Time_Update();
						break;
			case SPEED:	data[1] = 0x0D;	//风速自加属性
						Save_EEprom_Time_Update();
						break;
			case ADD:	if (0 == keyStatus)	//温度加减按键
						{
							data[1] = 0x09;	//温度自加属性
						}
						else
						{
							data[3]++;
							data[3] %= 10;
						}
						Save_EEprom_Time_Update();
						break;
			case SUB:	if (0 == keyStatus)	//温度加减按键
						{
							data[1] = 0x0A;	//温度自减属性
						}
						else
						{
							data[3]--;
							data[3] %= 10;
						}
						Save_EEprom_Time_Update();
						break;
			case TIMER:	if (0 == keyStatus)	//进入定时
						{
							keyStatus = 1;
							data[3] %= 10;
						}
						else
						{
							keyStatus = 0;
						}
						break;
			default: 	break;
		}
		if (0 != data[1])	//产生有效的属性命令
		{
			keyStatus = 0;
			Set_AirStatus_Attr((UnitPara_t*)data);
		}
		else if (0 != keyStatus) //定时设置中
		{
			AirCondition_TimerSet(data[3]);	//设置进入定时 
		}
	}
	Save_EEprom_OverTime();
}
/*******************************************************************************
函 数 名：	Save_EEprom_Time_Update
功能说明： 	更新延时时间
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Save_EEprom_Time_Update(void)
{
	_SetTimeing_ms((u32*)&EEprom_Save_UPST,KEY_DELAY_SAVE_TIME);
	Key_Press_Flag |= 0x01;
}
/*******************************************************************************
函 数 名：	Save_EEprom_OverTime
功能说明： 	超时存储信息到EEPROM
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Save_EEprom_OverTime(void)
{
	if((Key_Press_Flag & 0x01) && (0 == _CheckTime((u32 *)&EEprom_Save_UPST)))
	{
		Key_Press_Flag &= 0xFE;
		Air_StatusCheck();
	}
}
/**************************Copyright BestFu 2014-05-14*************************/	
