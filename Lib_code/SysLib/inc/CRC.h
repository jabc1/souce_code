/***************************Copyright BestFu 2014-05-14*************************
文	件：    CRC.H
说	明:	    CRC校验方法源代码头文件
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Joey
日	期：    2013.7.30
修  改:     暂无
*******************************************************************************/
#ifndef _CRC_H
#define _CRC_H

#include "BF_type.h"

u16 ChkCrcValue(u8 *buf, u16 len);
//u16 ChkCrcValueEx(u8 *buf, u16 cksum, u16 len);
u16 ChkCrcValueEx(u8 *buf, u16 len, u16 FirstData);
#endif
/**************************Copyright BestFu 2014-05-14*************************/
