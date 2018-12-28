/***************************Copyright BestFu 2014-05-14*************************
文	件：	Linksrc.h
说	明：	源的执行入口和整个联动的初始化工作源文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	暂无
*******************************************************************************/
#ifndef    _linksrc_h_
#define    _linksrc_h_

#include "Linkall.h"


//条件号condition
typedef enum
{
	CLARGE           = 0x01,            //大于
	CLARGEOREQUAL    = 0x02,            //大于或者等于
	CSMALL           = 0x03,            //小于
	CSMALLOREQUAL    = 0x04,            //小于或者等于
	CEQUAL           = 0x05,            //等于
	CNOTEQUAL        = 0x06,            //不等于
	CINRANGE         = 0x07,            //范围内
	COUTRANGE        = 0x08             //范围外
}CONDITION;

#define LINKSTD            0x00     //正常联动
#define LINKREV            0x01     //反向联动，只用于复杂联动中的源端

//源类型
#define    TBIT              0x01            //位型，只有0或1
#define    TINT8S            0x02            //带符号字符型，范围-128~127
#define    Tu8            0x03            //无符号字符型，范围0~255
#define    TINT16S           0x04            //带符号短整型，范围-32768~32767
#define    TINT16U           0x05            //无符号短整型，范围0~65535
#define    TINT32S           0x06            //带符号短整型，范围-21亿~21亿
#define    TINT32U           0x07            //无符号短整型，范围0~42亿
#define    TFP32             0x08            //单精度型浮点数据
#define    TFP64             0x09            //双精度型浮点数据

#define    MAX_SRC_OLD_CNT     10            //需要保存上一次采样值的个数

//typedef struct
//{
//	u8  UnitID;
//	u8  PropID;
//	int    Value;
//}SrcOldValItem;

//typedef struct
//{
//	SrcOldValItem  src[MAX_SRC_OLD_CNT];
//	u8  Cnt;
//}SrcOldValTable;

//extern SrcOldValTable SrcOld;

u8 ExeLinkPackage(u8 *data, u8 type, u8 RevFlag);
u8 LinkSrcExe(u8 sevent, u8 action, u8 type, int num);

void LinkInit(void);
void LinkClr(void);

u8 FindOldValueTableFull(u8 unitID, u8 propID);
u8 FindOldValueTableItem(u8 unitID, u8 propID);
u8 AddOldValueTableItem(u8 unitID, u8 propID);

u8 CmpSampAndLinkValue(u8 Cndt, int Lvtmp1, int Lvtmp2, int Svtmp);
u8 GetConditionReverse(u8 SrcCndt);
u8 GetConditionOffset(u8 SrcCndt, int *sv1, int *sv2);

#endif
/**************************Copyright BestFu 2014-05-14*************************/
