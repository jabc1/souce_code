/***************************Copyright BestFu 2016-01-12*************************
文	件：    FlashSizeFlag.c
说	明：    用于标识升级文件Flash的大小
编	译：    Keil uVision5 V5.12
版	本：    V1.0
编	写：    Jay
日	期：    2016-01-12
修　改：	无
*******************************************************************************/
#include "FlashSizeFlag.h"
#include "BF_type.h"

#if (1u == CURRENT_DEV_FLASH_SIZE)
const u8 flashFlag[] = "I'm 128k memory device";
#endif

#if (1u == CURRENT_DEV_FLASH_SIZE)
/*******************************************************************************
函 数 名：	void FlashSizeFlagInit(void)
功能说明： 	增加Flash大小标记
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void FlashSizeFlagInit(void)
{
	const u8* p = flashFlag;
	if(NULL == p)
	{
		return ;
	}
}
#endif

/**************************Copyright BestFu 2016-01-12*************************/
