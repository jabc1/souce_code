/***************************Copyright BestFu 2014-05-14*************************
文	件：	FlaseAttr.h
说	明：	关于ICFlash数据操作相关属性操作头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-11-22
修  改:     暂无
*******************************************************************************/
#ifndef __FLASHATTR_H
#define __FLASHATTR_H

#include "program.h"
#include "Updata.h" 
#include "UnitCfg.h"


MsgResult_t Set_IAPReady_Attr(UnitPara_t *data);
MsgResult_t Set_IAPPackSave_Attr(UnitPara_t *data);
MsgResult_t Get_IAPPackCheck_Attr(UnitPara_t *data, u8 *rlen, u8 *rpara);
MsgResult_t Set_IAPDataCRC_Attr(UnitPara_t *data);
MsgResult_t Set_433Channel_Attr(UnitPara_t *data);
MsgResult_t Get_433Channel_Attr(UnitPara_t *data, u8 *rlen, u8 *rpara);

#endif //IAP.h
/**************************Copyright BestFu 2014-05-14*************************/

