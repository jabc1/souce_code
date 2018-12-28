/***************************Copyright BestFu 2014-05-14*************************
文	件：	Air_Attr.h
说	明：	空调属性接口头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.09.17 
修　改：	暂无
*******************************************************************************/
#ifndef __AIRATTR_H
#define __AIRATTR_H

#include "UnitCfg.h"

extern const AttrExe_st AirAttrTab[];

void Air_ParaInit(u8 unitID);
void AirEepromInit(u8 unitID);
void Air_Instruct(void);
void Air_StatusShow_thread(void);

MsgResult_t Set_AirFind_Attr(UnitPara_t *pData);

MsgResult_t Set_AirBrand_Attr(UnitPara_t *pData);
MsgResult_t Get_AirBrand_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

MsgResult_t Set_AirStatus_Attr(UnitPara_t *pData);
MsgResult_t Get_AirStatus_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

MsgResult_t Set_AirTimer_Attr(UnitPara_t *pData);

MsgResult_t Set_DeviceRelate_Attr(UnitPara_t *pData);
MsgResult_t Get_DeviceRelate_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  ;

MsgResult_t Set_DeviceRelateSwitch_Attr(UnitPara_t *pdata);
MsgResult_t Get_DeviceRelateSwitch_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)   ;


MsgResult_t Set_MaxMinTemperature_Attr(UnitPara_t *pdata);
MsgResult_t Get_MaxMinTemperature_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)   ;

MsgResult_t Set_RS232Cfg_Attr(UnitPara_t *pData);
MsgResult_t Get_RS232Cfg_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara);

MsgResult_t Set_Correction_Temperature_Attr(UnitPara_t *pData);
MsgResult_t Get_Correction_Temperature_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara);

MsgResult_t Set_Air_Type_Attr(UnitPara_t *pData);
MsgResult_t Get_Air_Type_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara);

MsgResult_t Set_Air_Addr_Attr(UnitPara_t *pData);
MsgResult_t Get_Air_Addr_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara) ;

MsgResult_t Set_Air_List_Addr_Attr(UnitPara_t *pData);
MsgResult_t Get_Air_List_Addr_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara);
#endif		//AirAttr.h end
/**************************Copyright BestFu 2014-05-14*************************/
