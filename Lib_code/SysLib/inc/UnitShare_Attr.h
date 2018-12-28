/***************************Copyright BestFu 2014-05-14*************************
文	件：	UnitShare_Attr.h
说	明：	个单元共用属性实现函数头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014-04-08
修　改：	暂无
*******************************************************************************/
#ifndef __UNITSHARE_ATTR_H
#define __UNITSHARE_ATTR_H

#include "UnitCfg.h"

#define MAX_ATTR_NUM	    90//最大单元属性数
#define MAX_ATTRPARA_LEN	10//最大属性参数长度

MsgResult_t Set_ManyUnit_Attr(UnitPara_t *pData);
MsgResult_t Get_ManyUnit_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);
MsgResult_t Set_UnitChecking_Attr(UnitPara_t *data);

#endif //UnitShare_Attr.h
/**************************Copyright BestFu 2014-05-14*************************/
