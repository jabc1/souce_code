/***************************Copyright BestFu 2014-05-14*************************
文	件：	DeviceRelate.h
说	明：	设备关联头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.11.17 
修　改：	暂无
*******************************************************************************/
#ifndef __DEVICERELATE_H
#define __DEVICERELATE_H

#include "UnitCfg.h"

#define DEVICE_RELATE_ADDR	(0x3C00)		//空调状态存储地址
#define BINDOBJECT_ADDR	    (0x3D00)		//绑定对象存储地址
#define CORRECTION_TEMPERATURE_ADDR	    (0x6200)		//校正环境温度
#define RS485_PARA_ADDR	    (0x6100)		//校正环境温度
//设备关联结构体
typedef  struct 
{
    u8 UseFlag;         //使用标志  0x00.未使用   0x01.使能          0x02.禁能              
    u8 RelateType;      //对象类型  0x00.未关联   0x01.设备单节点    0x02组播         0x04.设备类型广播                      
    u8 ObjectID[4];     //对象地址                |--设备节点ID      |-- 组区域地址   |-- 设备类型广播地址
    u8 AssistID[2];     //辅助地址                |--设备类型        |-- 组号         |-- 设备类型
    u8 UnitNum;			//单元号
}DeviceRelate_t;

typedef struct
{
	u8 ObjectType;
	u8 Addr;
}BindObject_T;



extern BindObject_T gBindObject;    //add zyh 20150429
extern DeviceRelate_t   gDeviceRelate;


void DeviceRelate_Init(void);
void DeviceRelate_EEPROMInit(void);

extern void BindObject_Init(void);
extern void BindObject_EEPROMInit(void);

MsgResult_t Set_DeviceRelate_Attr(UnitPara_t *pData);
MsgResult_t Get_DeviceRelate_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  ;

MsgResult_t Set_DeviceRelateSwitch_Attr(UnitPara_t *pdata);
MsgResult_t Get_DeviceRelateSwitch_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)   ;


MsgResult_t Set_BindObject_Attr(UnitPara_t *pdata);
MsgResult_t Get_BindObject_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)   ;



#endif		//DeviceRelate.h end
/**************************Copyright BestFu 2014-05-14*************************/
