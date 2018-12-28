/***************************Copyright BestFu 2014-05-14*************************
文	件：	FlaseAttr.c
说	明：	关于ICFlash数据操作相关属性
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-11-22
修  改:     2014.12.25 Unarty 依据不同存储大小的Flash,重新分区程序代码运行空间大小
*******************************************************************************/
#include "FlashAttr.h"
#include "Updata.h" 
#include "Thread.h"
#include "SI4432.h"
#include "WDG.h"




static u8 UPDATA_LEVEL;     //更新完成级别 F0/F1/F2/F3/F4
static u8 Version;			//数据包升级版本


/*******************************************************************************
函 数 名： 	Set_IAPReady_Attr 
功能说明：  设置升级准备属性
参	  数:   data[0]: 目标单元
			data[1]: 参数长度
返 回 值：  消息执行结果
*******************************************************************************/
MsgResult_t Set_IAPReady_Attr(UnitPara_t *pData)
{
	if (Updata_Check(SOFT->version, (u32*)&pData->data[2], (u32*)&pData->data[6]))	//升级条件核对
	{
		Soft_t	iapFlag;
		
		iapFlag.name = *(u16*)&pData->data[0];
		iapFlag.appSize = *(u16*)&pData->data[10];
		iapFlag.version = *(u32*)&pData->data[13];
		if (SOFT->appAddress == APPADDR_A)//当前软件运行于A程序区
		{
			Version =  0x55;
			switch (*(u16*)USER_FLASH_AREA)	//IC Flash大小
			{
				case 64:			//64K
					iapFlag.appAddress = 0x08008400;
					break;
				case 128:			//128k
					iapFlag.appAddress = 0x0800C400;
					break;
				case 256:			//256k
					iapFlag.appAddress = 0x08019800;
					break;
				case 512:			//512K
					iapFlag.appAddress = 0x08032800;
					break;
				default : 
					iapFlag.appAddress = 0x08008400;
					break;
			}
		}
		else
		{
			Version =  0xAA;
			iapFlag.appAddress = APPADDR_A;
		}
		
		if (Updata_Ready(pData->data[12], &iapFlag))	//升级准备完成
		{
			UPDATA_LEVEL = 0xF1;
			pData->len = 7;
			*(u16*)&pData->data[0] = SOFT->name;
			*(u32*)&pData->data[2] = SOFT->version;
			*(u8*)&pData->data[6] = Version;
			
			return OWN_RETURN_PARA;
		}		
	}

	return PARA_MEANING_ERR;
}


/*******************************************************************************
函 数 名： 	Set_IAPPackSave_Attr
功能说明：  升级数据包保存
参	  数:   data[0]: 目标单元
			data[1]: 参数长度       
返 回 值： 	消息执行结果
*******************************************************************************/
MsgResult_t Set_IAPPackSave_Attr(UnitPara_t *pData)
{
	WDG_Feed();
	
    if (UPDATA_LEVEL == 0xF1)	//F0
	{
		if (pData->data[0] == Version)
		{
			Updata_PackProcess(*(u16*)&pData->data[1], pData->len - 3, (u8*)&pData->data[3]);//数据包处理
		}	
		return COMPLETE;
	}
	else if (UPDATA_LEVEL == 0xF2)
	{
		return COMPLETE;
	}
	else
	{
		UPDATA_LEVEL = 0;
	}
	
	return NO_ACK;
}

/*******************************************************************************
函 数 名:  	Get_IAPPackCheck_Attr
功能说明:  	获取设备属性
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_IAPPackCheck_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	WDG_Feed();
	
    if (UPDATA_LEVEL == 0xF1)
	{
		if (Updata_PackCheck(rlen, (u16*)rpara))	//没有掉包
		{
			UPDATA_LEVEL = 0xF2;
			return COMPLETE;
		}
		return ((MsgResult_t)0);
	}
	else if (UPDATA_LEVEL == 0xF2)
	{
		return COMPLETE;	
	}
	else
	{
		UPDATA_LEVEL = 0;
	}

	return NO_ACK;
}

/*******************************************************************************
函 数 名:  	Set_IAPDataCRC_Attr
功能说明:  	获取升级数据校验结果
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_IAPDataCRC_Attr(UnitPara_t *pData)
{
	if (UPDATA_LEVEL == 0xF2)
	{
		*(u8*)&pData->data[4] ^= *(u8*)&pData->data[5];
		*(u8*)&pData->data[5] ^= *(u8*)&pData->data[4];
		*(u8*)&pData->data[4] ^= *(u8*)&pData->data[5];

		if (Updata_Calibrate(*(u32*)&pData->data[0], *(u16*)&pData->data[4]) == TRUE)	//校验通过
		{
			if (Updata_Flag(FLAGADDR))	//修改更新标识区
			{
				if(PartionChange())
				{
					UPDATA_LEVEL = 0xF3;
					WDG_FeedOFF();	//关闭喂狗，设备自动进入重启
					return COMPLETE;
				}
			} 	
		}
	}
	else if (UPDATA_LEVEL == 0xF3)
	{	
		return COMPLETE;
	}
	UPDATA_LEVEL = 0;

	return CMD_EXE_ERR;
}

/*******************************************************************************
函 数 名： 	Set_433Channel_Attr
功能说明：  设置433信道属性
参	  数:   data[0]: 目标单元
			data[1]: 参数长度       
返 回 值：  消息执行结果
*******************************************************************************/
MsgResult_t Set_433Channel_Attr(UnitPara_t *pData)
{
	if ((pData->len != 1))	//参数区长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] < CHANNEL_MIN     //参数区内容不合法
		|| pData->data[0] > CHANNEL_MAX
		)
	{
		return PARA_MEANING_ERR;
	}
	
	if (pData->data[0]  != Channel_Get())
	{
		if (Updata_Channel(pData->data[0]) == FALSE)	//更新信道失败
		{		
			return CMD_EXE_ERR;
		}
		Thread_Login(ONCEDELAY, 0, 800, &Si4432_Init);	//关闭喂狗，设备自动进入重启
	}

	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_433Channel_Attr
功能说明:  	获取433信道属性
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_433Channel_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rpara = Channel_Get();		//信道值
	*rlen = 1;
	
	return COMPLETE;
}

/**************************Copyright BestFu 2014-05-14*************************/
