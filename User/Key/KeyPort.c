/***************************Copyright BestFu 2014-05-14*************************
文	件：	KeyPort.c
说	明：	按键端口配置处理原文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.11.14
修　改：	暂无
*******************************************************************************/
#include "KeyPort.h"
#include "GPIO.h"
#include "BackLight.h"
#include "Thread.h"


static const KeyPort_t KeyPort[] = {{GPIO_B, pin_8},
									{GPIO_B, pin_9},
									{GPIO_C, pin_13},
									{GPIO_C, pin_15},
									{GPIO_C, pin_14},
									{GPIO_A, pin_12},
								};


volatile static u8 First_Press_Key = 0;
/*******************************************************************************
函 数 名：	keyPort_Init
功能说明： 	按键端口初始化函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void KeyPort_Init(void)
{
	u32 i;
	
	for (i = 0; i < (sizeof(KeyPort)/sizeof(KeyPort[1])); i++)
	{
		GPIOx_Cfg(KeyPort[i].port, KeyPort[i].pin, IN_UPDOWN);
	}
}

/*******************************************************************************
函 数 名：	KeyPort_ValueGet
功能说明： 	按键端口状态值获取
参	  数： 	无
返 回 值：	
*******************************************************************************/
u32 KeyPort_ValueGet(void)
{
	u32 i, val;
	
	for (i = (sizeof(KeyPort)/sizeof(KeyPort[1])), val = 0; i > 0; )
	{
		i--;
		val <<= 1;
		if ((!GPIOx_Get(KeyPort[i].port, KeyPort[i].pin)) && ((0 == (First_Press_Key & 0x01))))	//如果按键管脚按下
		{
			if(BACKLIGHT_RD)
			{
				val++;
			}
			else
			{
				Backlight_Run();
				First_Press_Key |= 0x01;
				Thread_Login(ONCEDELAY, 0, 300, &Clr_Flag);
			}
		}
	}
	return val;
}

/*******************************************************************************
函 数 名：	keyPort_Scan
功能说明： 	按键端口扫描
参	  数： 	无
返 回 值：	当前按键值
*******************************************************************************/
u8 KeyPort_Scan(void)
{
	static u8 portVal = KEY_DEFAULT_VALUE, portCnt = 0;
		if (portVal != KeyPort_ValueGet()) 	//按键值更改
		{
			Backlight_Run();
			if ((portCnt > KEY_SHORT_CNT)&&(portCnt <= KEY_LONG_CNT))	//短按事件
			{
				portCnt = 0;
				return portVal;
			}
			else		//清空按键状态
			{
				portCnt = 0;
				portVal = KeyPort_ValueGet();
			}
		}

		else if ((portVal != KEY_DEFAULT_VALUE)							//有按键按下
				&& (++portCnt > (KEY_LONG_CNT + KEY_LONG_SPACE))						//长按事件
				)
		{
			portCnt -= KEY_LONG_SPACE;
			Backlight_Run();
			return portVal;
			}
	return 0;		
}
/*******************************************************************************
函 数 名：	Clr_Flag
功能说明： 	清除标志位
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Clr_Flag(void)
{
	First_Press_Key &= 0xFE;
}
/**************************Copyright BestFu 2014-05-14*************************/	
