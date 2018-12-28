/***************************Copyright BestFu 2014-05-14*************************
文	件：	DeviceRelate.c
说	明：	设备关联属性接口原代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.09.17 
修　改：	暂无
*******************************************************************************/
#include "DeviceRelate.h"

DeviceRelate_t   gDeviceRelate;

BindObject_T gBindObject;    //add zyh 20150429


/*****************************************************************************
函 数 名:  	DeviceRelate_Init
功能说明:  	设备关联初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void DeviceRelate_Init(void)
{
	EEPROM_Read(DEVICE_RELATE_ADDR, sizeof(gDeviceRelate), (u8*)&gDeviceRelate);
	if (gDeviceRelate.UseFlag > 0x02
		|| gDeviceRelate.UnitNum > 0xF0
		)
	{
		DeviceRelate_EEPROMInit();
	}
}

/*****************************************************************************
函 数 名:  	DeviceRelate_EEPROMInit
功能说明:  	设备关联数据初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void DeviceRelate_EEPROMInit(void)
{
	gDeviceRelate.UseFlag 	= 0;
//	gDeviceRelate.UnitNum  	= 0;
	
	EEPROM_Write(DEVICE_RELATE_ADDR, sizeof(gDeviceRelate), (u8*)&gDeviceRelate);		
}


/*****************************************************************************
函 数 名:  	BindObject_EEPROMInit
功能说明:  	绑定对象数据初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void BindObject_EEPROMInit(void)
{
	gBindObject.ObjectType  = 0;
	gBindObject.Addr        = 1;
	EEPROM_Write(BINDOBJECT_ADDR, sizeof(gBindObject), (u8*)&gBindObject);		
}

/*****************************************************************************
函 数 名:  	DeviceRelate_Init
功能说明:  	绑定对象初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void BindObject_Init(void)
{
	EEPROM_Read(BINDOBJECT_ADDR, sizeof(gBindObject), (u8*)&gBindObject);
	if(gBindObject.ObjectType > 0x02 
		|| gBindObject.Addr > 64 
		|| 0 == gBindObject.Addr)
	{
		BindObject_EEPROMInit();
	}
}

/*******************************************************************************
函 数 名：  Set_DeviceRelate_Attr
功能说明：  设置设备关联属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
*******************************************************************************/
MsgResult_t Set_DeviceRelate_Attr(UnitPara_t *pData)
{
    //gDeviceRelate.UseFlag     = *rpara++ ;
    gDeviceRelate.RelateType  = pData->data[0] ;
    gDeviceRelate.ObjectID[0] = pData->data[1] ;
    gDeviceRelate.ObjectID[1] = pData->data[2] ;
    gDeviceRelate.ObjectID[2] = pData->data[3] ;
    gDeviceRelate.ObjectID[3] = pData->data[4] ;

    gDeviceRelate.AssistID[0] = pData->data[5] ;
    gDeviceRelate.AssistID[1] = pData->data[6] ;

    gDeviceRelate.UnitNum     = pData->data[7] ;
    EEPROM_Write(DEVICE_RELATE_ADDR, sizeof(gDeviceRelate), (u8*)&gDeviceRelate);//保存状态
    return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_DeviceRelate_Attr
功能说明：  读写设备关联属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
*******************************************************************************/
MsgResult_t Get_DeviceRelate_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
	
    *rlen = 8;
    *rpara++ = gDeviceRelate.RelateType;
    *rpara++ = gDeviceRelate.ObjectID[0];
    *rpara++ = gDeviceRelate.ObjectID[1];
    *rpara++ = gDeviceRelate.ObjectID[2];
    *rpara++ = gDeviceRelate.ObjectID[3];
    *rpara++ = gDeviceRelate.AssistID[0];
    *rpara++ = gDeviceRelate.AssistID[1];
    *rpara++ = gDeviceRelate.UnitNum;
	
    return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_BindObject_Attr
功能说明：  设置绑定对象属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150429
*******************************************************************************/
MsgResult_t Set_BindObject_Attr(UnitPara_t *pData)
{
	gBindObject.ObjectType = pData->data[0];  //绑定对象
	gBindObject.Addr = pData->data[1];        //控制空调地址
	
	EEPROM_Write(BINDOBJECT_ADDR, sizeof(gBindObject), (u8*)&gBindObject);//保存状态
    return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_BindObject_Attr
功能说明：  获取绑定对象属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150429
*******************************************************************************/
MsgResult_t Get_BindObject_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
    *rlen = 2;
    *rpara++ = gBindObject.ObjectType;
    *rpara++ = gBindObject.Addr;
    return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_DeviceRelateData_Attr
功能说明：  设置设备关联 使能/禁能
参    数：  *pdata     --输入参数
返 回 值：  TURE / FLASE
*******************************************************************************/
MsgResult_t Set_DeviceRelateSwitch_Attr(UnitPara_t *pdata)
{
    gDeviceRelate.UseFlag = pdata->data[0];
	EEPROM_Write(DEVICE_RELATE_ADDR, sizeof(gDeviceRelate), (u8*)&gDeviceRelate);//保存状态
    return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_DeviceRelateSwitch_Attr
功能说明：  读取设备 使能/禁能 状态
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
*******************************************************************************/
MsgResult_t Get_DeviceRelateSwitch_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)   
{
    *rlen = 1;
    *rpara = gDeviceRelate.UseFlag;
    return COMPLETE;
}

/**************************Copyright BestFu 2014-05-14*************************/
