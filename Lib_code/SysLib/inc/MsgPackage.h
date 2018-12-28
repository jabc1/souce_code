/***************************Copyright BestFu 2014-05-14*************************
文	件：	MsgPackage.h
说	明：	设备自身消息反馈相关函数头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.24 
修　改：	暂无
*******************************************************************************/
#ifndef __MSGPACKAGE_H
#define __MSGPACKAGE_H

#include "BF_type.h"
#include "command.h"
#include "UnitCfg.h"

#define EVENT						(0x00)
#define COMMUNICATION_VERSION		(0x01)			//通信版本
#define MSG_UPLOAD_ID				(0x88888888)	//消息上报地址

void Msg_Feedback(Communication_t *pMsg);
void Msg_Upload(u8 unit, u8 cmd, u8 len, u8 *data);
void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
void Msg_UploadUnit(u8 unit, u8 cmd, u8 len, u8 *data);
void SaveSerialNum(void);

#endif //MsgPackage.h end 
/**************************Copyright BestFu 2014-05-14*************************/
