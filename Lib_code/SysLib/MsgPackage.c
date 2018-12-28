/***************************Copyright BestFu 2014-05-14*************************
文	件：	MsgPackage.c
说	明：	设备自身消息反馈相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.24 
修　改：	暂无
*******************************************************************************/
#include "MsgPackage.h"
#include "UserData.h"
#include "BestFuLib.h"
#include "cmdProcess.h"
#include "FaultManage.h"
//static u8 serialNum;	//消息流水号

static void Msg_Package(CMDAim_t *aim, CMDPara_t *para, u8 *data);

/*******************************************************************************
函 数 名：	SaveSerialNum
功能说明： 	消息反馈
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void SaveSerialNum(void)
{
	SysEEPROMData_Set(sizeof(gSysData.serialNum) , (u8*)&gSysData.serialNum);	
}

/*******************************************************************************
函 数 名：	Msg_Feedback
功能说明： 	消息反馈
参	  数： 	pMsg:	消息内容
返 回 值：	TRUE(重复)/FALSE(不重复)
*******************************************************************************/
void Msg_Feedback(Communication_t *pMsg)
{
	FIFO_t *aimFIFO;
	u32 tmp_serialNum ; //yanhuan adding 2015/10/20 防止回馈信息的流水号被改变
	pMsg->version = COMMUNICATION_VERSION;
	pMsg->aim.userID = gSysData.userID;
	pMsg->aim.sourceID = gSysData.deviceID;
	
	if (pMsg->aim.object.id == gSysData.deviceID)	//如果反馈目标为自己
	{
		pMsg->aim.serialNum = ++gSysData.serialNum;
		aimFIFO = &rcv433fifo;
	}
	else if(pMsg->aim.objectType > SINGLE_ACT)// 表示为广播，无论哪种广播都要往自己的发送队列，周旺添加2015.9.28
	{
		tmp_serialNum = pMsg->aim.serialNum ;
		pMsg->aim.serialNum = ++gSysData.serialNum;
		aimFIFO = &rcv433fifo;
		if(FALSE == cmd_put(aimFIFO, (u8*)pMsg, sizeof(Communication_t) + pMsg->para.len))//判断433接收队列是否溢出
			Fault_Upload(FAULT_2 , RECV_433_FIFO_OVER , NULL);//暂时不做处理 yanhuan adding 2015/10/10
		pMsg->aim.serialNum = tmp_serialNum ;
		aimFIFO = &send433fifo;
	}
	else
	{
		aimFIFO = &send433fifo;
	}	
	if(FALSE == cmd_put(aimFIFO, (u8*)pMsg, sizeof(Communication_t) + pMsg->para.len)) //判断433队列是否溢出 
	{                                           //yanhuan adding 2015/10/10
		if(aimFIFO == &send433fifo)
			Fault_Upload(FAULT_2 , SEND_433_FIFO_OVER , NULL);//暂时不做处理
		else if(aimFIFO == &rcv433fifo)
			Fault_Upload(FAULT_2 , RECV_433_FIFO_OVER , NULL);//暂时不做处理
	}	
}
	
/*******************************************************************************
函 数 名：	Msg_Upload
功能说明： 	消息上报
参	  数： 	unit:	单元号
			cmd：	属性号
			len:	参数长度
			*data:	参数内容
返 回 值：	无
*******************************************************************************/
void Msg_Upload(u8 unit, u8 cmd, u8 len, u8 *data)
{
	u8 buf[64];
	
	buf[0] = COMPLETE;	//增加通信成功标识
	buf[1] = unit;
	buf[2] = cmd;
	buf[3] = len;
	memcpy(&buf[4], data, len);
	
	Msg_UploadUnit( 0x00, 0x00, len + 4, buf);
}

/*******************************************************************************
函 数 名：	Msg_UploadUnit
功能说明： 	单元消息上报
参	  数： 	unit:	单元号
			cmd：	属性号
			len:	参数长度
			*data:	参数内容
返 回 值：	无
*******************************************************************************/
void Msg_UploadUnit(u8 unit, u8 cmd, u8 len, u8 *data)
{
	CMDAim_t aim;
	CMDPara_t para;
	
	aim.objectType = SINGLE_ACT;
	aim.object.id = MSG_UPLOAD_ID;
	aim.actNum = 0;
	
	para.msgType = EVENT;
	para.unit = unit;
	para.cmd = cmd;
	para.len = len;
	Msg_Package(&aim, &para, data);
}

/*******************************************************************************
函 数 名：	Msg_Send
功能说明： 	消息发送
参	  数： 	objectType:		目标地址类型
			objectID：		目标地址
			actNum:			广播类型号
			unit:			单元号
			cmd：			属性号       
			len:			参数长度
			*data:			参数内容
返 回 值：	无
*******************************************************************************/
void Msg_Send(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data)
{
	CMDAim_t aim;
	CMDPara_t para;
	
	aim.objectType = objecttype;
	aim.object.id = objectID;			//设置目标地址
	aim.actNum = actNum;	
	
	/*消息参数区赋值*/
	para.msgType = WRITENACK;			//消息类型，事件
	para.cmd = cmd;
	para.unit = unit;
	para.len = len;
	Msg_Package(&aim, &para, data);
}

/*******************************************************************************
函 数 名：	Msg_Package
功能说明： 	消息打包
参	  数： 	aim:	目标描述
			para:	指令描述
			*data:	参数内容
返 回 值：	无
*******************************************************************************/
void Msg_Package(CMDAim_t *aim, CMDPara_t *para, u8 *data)
{
	u8 msg[250];
	Communication_t *pMsg = (Communication_t*)msg;
	
	memcpy(&pMsg->aim, aim, sizeof(CMDAim_t));
	pMsg->aim.serialNum = ++gSysData.serialNum;		//设置消息流水号
	
	/*消息参数区赋值*/
	memcpy(&pMsg->para, para, sizeof(CMDPara_t));
	memcpy(&msg[sizeof(Communication_t)], data, para->len);
	Msg_Feedback(pMsg);
}

/**************************Copyright BestFu 2014-05-14*************************/
