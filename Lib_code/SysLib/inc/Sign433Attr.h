/***************************Copyright BestFu ***********************************
**  文    件：  Sign433Attr.h
**  功    能：  433信号强度测试属性接口层
**  编    译：  Keil uVision5 V4.72
**  版    本：  V1.1
**  编    写：  Seven
**  创建日期：  2014.10.08
**  修改日期：  2014.10.08
**  说    明：  >>串口数据流进入 --- 433信号
*******************************************************************************/
#ifndef _SIGN433_ATTR_H_
#define _SIGN433_ATTR_H_

#include "UnitCfg.h"

extern MsgResult_t Set_Sign433_StartSendFrame_Attr(UnitPara_t *pData);
extern MsgResult_t Get_Sign433_StartSendFrame_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

extern MsgResult_t Set_Sign433_StartRecFrame_Attr(UnitPara_t *pData);
extern MsgResult_t Get_Sign433_StartRecFrame_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

extern MsgResult_t Set_Sign433_RecFrameStream_Attr(UnitPara_t *pData);

extern MsgResult_t Set_Sign433_TestState_Attr(UnitPara_t *pData);
extern MsgResult_t Get_Sign433_TestState_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

extern MsgResult_t Get_Sign433_RcvData_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);
#endif
/***************************Copyright BestFu **********************************/
