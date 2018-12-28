/***************************Copyright BestFu 2014-05-14*************************
文	件：	UserData.h
说	明：	用户数据管理文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.24 
修　改：	暂无
*******************************************************************************/
#ifndef __USERDATA_H
#define __USERDATA_H

#include "Bf_type.h"

#define SYS_VERSION			(10128)					//系统库版本, UnartyChen Check

#define DEFAULT_USERID		(0xFFFFFFFF)			//默认用户ID号
#define SUPER_USERID		(0x54534542)			//超级用户ID号("BEST")
#define DEFAULT_AREA		(0xFFFFFFFF)			//默认区域号
#define UNIT_MAX_NUM		(UNITDATA_SPACE/ONCEUNITDATA_SPACE)	//容许最大单元数
#define AREA_RANK_NUM		(4)									//区域分级数

#define USERDATA_START_ADD		(0x0000)								//用户数据存储起始地址
#define USERDATA_END_ADD		(0x3000)								//用户数据存储结束地址(8K)

/*系统异常次数EEPROM记录空间*/
#define FAULT_START_ADD				(USERDATA_START_ADD)					//异常存储首地址
#define FAULT_SPACE					(0x40)									//异常数据占用空间大小 

/*系统数据EEPROM空间分配*/
#define SYSDATA_START_ADD			(FAULT_START_ADD + FAULT_SPACE)					//系统数据存储首地址
#define SYSDATA_SPACE				(0x40)									//系统数据占用空间大小 
#define SYSDATA_RESEVER_SPACE		(SYSDATA_SPACE1 ?sizeof(Mode1_t))		//系统数据剩余空间

/*单元数据EEPROM空间分配*/
#define UNITDATA_START_ADD			(SYSDATA_START_ADD + SYSDATA_SPACE)	//单元数据存储首地址 
#define UNITDATA_SPACE				(0x180)									//用户数据占用空间大小 
#define ONCEUNITDATA_SPACE			(0x10)									//单个用户单元数据空间
#define UNITDATA_RESEVER_SPACE		(UNITDATA_SPACE - sizeof(UnitData_t))	//用户数据剩余空间

/*单元组数据EEPROM空间分配*/
#define GROUP_START_ADD			(0x200)										//设备组组数据存储地址
#define UNIT_GROUP_NUM			(80)										//每个单元组的总数
#define AREA_GROUP_NUM			(16)										//每个区域组的总数
#define GROUP_SPACE				(1.0/8)										//组占用空间
#define UNIT_GROUP_SPACE		(UNIT_GROUP_NUM*GROUP_SPACE)				//每个单元组占用的存储空间
#define AREA_GROUP_SPACE		(AREA_GROUP_NUM*GROUP_SPACE)				//每个区域组占用的存储空间

/*单元场景数据EEPROM空间分配*/
#define SCENE_START_ADD				(0x400)									//场景起始地址
#define SCENE_SPACE					(0x1800)								//场景占用总空间 （6K， 最多只能配置128个场景，每级区域下只能配置16个场景）
#define SCENE_END_ADD				(SCENE_START_ADD+SCENE_SPACE)			//场景EEPROM结束地址

#define SCENEFLAG_START_ADD			(SCENE_START_ADD)						//设备场景标识数据存储地址							
#define UNIT_SCENEFLAG_NUM			(80)									//每个单元场景标识总数
#define AREA_SCENEFLAG_NUM			(16)									//每个区域场景标识总数
#define SCENEFLAG_SPACE				(1)										//场景标识占用空间
#define UNIT_SCENEFLAG_SPACE		(UNIT_SCENEFLAG_NUM*SCENEFLAG_SPACE)	//每个单元场景标识占用的存储空间
#define AREA_SCENEFLAG_SPACE		(AREA_SCENEFLAG_NUM*SCENEFLAG_SPACE)	//每个区域场景标识占用的存储空间
#define SCENEFLAG_DEFAULT_VALUE		(0x00)									//场景标识默认值

#define SCENESAVEFLAG_START_ADD		(SCENESAVE_START_ADD - 0x10)			//设备场景数据存储标识
#define SCENE_MAX_NUM				(256)									//最大场景数

#define SCENESAVE_START_ADD			(0xC00)									//场景存储起始地址
#define SCENESAVE_DATA_SPACE		(32)									//场景标识占用空间

#define SCENE_DATA_SPACE1			(8)										//场景数据空间1大小
#define SCENE_DATA_SPACE2			(16)									//场景数据空间2大小
#define SCENE_DATA_SPACE3			(32)									//场景数据空间3大小

#define SCENE_SPACE1_NUM			(128)									//场景空间1的总数
#define SCENE_SPACE2_NUM			(64)									//场景空间2的总数
#define SCENE_SPACE3_NUM			(64)									//场景空间3的总数

#define ALINK_START_ADDR            (0x2000)								//最简联动首地址
#define ALINK_SPACE                 (0x0800)                                //最简联动空间

#pragma pack(1)             //按1字节对齐
typedef struct		//系统共用数据区域
{
	u32 userID;		//用户号
	u32 deviceID;	//设备ID
	u8 	sendCnt;	//数据发送次数
	u8  serialNum;  //流水号
	u32 BandAddr[5];//转发绑定地址
	u8  BandAreaEn;//转发绑定区域广播指令使能标志
}SysData_t;			//系统共用数据区

typedef struct 		//单元数据区域
{
	u8 	area[4];		//区域号
	u16 type;			//所属类型
	u8 	able;			//单元使能标志位
	u8  common;			//单元共用大类
}UnitData_t;

#pragma pack()
typedef enum 
{
	FristLevel = 1,
	SecondLevel = 2,
}DataInitLevel_t;

extern SysData_t gSysData;

void UserData_Init(void);
void UserEEPROMData_Init(DataInitLevel_t level);

/*异常统计EEPROM数据操作*/
void FaultData_Init(void);
void FaultData_Save(u8 i);
void FaultData_Get(u8 *len, u8 *data);

/*系统EEPROM数据操作*/
void SysEEPROMData_Init(void);
u8 SysEEPROMData_Set(u8 size , u8 *data); 

/*单元共用EEPROM数据操作*/
void UnitPublicEEPROMData_Init(void);
void UnitPublicEEPROMUnitData_Init(u8 unit);
void UnitPublicEEPROMData_Get(u8 unitCnt, UnitData_t *unitData);
u8 UnitPublicEEPROMData_Set(u8 size , u8 *data);

/*单元共用EEPROM数据操作*/
void UnitPrivateEEPROMData_Init(void);
void UnitPrivateEEPROMUnitData_Init(u8 unit);

/*组EEPROM数据操作*/
void GroupEEPROM_Init(void);
void GroupEEPROMUnit_Init(u8 unit);
void GroupEEPROM_Clr(u8 unit, u8 area, u8 num);
void GroupEEPROM_Set(u8 unit, u8 area, u8 num);
u8 GroupEEPROM_Get(u8 unit, u8 area, u8 num);

/*场景EEPROM数据操作*/
void SceneEEPROM_Init(void);
void SceneEEPROMUnit_Init(u8 unit);
void SceneEEPROM_Clr(u8 unit, u8 area, u8 num);
void SceneEEPROM_Get(u8 unit, u8 area, u8 num, u8 *data);
u32 SceneEEPROM_Set(u8 unit, u8 area, u8 num, u8 *data);

#endif // UserData.h end 
/**************************Copyright BestFu 2014-05-14*************************/
