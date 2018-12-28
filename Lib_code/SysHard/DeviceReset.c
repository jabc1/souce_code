/***************************Copyright BestFu 2014-05-14*************************
文	件：	DeviceReset.c
说	明：	设备重置接口函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.1
修  改:     暂无
*******************************************************************************/
#include "DeviceReset.h"
#include "SysHard.h"
#include "Thread.h"
#include "GPIO.h"
#include "UserData.h"
#include "UnitShare_Attr.h"

#if RESET_USER_DATA
	static void Reset_Check(void);
#endif

/*******************************************************************
函 数 名：	DeviceResetCheck_Init
功能说明： 	设备重置检测初始化
参	  数： 	无
返 回 值：	无
*******************************************************************/
void DeviceResetCheck_Init(void)
{
#if RESET_USER_DATA
	RESET_KEY_INIT;
	Thread_Login(FOREVER, 0, 2000, &Reset_Check);
#endif
}

#if RESET_USER_DATA
/*******************************************************************
函 数 名：	Reset_Check()
功能说明： 	用户数据重置检测
参	  数： 	无
返 回 值：	无
*******************************************************************/
void Reset_Check(void)
{
	static u32 cnt = 0;
	
	if (!RESET_KEY_VAL)	//如果重置按键有效
	{	
		if (!(cnt++))	//第一次进入
		{
			Thread_Login(FOREVER, 0, 20, &Reset_Check);	//加快检测速度
		}
		if (cnt == 100)	//2S
		{
			u8 data[] = {0, 0x90, 1, 0};;
			UserEEPROMData_Init(SecondLevel); //复位用户数据
			Set_UnitChecking_Attr((UnitPara_t*)data);
		}
	}
	else if (0 != cnt)
	{
		DeviceResetCheck_Init();
		cnt = 0;
	}
}
#endif

/**************************Copyright BestFu 2014-05-14*************************/
