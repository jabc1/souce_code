/***************************Copyright BestFu 2014-05-14*************************
文	件：	UnitCgf.h
说	明：	单元模块配置头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.14 
修　改：	暂无
*******************************************************************************/
#ifndef __UNITCFG_H
#define __UNITCFG_H

#include "BF_type.h"
#include "UserData.h"

#define    SRCEVENT          1               // 源为事件型
#define    SRCVALUE          2               // 源为数值型

typedef enum
{
	WRITEWITHACK 	= 0x02,		//需应答写操作消息类型码
    ACKWRITE		= 0x03,     //写操作应答消息类型码
    WRITENACK       = 0x04,    	//无需应答写操作消息类型码

    READWITHACK     = 0x05,    	//读操作消息类型码
    READACK         = 0x06,    	//读操作应答消息类型码

    WRITESUC        = 0x07,    	// 写操作成功时才回应消息类型码
    WRITEACKSUC     = 0x08,    	// 写操作成功时应答消息类型码
    WRITEFAIL       = 0x09,    	// 写操作失败时才回应消息类型码
    WRITEACKFAIL    = 0x0A,   	// 写操作失败时应答消息类型码

    READSUC         = 0x0B,    	// 读操作成功时才回应消息类型码
    READACKSUC      = 0x0C,    	// 读操作成功时应答消息类型码
    READFAIL        = 0x0D,    	// 读操作失败时才回应消息类型码
    READACKFAIL     = 0x0E,    	// 读操作失败时应答消息类型码
}MsgRcve_t;

typedef enum
{
	SINGLE_ACT 			         = 0X01,		//单节点操作
	GROUP_ACT 			         = 0X02,		//组广播
	SCENE_ACT			         = 0X03,		//情景广播
	DEVIVCE_TYPE_ACT 	         = 0x04,		//设备类型广播 
	DEV_ALL_ACT      	         = 0x05,    	//所有广播
	UNIT_USER_TYPE_ACT	         = 0x06,		//单元应用类型广播
	UNIT_COMMON_TYPE_ACT         = 0x07,		//单元应用大类广播
	SPEC_AREA_DEV_TYPE_ACT  	 = 0x14,		//特定区域设备类型广播				/**>  Jay Add 2015.11.17*/
	SPEC_AREA_UNIT_USER_TYPE_ACT = 0x16			//特定区域单元应用类型广播			/**>  Jay Add 2015.11.17*/
}ObjectType_t;

typedef enum
{
	COMPLETE = 1, 	//操作正常完成
	
	OWN_RETURN_PARA,	//拥有返回参数	
	NO_ACK,				//不做应答
	USER_ID_ERR,	//用户号错误
	ADDS_TYPE_ERR,	//目标地址类型错误	
	ID_ERR,			//目标地址错误
	
	AREA_ERR,		//区域错误
	MSG_REPEAT,		//消息重复
	UNIT_DISABLE,	//单元禁能
	NO_WRITE,		//属性没有写权限
	NO_READ,		//属性没读权限
	
	MSG_TYPE_ERR,	//消息类型错误
	NO_CMD,			//单元属性不存在
	PARA_MEANING_ERR,	//参数意义错误
	PARA_LEN_ERR,		//参数长度错误
	EEPROM_ERR,			//EEPROM操作错误
	
	EEPROM_RAND_ERR,	//EEPROM操作范围越界
	CMD_EXE_ERR,		//指令执行错误
	
	LINKFULL,           //联动满，无法再写
	LINKNUM_ERR,		//联动号获取失败
	LINKMAP_ERR,         //联动操作类型码和联动操作类型匹配错误
	LINK_MODIFY_ERR,    //联动修改错误
	LINK_WRITE_ERR,     //联动写入失败
	
	CMD_EXE_OVERTIME,	//指令执行超时
	
	DATA_SAVE_OVER,		//数据存储满
	DATA_NO_FIND,		//数据没有找到
	
	VOICE_POWER_BAT,	//语音处于电池供电状态
}MsgResult_t;		//消息操作结果枚举量

typedef enum
{
	LEVEL_0,		//等级最低
	LEVEL_1,		//
}Level_t;
#pragma pack(1)
typedef struct
{
	const 	u8 unit;
	const 	u8 cmd;
			u8 len;
	const 	u8 data[200];
}UnitPara_t;
#pragma pack()

typedef struct
{
	u32 cmd;                                   			//指令号
	Level_t level;											//属性重要级别
	MsgResult_t (*pSet_Attr)(UnitPara_t *pData);            //set函数入口
	MsgResult_t (*pGet_Attr)(UnitPara_t *pData, \
						u8 *rlen, u8 *rpara);  		//get函数入口
}AttrExe_st;

typedef struct
{
	void (*Init)(u8 unitID);        // 单元初始化，包括自身全局参数和驱动初始化
	void (*DefEE)(u8 unitID);       // EEPROM参数设定默认值
	void (*Instruct)(void);	//单元验证函数
	const AttrExe_st *attrExe;			//属性列表指针
}Unit_st;

extern const Unit_st UnitTab[];
extern UnitData_t gUnitData[];

void Unit_Init(void);
u8 UnitCnt_Get(void);

/*用户EEPROM数据读写操作函数*/
extern MsgResult_t EEPROM_Write(u32 addr, u32 len, u8 *data);
extern MsgResult_t EEPROM_Read(u32 addr, u32 len, u8 *data);
extern void Upload(void);
extern void PropEventFifo(unsigned char unitID, unsigned char propID, 
					unsigned char type, int value);
#endif
/**************************Copyright BestFu 2014-05-14*************************/
