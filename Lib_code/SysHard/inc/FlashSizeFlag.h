/***************************Copyright BestFu 2016-01-12*************************
文	件：    FlashSizeFlag.h
说	明：    用于标识升级文件Flash的大小头文件
编	译：    Keil uVision5 V5.12
版	本：    V1.0
编	写：    Jay
日	期：    2016-01-12
修　改：	无
*******************************************************************************/
#ifndef __FLASHSIZEFLAG_H
#define __FLASHSIZEFLAG_H
#include "SysHard.h"

#if (1u == CURRENT_DEV_FLASH_SIZE)
//Funtion declaration
void FlashSizeFlagInit(void);
#endif

#endif
/**************************Copyright BestFu 2016-01-12*************************/
