/***************************Copyright BestFu 2014-05-14*************************
文	件：    SysExeAttr.c
说	明：    系统级属性命令执行表，所有命令的实现
编	译：    Keil uVision4 V4.54.0.0
版	本：    v2.0
编	写：    Unarty
日	期：    2014.06.26
修　改：	李诺淮 2014.10.22 添加无线包测试接口 B0～BF
			Unarty 2014.12.01 删除0xCE 与0xCF 属性，（此接口是为兼容V1.00.00版本而预留的，现已完全去除此接口）
*******************************************************************************/
#include "SysExeAttr.h"
#include "UnitShare_Attr.h"
#include "Scene&Group.h"
#include "FlashAttr.h"
#include "SysTestAttr.h"
#include "Upload.h"
#include "linkWRDC.h"
#include "UserDataDownload.h"
#include "Sign433Attr.h"
#include "FaultManage.h"
#include "SysHard.h"
#include "Thread.h"
/*属性读写列表*/
const AttrExe_st SysAttrTable[] =
{
	{0x00, LEVEL_0, Set_ManyUnit_Attr				, Get_ManyUnit_Attr					},

	{0x90, LEVEL_0, Set_UnitChecking_Attr			, NULL								},

	{0xA0, LEVEL_0, Set_Scene_Attr					, Get_Scene_Attr					},
	{0xA1, LEVEL_0, Set_Group_Attr					, Get_Group_Attr					},
	{0xA2, LEVEL_0, Set_UnitArea_Attr				, Get_UnitArea_Attr					},
	{0xAF, LEVEL_0, Set_RepeatBroadcast_Attr		, Get_RepeatBroadcast_Attr			},	//周旺增加2015.9.9
	{0xB0, LEVEL_0, Set_Sign433_StartSendFrame_Attr , Get_Sign433_StartSendFrame_Attr	},	//启动发送测试
	{0xB1, LEVEL_0, Set_Sign433_StartRecFrame_Attr  , Get_Sign433_StartRecFrame_Attr    },	//启动接收测试
	{0xB2, LEVEL_0, Set_Sign433_RecFrameStream_Attr	, NULL					            },	//接收测试数据帧流
//{0XB3,0XB4 } 预留
	{0xBE, LEVEL_0, NULL							, Get_DeviceThreshold_Attr			},	//获取设备通信门槛值 yanhuan adding 2015/12/02
	{0xBF, LEVEL_0, Set_Sign433_TestState_Attr		, Get_Sign433_TestState_Attr		},	//中断测试
	
	{0xC0, LEVEL_0, NULL							, Get_DeviceType_Attr				},
	{0xC1, LEVEL_0, NULL							, Get_Version_Attr					},
	{0xC2, LEVEL_0, Set_UserID_Attr					, Get_UserID_Attr					},
	{0xC3, LEVEL_0, NULL							, Get_DeviceAddr_Attr				},
	{0xC4, LEVEL_0, Set_CmdAckNum_Attr				, Get_CmdAckNum_Attr				},
	{0xC5, LEVEL_0, Set_RepeatBandAddr_Attr			, Get_RepeatBandAddr_Attr			},
	
	{0xC9, LEVEL_0, Set_UserDataFormat_Attr			, NULL								},
//	{0xCE, LEVEL_0, NULL							, Get_DeviceInfo_Attr				},	//2014.12.01 Unarty　
//	{0xCF, LEVEL_0, Set_DeviceAdd_Attr				, NULL								},

	{0xD2, LEVEL_1, Set_UnitAble_Attr				, Get_UnitAble_Attr					},
	{0xD4, LEVEL_0, Set_UnitType_Attr				, Get_UnitType_Attr					},
	{0xD5, LEVEL_0, Set_CommonType_Attr				, Get_CommonType_Attr				},

	{0xE0, LEVEL_0, Set_Link_Attr					, Get_Link_Attr						},
	{0xE3, LEVEL_0, Set_DelLinkForUnit_Attr			, NULL								},

	{0xEA, LEVEL_0,	UserDataDownload_Ready			, NULL								},
	{0xEB, LEVEL_0, UserDataFrame_Save				, UserDataFrame_Check				},	
#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)	
	{0xEC, LEVEL_0, Set_ResetAddr_Attr				, Get_ResetAddr_Attr				},	//重设地址 Jay Add 2015.11.13
#endif	
	{0xF0, LEVEL_0, Set_IAPReady_Attr				, NULL								},
	{0xF1, LEVEL_0, Set_IAPPackSave_Attr			, Get_IAPPackCheck_Attr				},
	{0xF2, LEVEL_0, Set_IAPDataCRC_Attr				, Get_IAPPackCheck_Attr				},
	{0xF3, LEVEL_0, Set_IAPDataCRC_Attr				, NULL								},
	{0xF8, LEVEL_0, Set_433Channel_Attr				, Get_433Channel_Attr				},
	
	{0xFA, LEVEL_0, HardWDGTest_Attr				, NULL								},
	{0xFB, LEVEL_0, SI4432Test_Attr					, NULL								},
	{0xFD, LEVEL_0, NULL			          		, Get_Eeprom_Info_Attr				},	//EEPROM数据读取接口 yanhuan adding 2015/10/10
	{0xFE, LEVEL_0, Set_FaultFlag_Attr				, Get_FaultFlag_Attr				},
};
/*******************************************************************************
函 数 名:  Upload_RESTART
功能说明:  设备上报重启
参    数:  无
返 回 值:  无
*******************************************************************************/
void Upload_RESTART(void)
{
	Upload_Fault(RESTART);	//上报设备重启
}
/*******************************************************************************
函 数 名:  SysAttr_Init
功能说明:  系统属性初始化
参    数:  unitID:	单元号
返 回 值:  无
*******************************************************************************/
void SysAttr_Init(u8 unitID)
{
	UserData_Init();
	UserDataDownload_Init();
	LinkInit();
	PropFifoInit();
	Fault_Upload(FAULT_2 , DEV_RESTART , NULL); //设备重启次数记录保存     yanhuan adding 2015/10/10
	Thread_Login(ONCEDELAY , 0 ,1000 , Upload_RESTART); //1秒之后上报重启  yanhuan adding 2015/12/02
}

/*******************************************************************************
函 数 名:  	Get_Version_Attr
功能说明:  	获取版本信息
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_Version_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u8 *p; 
	
	*rlen = 0;
	*(u16*)&rpara[*rlen] = SYS_VERSION;		//系统库版本
	*rlen += 2;
	*(u32*)&rpara[*rlen] = SOFT->version;	//软件版本
	*rlen += sizeof(SOFT->version);
	*(u32*)&rpara[*rlen] = SOFT->appAddress; //软件运行地址
	*rlen += sizeof(SOFT->appAddress);
	*(u32*)&rpara[*rlen] = HARD->hardVer;	//硬件版本
	*rlen += sizeof(HARD->hardVer);
	
	for (p = HARD->str; (*p <= 'Z')&&(*p >= '0'); p++)	//硬件描述信息
	{
		rpara[((*rlen)++)] = *p;		
	}
	
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Get_DeviceType_Attr
功能说明:  	获取设备类型
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_DeviceType_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if (!(pData->len)	//全局查找
		|| (SOFT->name == *(u16*)&pData->data[0])	//按设备类型查找
		|| (0xFFFF == *(u16*)&pData->data[0])		//全类型查找   
		)
	{
		*rlen = 0;
		*(u16*)&rpara[*rlen] = SOFT->name;
		*rlen += 2;
		*(u32*)&rpara[*rlen] = SOFT->version;
		*rlen += 4;

		return COMPLETE;
	}
			
	return CMD_EXE_ERR;
}

/*******************************************************************************
函 数 名：  Set_UserID_Attr
功能说明：  设置用户号
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UserID_Attr(UnitPara_t *pData)
{
	if ((pData->len > 4)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	gSysData.userID = *(u32*)&pData->data[0];
	if (SysEEPROMData_Set(sizeof(gSysData.userID) , (u8*)&gSysData.userID) == FALSE)
	{
		return EEPROM_ERR;
	}
	
	return COMPLETE;
	
}

/*******************************************************************************
函 数 名:  	Get_UserID_Attr
功能说明:  	获取用户号
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_UserID_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{	
	*(u32*)rpara = gSysData.userID;
	*rlen = sizeof(gSysData.userID);
	
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Get_DeviceAddr_Attr
功能说明:  	获取设备设备地址
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_DeviceAddr_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*(u32*)rpara = gSysData.deviceID;
	*rlen = sizeof(gSysData.deviceID);
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_CmdAckNum_Attr
功能说明：  设置指令应答次数
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_CmdAckNum_Attr(UnitPara_t *pData)
{
	if ((pData->data[0] > 5)||((!pData->data[0])))//参数内容不合法
	{
		return PARA_LEN_ERR;
	}
	gSysData.sendCnt = pData->data[0];
	if (SysEEPROMData_Set(sizeof(gSysData.sendCnt) , (u8*)&gSysData.sendCnt) == FALSE)
	{
		return EEPROM_ERR;
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_CmdAckNum_Attr
功能说明:  	获取指令应答次数
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_CmdAckNum_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rpara = gSysData.sendCnt;
	*rlen = sizeof(gSysData.sendCnt);

	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_RepeatBandAddr_Attr
功能说明：  设置转发绑定地址
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_RepeatBandAddr_Attr(UnitPara_t *pData)
{
	u8 i,cnt;
	if ( (pData->len > 25) 
		|| ((pData->len%5)) )//参数长度不合法,只有5，10，15，20，25是正常的
	{
		return PARA_LEN_ERR;
	}
	
	cnt = pData->len / 5;
	
	for(i=0; i<cnt; i++)
	{
		if(pData->data[i*5] == 0 
			|| pData->data[i*5] > 5)
		{
			return PARA_MEANING_ERR;
		}
	}
	
	for(i=0; i<cnt; i++)
	{
		gSysData.BandAddr[pData->data[i*5]-1] = *(u32*)&pData->data[i*5+1];
	}
	
	if (SysEEPROMData_Set(sizeof(gSysData.BandAddr) , (u8*)&gSysData.BandAddr) == FALSE)
	{
		return EEPROM_ERR;
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_RepeatBandAddr_Attr
功能说明:  	获取转发绑定地址
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_RepeatBandAddr_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u8 i,j=0;

	if(pData->data[0] == 0xFF)
	{
		//全读
		*rlen = 0;
		for(i=0; i<5; i++)
		{
			if(gSysData.BandAddr[i] 
				&& gSysData.BandAddr[i] != 0xFFFFFFFF)
			{
				*(rpara + j*5) = i + 1;
				*(u32 *)(rpara + 1 + j*5) = gSysData.BandAddr[i];
				*rlen += 5;
				j++;
			}
		}
	}
	else if(pData->data[0] == 0 || pData->data[0] > 5)
	{
		return PARA_LEN_ERR;
	}
	else
	{
		*rpara = pData->data[0];
		*(u32 *)(rpara+1) = gSysData.BandAddr[pData->data[0]-1];
		*rlen = 5;
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_RepeatBroadcast_Attr
功能说明：  设置转发符合本区域的广播功能：非1为使能，0为禁能
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_RepeatBroadcast_Attr(UnitPara_t *pData)
{
	gSysData.BandAreaEn = pData->data[0] ? 1 : 0;
	
	if (SysEEPROMData_Set(sizeof(gSysData.BandAreaEn) , (u8*)&gSysData.BandAreaEn) == FALSE)
	{
		return EEPROM_ERR;
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_RepeatBroadcast_Attr
功能说明:  	获取转发符合本区域的广播功能：1为使能，0为禁能
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_RepeatBroadcast_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rpara = (gSysData.BandAreaEn == 1) ? 1 : 0;
	*rlen = sizeof(gSysData.BandAreaEn);

	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_Area_Attr
功能说明：  设置单元区域
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UnitArea_Attr(UnitPara_t *pData)
{
	
	if ((pData->len > 5)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1; i < UnitCnt_Get(); i++)
		{
			*(u32*)gUnitData[i].area = *(u32*)&pData->data[1];
			if (UnitPublicEEPROMData_Set(sizeof(gUnitData[i].area) , gUnitData[i].area) == FALSE)
			{
				return EEPROM_ERR;
			}
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}
	else
	{
		*(u32*)gUnitData[pData->data[0]].area = *(u32*)&pData->data[1];
		if (UnitPublicEEPROMData_Set(sizeof(gUnitData[pData->data[0]].area) , gUnitData[pData->data[0]].area) == FALSE)
		{
			return EEPROM_ERR;
		}
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_Area_Attr
功能说明:  	获取单元区域
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_UnitArea_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if ((pData->len > 3)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1, *rlen = 0; i < UnitCnt_Get(); i++)
		{
			*(u32*)&rpara[*rlen] = *(u32*)gUnitData[i].area;
			*rlen += sizeof(gUnitData[i].area);
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}		
	else
	{
		*(u32*)rpara = *(u32*)gUnitData[pData->data[0]].area;
		*rlen = sizeof(gUnitData[pData->data[0]].area);
	}
	
	return COMPLETE;
}
	
/*******************************************************************************
函 数 名：  Set_UnitType_Attr
功能说明：  设置单元类型
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UnitType_Attr(UnitPara_t *pData)
{
	if ((pData->len > 3)||(!pData->len))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1; i < UnitCnt_Get(); i++)
		{
			gUnitData[i].type = *(u16*)&pData->data[1];
			if (UnitPublicEEPROMData_Set(sizeof(gUnitData[i].type) , (u8*)&gUnitData[i].type) == FALSE)
			{
				return EEPROM_ERR;
			}
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}
	else
	{
		gUnitData[pData->data[0]].type = *(u16*)&pData->data[1];
		if (FALSE == (UnitPublicEEPROMData_Set(sizeof(gUnitData[pData->data[0]].type) , (u8*)&gUnitData[pData->data[0]].type)))
		{
			return EEPROM_ERR;
		}
	}
		
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_UnitType_Attr
功能说明:  	获取单元类型
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_UnitType_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if ((pData->len > 3)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1, *rlen = 0; i < UnitCnt_Get(); i++)
		{
			*(u16*)&rpara[*rlen] = gUnitData[i].type;
			*rlen += sizeof(gUnitData[i].type);
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}		
	else
	{
		*(u16*)rpara = gUnitData[pData->data[0]].type;
		*rlen = sizeof(gUnitData[pData->data[0]].type);
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_CommonType_Attr
功能说明：  设置单元应用大类
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_CommonType_Attr(UnitPara_t *pData)
{
	if ((pData->len > 2)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1; i < UnitCnt_Get(); i++)
		{
			gUnitData[i].common = pData->data[1];
			if (FALSE == (UnitPublicEEPROMData_Set(sizeof(gUnitData[i].common) , (u8*)&gUnitData[i].common)))
			{
				return EEPROM_ERR;
			}
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}
	else
	{
		gUnitData[pData->data[0]].common = pData->data[1];
		if (UnitPublicEEPROMData_Set(sizeof(gUnitData[pData->data[0]].common) , (u8*)&gUnitData[pData->data[0]].common) == FALSE)
		{
			return EEPROM_ERR;
		}
	}
		
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_CommonType_Attr
功能说明:  	获取单元应用大类
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_CommonType_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if ((pData->len > 2)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1, *rlen = 0; i < UnitCnt_Get(); i++)
		{
			rpara[*rlen] = gUnitData[i].common;
			*rlen += sizeof(gUnitData[i].common);
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}		
	else
	{
		*rpara = gUnitData[pData->data[0]].common;
		*rlen = sizeof(gUnitData[pData->data[0]].common);
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_UnitAble_Attr
功能说明：  设置单元使能标记
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UnitAble_Attr(UnitPara_t *pData)
{
	if ((pData->len > 5)||((!pData->len)))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if (pData->data[0] == 0x00)	//全单元操作
	{
		u8 i;
		for (i = 1; i < UnitCnt_Get(); i++)
		{
			gUnitData[i].able = pData->data[1];
			if (UnitPublicEEPROMData_Set(sizeof(gUnitData[i].able) , (u8*)&gUnitData[i].able) == FALSE)
			{
				return EEPROM_ERR;
			}
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}	
	else 
	{
		gUnitData[pData->data[0]].able = pData->data[1];
		if (UnitPublicEEPROMData_Set(sizeof(gUnitData[pData->data[0]].able) , (u8*)&gUnitData[pData->data[0]].able) == FALSE)
		{
			return EEPROM_ERR;
		}
	}
		
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_UnitAble_Attr
功能说明:  	获取单元使能标记
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_UnitAble_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if ((pData->len > 3))//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	else if ((pData->data[0] == 0x00)||(!pData->len))	//全单元操作
	{
		u8 i;
		for (i = 1, *rlen = 0; i < UnitCnt_Get(); i++)
		{
			rpara[*rlen] = gUnitData[i].able;
			*rlen += sizeof(gUnitData[i].able);
		}
	}
	else if (pData->data[0] >= UnitCnt_Get())	//参数内容不合法
	{
		return PARA_MEANING_ERR;
	}		
	else
	{
		*rpara = gUnitData[pData->data[0]].able;
		*rlen = sizeof(gUnitData[pData->data[0]].able);
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_Eeprom_Info_Attr
功能说明:  	读取EEPROM数据的接口
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
注    意：  EEPROM按照256kb计算，地址从0x0000 - 0x7FFF
*******************************************************************************/
MsgResult_t Get_Eeprom_Info_Attr(UnitPara_t *data, u8 *rlen, u8 *rpara)
{
	u16 addr ;
	addr = data->data[0]+ data->data[1]*256 ;
	
	if(data->data[2] > 130) return PARA_MEANING_ERR ; //每次最多读取130bytes
	else if(addr > 0x7FFF) return EEPROM_RAND_ERR ;
	else if((addr + data->data[2]) > 0x8000)
	{
		*rlen = (0x8000-addr) ;		
	}
	else
		*rlen = data->data[2] ;
	
	if (ReadDataFromEEPROM(addr, *rlen, rpara) == FALSE)
	{
		return EEPROM_ERR;
	}	
	return COMPLETE ;
}
/*******************************************************************************
函 数 名：  Set_FaultFlag_Attr
功能说明：  设置异常标记属性
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_FaultFlag_Attr(UnitPara_t *pData)
{	
	if(pData->len == 0)
		FaultData_Init();	
	else if(pData->len == 5)
	{
		WriteDataToEEPROM(FAULT_START_ADD + sizeof(u32)*(pData->data[0]), 
							sizeof(u32), (u8*)&pData->data[1]);  	//获取标识
	}      //使用EEPROM_W错误标记来存储设置值  yanhuan adding 2015/11/10
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_FaultFlag_Attr
功能说明:  	获取异常标记属性
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_FaultFlag_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	FaultData_Get(rlen, rpara);	
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_UserDataFormat_Attr
功能说明：  设置用户数据恢复出厂设置
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UserDataFormat_Attr(UnitPara_t *pData)
{
	if (pData->len > 1)	//数据长度错误
	{
		return PARA_LEN_ERR;
	}
	if((1 == pData->len)
		&& (0 == pData->data[0]))  //恢复出厂设置 需要恢复433信道
	{
		UserEEPROMData_Init(SecondLevel);
	}
	else if (0 == pData->len)	//数据长度为零  默认处理单元为0
	{
		UserEEPROMData_Init(FristLevel);
	}
	else if (pData->data[0] < UnitCnt_Get()) //参数正确
	{
		UnitPublicEEPROMUnitData_Init(pData->data[0]);
		UnitPrivateEEPROMUnitData_Init(pData->data[0]);
		GroupEEPROMUnit_Init(pData->data[0]);
		SceneEEPROMUnit_Init(pData->data[0]);
		DelLinkForUnit(pData->data[0]);
	}
	else 	
	{
		return PARA_MEANING_ERR;
	}
	
	return COMPLETE;
}

///*******************************************************************************
//函 数 名:  	Get_DeviceInfo_Attr
//功能说明:  	获取设备信息 V1.8 ADD
//参    数:  	data[0]: 目标单元
//			data[1]: 参数长度
//			rLen:	返回参数长度
//			rpara:	返回参数内容
//返 回 值:  	消息执行结果
//*******************************************************************************/
//MsgResult_t Get_DeviceInfo_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
//{
//	*(u16*)&rpara[0] = SOFT->name; 
//	*(u32*)&rpara[2] = SOFT->version;
//	*(u32*)&rpara[6] = HARD->hardVer;
//	*rlen = 10;
////		str->ucpData[10] = g_stEeEx.Sys.Board.DevLinckEn;
////		*(u32*)&str->ucpData[11] = HARDINFO->data;
////		*(u32*)&str->ucpData[15] = HARDINFO->time;	
////		for ( i = 0; i < 100 && HARDINFO->str[i] != '\0'; i++)
////		{
////			str->ucpData[i+19] = HARDINFO->str[i];
////		}
////		str->ucDataLen = i + 19;
//	
//	return COMPLETE;
//}

///*******************************************************************************
//函 数 名：  Set_DeviceAdd_Attr
//功能说明：  设置设备添加属性
//参	  数：  data[0]: 目标单元
//			data[1]: 参数长度
//返 回 值:  	消息执行结果
//*******************************************************************************/
//MsgResult_t Set_DeviceAdd_Attr(UnitPara_t *pData)
//{
//	u8 i;
//	
//	if (!pData->len)//参数长度不合法
//	{
//		return PARA_LEN_ERR;
//	}
//	UserEEPROMData_Init();
//	for (i = 1; i < UnitCnt_Get(); i++)
//	{
//		*(u32*)gUnitData[i].area = *(u32*)&pData->data[0];
//		if (UnitPublicEEPROMData_Set(sizeof(gUnitData[i].area) , gUnitData[i].area) == FALSE)
//		{
//			return EEPROM_ERR;
//		}
//	}
//	gSysData.userID = *(u32*)&pData->data[4];
//	if (SysEEPROMData_Set(sizeof(gSysData.userID) , (u8*)&gSysData.userID) == FALSE)
//	{
//		return EEPROM_ERR;
//	}
//	
//	return COMPLETE;
//}

/*******************************************************************************
函 数 名：  Set_link_Attr
功能说明：  设置联动
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_Link_Attr(UnitPara_t *pData)
{
	MsgResult_t result;
	
	if (!pData->len)//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	switch(pData->data[0])
	{
		case WRITELINK:
			result = (MsgResult_t)WriteLinkItem(pData->data[1], (u8*)&pData->data[2]);
			if (result == COMPLETE)
			{
				pData->len = 1;
				return OWN_RETURN_PARA;
			}
			return result;
		case DELLINK:
			if (DelLinkItem(pData->data[1], pData->data[2]) == 1)
			{
				return COMPLETE;
			}
			return LINKMAP_ERR;
		case MODIFYLINK:
			if((ModifyLinkItem(pData->data[1], pData->data[2], (u8*)&pData->data[3])) == 1)
			{
				return COMPLETE;
			}
			return LINK_MODIFY_ERR;

		default: break;
	}
	return PARA_MEANING_ERR;
}

/*******************************************************************************
函 数 名:  	Get_Link_Attr
功能说明:  	获取联动
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_Link_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	switch(pData->data[0])
	{
		case READLINK:
			if (ReadLinkItem(pData->data[1], pData->data[2], rlen, rpara) == 1)
			{
				return COMPLETE;
			}
			break;
		case QUERYLINK:
			if (QueryLinkItem(pData->data[1], pData->data[2], rlen, rpara))
			{
				return COMPLETE;
			}
			break;
		default: break;
	}

	return LINKMAP_ERR;
}

/*******************************************************************************
函 数 名：  Set_DelLinkForUnit_Attr
功能说明：  按单元删除联动
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_DelLinkForUnit_Attr(UnitPara_t *pData)
{
	if (!pData->len)//参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	if (pData->data[0] == 0)		//Unarty　ＡＤＤ　删除设备所有联动    
	{
		DelAllLinkFromTable(SIMPLESTLINK);
	}
	else if(DelLinkForUnit(pData->data[0]) == 0)
	{
		return LINK_WRITE_ERR;
	}
	
	return COMPLETE;
}

#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)	
/*******************************************************************************
函 数 名:  	MsgResult_t Get_ResetAddr_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
功能说明:  	获取重设地址
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_ResetAddr_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	if(DEVICE_ID_IS_OK)											//如果已设置的新地址合法
    {
        *(u32*)rpara = RE_DEVICE_ID;
    }
    else
    {
        *(u32*)rpara = 0xffffffff;
    }
	Get_UniqueDeviceID((u8*)(rpara+4));	
    *rlen = 16;                          						//4bytes新地址 + 12bytes Chip ID地址 
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  MsgResult_t Set_ResetAddr_Attr(UnitPara_t *pData)
功能说明：  写入重设地址
参	  数：  data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
			0x01(COMPLETE):表示设置成功
			0x02:表示没设置过，设置失败
			0x03:表示已设置过，无需再设置
*******************************************************************************/
MsgResult_t Set_ResetAddr_Attr(UnitPara_t *pData)
{
    u8 Tab_Tem[ENCRYPTION_END_ADDR - RE_DEVICE_ID_START_ADDR]= {0};
    u32 DeviceID_Tem = 0xffffffff;                                                  
    if (pData->len < 8)                                     //参数长度不合法
	{
		return PARA_LEN_ERR;
	}
	if(DEVICE_ID_IS_OK)										//如果已设置的新地址合法	
    {
		if(RE_DEVICE_ID == (*(u32*)(&pData->data[0])))		//若重设地址已设置过，则直接返回成功
		{
			return COMPLETE; 
		}
//		else
//		{
//            return PARA_MEANING_ERR;                        //参数错误          
//		}
    }
    else                                                    //若当前运行STM32原始设备地址,校验当前设备地址与上位机发送的当前校验地址是否相同 
    {
        if(DEVICE_ID != (*(u32*)(&pData->data[4])))
        {
            return PARA_MEANING_ERR;                        //参数错误          
        }
    }
#if (FLASH_ENCRYPTION_EN > 0u)
	*(u32*)(Tab_Tem + (ENCRYPTION_FLAG_ADDR - RE_DEVICE_ID_START_ADDR))	= ENCRYPTION_FLAG;
	*(u32*)(Tab_Tem + (ENCRYPTION_ADDR - RE_DEVICE_ID_START_ADDR))   	= FlashEncryptionFunc((*(u32*)(&pData->data[0])));
#endif
	/*重设地址功能数据格式为->重设地址标记(2bytes 0xaa55) + 重设地址(4bytes) + 重设地址反码(4bytes) */       
	*(u16*)(Tab_Tem + 0x00) = RE_DEVICE_ID_FLAG;
	*(u32*)(Tab_Tem + (RE_DEVICE_ID_ADDR - RE_DEVICE_ID_START_ADDR)) 		= (*(u32*)(&pData->data[0]));
	*(u32*)(Tab_Tem + (RE_DEVICE_ID_ANTI_ADDR - RE_DEVICE_ID_START_ADDR))	=~(*(u32*)(&pData->data[0]));
	if(Program_Write_Ext(RE_DEVICE_ID_START_ADDR,sizeof(Tab_Tem), (u8 *)(Tab_Tem)))
	{
		DeviceID_Tem = (*(u32*)(&pData->data[0]));
		WriteDataToEEPROM(SYSDATA_START_ADD + offsetof(SysData_t, deviceID), sizeof(u32), (u8*)&DeviceID_Tem);
		Thread_Login(ONCEDELAY,0,1000,HardID_Change);
		return COMPLETE;                                    //表示没设置过，设置成功
	}
	else
	{
		return ((MsgResult_t)(0x02));                       //表示没设置过，设置失败
	}
}
#endif


/**************************Copyright BestFu 2014-05-14*************************/
