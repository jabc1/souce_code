/***************************Copyright BestFu 2014-05-14*************************
文	件：	UnitCfg.c
说	明：	单元模块配置源代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.14 
修　改：	暂无
*******************************************************************************/
#include "UnitCfg.h"
#include "UserData.h"


#define UNIT_CNT	(sizeof(UnitTab)/sizeof(Unit_st))		//总单元数

/*添加: 包含单元属性头文件*/
#include "SysExeAttr.h"

#include "AirAttr.h"



/*添加单元接口函数，与属性接口列表*/
/*格式：｛上电初始化函数接口，EEPROM数据初始化函数接口，单元验证函数接口，单元属性列表名｝*/
/*以上接口函数，当单元不需要此功能时直接用NULL表示。单元属性列表名必须为一个确定*/
const Unit_st UnitTab[] = 
{
	{SysAttr_Init	, NULL				, NULL			, SysAttrTable	},	//系统共用属性，添加者请误修改

	{Air_ParaInit	, AirEepromInit		, Air_Instruct	, AirAttrTab},
};



UnitData_t gUnitData[UNIT_CNT];

/*******************************************************************************
函 数 名：	UnitCnt_Get
功能说明： 	获取当前单元总数
参	  数： 	无
返 回 值：	当前单元数
*******************************************************************************/
u8 UnitCnt_Get(void)
{
	return ((sizeof(UnitTab)/sizeof(Unit_st)));
}

/*******************************************************************************
函 数 名：	Unit_Init
功能说明： 	初始化所有单元
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Unit_Init(void)
{
	u8 i = UNIT_CNT;
	UnitPublicEEPROMData_Get(UNIT_CNT, gUnitData);	//获取用户设置的单元属性值
	
	while (i)	
	{
		if (UnitTab[--i].Init != NULL)
		{
			UnitTab[i].Init(i);		//初始化各单元数据
		}
	}
}

/**************************Copyright BestFu 2014-05-14*************************/
