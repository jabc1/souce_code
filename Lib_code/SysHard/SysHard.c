/***************************Copyright BestFu 2014-05-14*************************
文	件：    SysHard.c
说	明：    系统硬件相关函数
编	译：   Keil uVision4 V4.54.0.0
版	本：    v2.0
编	写：    Unarty
日	期：    2014.06.26
修　改：	暂无
*******************************************************************************/
#include "SysHard.h"
#include "sys.h"
#include "SysTick.h"
#include "SI4432.h"
#include "I2C.h"
#include "UserData.h"
#include "FlashSizeFlag.h"
#include "program.h"
																				
/*******************************************************************************
函 数 名：	SysHard_Init
功能说明： 	初始化系统硬件相关内容
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void SysHard_Init(void)
{	
#if (1u == CURRENT_DEV_FLASH_SIZE)
	FlashSizeFlagInit();
#endif
	SysTick_Init(72);		//初始化系统时钟
#if (FLASH_ENCRYPTION_EN > 0u)
	FlashEncryptionInit();
#endif
	I2C_Init();				//IIC初始化
	Si4432_Init();			//433无线模块初始化
}

/*******************************************************************************
函 数 名：	HardID_Check
功能说明： 	硬件地址核对
参	  数： 	id: 传入匹配ID
返 回 值：	0（地址匹配成功)/否则返回设备真实ID
*******************************************************************************/
u32 HardID_Check(u32 id)
{
#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
	if(DEVICE_ID_IS_OK)				//如果已设置的新地址合法
    {
        if(RE_DEVICE_ID != id)
        {
            return RE_DEVICE_ID;
        }
    }
    else
#endif
    {
        if (DEVICE_ID != id)
        {
            return DEVICE_ID;
        }
    }//End of if(DEVICE_ID_IS_OK)
	return 0;
}

#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
/*******************************************************************************
函 数 名：	void HardID_Change(void)
功能说明： 	用于新旧地址的更新
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void HardID_Change(void)			/**> 用于新旧地址更新，Jay Add 2015.11.13 **/
{
	if(DEVICE_ID_IS_OK)				//如果已设置的新地址合法
	{
		gSysData.deviceID = RE_DEVICE_ID;
	}
}
#endif

/**************************Copyright BestFu 2014-05-14*************************/
