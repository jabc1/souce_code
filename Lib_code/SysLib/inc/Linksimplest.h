/***************************Copyright BestFu 2014-05-14*************************
文	件：	Linksimplest.h
说	明：	最简联动源文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	暂无
*******************************************************************************/
#ifndef _LinkSimplest_
#define _LinkSimplest_

#include "linkall.h"

#define    ALINK_SRC_VALUE_EN  0   // 暂时为四合一独有的值联动
#define    LINKERROR          255
#define    CLINKEXED          1

#if(ALINK_EN > 0)
typedef struct //简单联动结构,32Bytes
{
	u8   UseFlag;      //使用标志
	u8   LinkNum;      //联动号
	
	u8   SUnitID;      //源单元ID号
	u8   SPropID;      //源属性ID号
	u8   SCondition;   //源条件
	u8   SValue[4];    //源值

	u8   DDevID[4];    //目标设备ID
	u8   DType;        //目标类型
	u8   DAddrT[2];    //目标类型地址码，单节点时不用
	u8   DMsgType;     //消息类型码，区分是否需要回应
	u8   UnitID;       //目标单元ID
	u8   DestCmd;      //目标命令
	u8   DestLen;      //目标长度
}ALinkHead;

typedef struct //简单联动结构,32Bytes
{
	u8   UseFlag;      //使用标志
	u8   LinkNum;      //联动号
	
	u8   SUnitID;      //源单元ID号
	u8   SPropID;      //源属性ID号
	u8   SCondition;   //源条件
	u8   SValue[4];    //源值
	
	u8   DDevID[4];    //目标设备ID
	u8   DType;        //目标类型
	u8   DAddrT[2];    //目标类型地址码，单节点时不用
	u8   DMsgType;     //消息类型码，区分是否需要回应
	u8   UnitID;       //目标单元ID
	u8   DestCmd;      //目标命令
	u8   DestLen;      //目标长度
	u8   data[13];     //目标参数
}ALinkItem;

typedef struct
{
	ALinkItem   linkitem[MAX_ALINKITEM];  //联动项
	u8       Cnt;           //计数
}ALinkTable;

#if (ALINK_SRC_VALUE_EN > 0)
typedef struct
{
	u8  LinkNum;
	u8  Value;
	u8  FirstFlag ; //为了判断是否为第一次遍历
	int OldValue;
}ALinkVItem;

typedef struct
{
	ALinkVItem LinkV[MAX_ALINKITEM];
	u8 cnt;
}ALinkVTable;  //保存数值型源，满足条件后联动是否执行过

extern ALinkVTable  ALinkV;
#endif


extern ALinkTable   ALink; 
void ALinkInit(void);
void ALinkClr(void);
u8 CheckALinkFull(void);
u8 ChkALinkIsExist(u8 *linknum, u8 *data);
u8 GetALinkNum(u8 *linknum);
u8 QueryALink(u8 num);
u8 ReadALink(u8 num , u8 *len, u8 *data);
u8 WriteALink(u8 num, u8 linknum, u8 addflag, u8 *data);
u8 DelALink(u8 num);
u8 DelLinkForUnit(u8 unitnum);
u8 CheckALinkValueAndExe(u8 *data, u8 linknum, int newsamp);

void ALinkVInit(void);
u8 FindALinkValueNum(u8 linknum);
u8 GetALinkValue(u8 linknum);
u8 AddALinkValue(u8 linknum);
u8 DelALinkValue(u8 linknum);

__inline u32 GetALinkInited(void)
{
	return ALINK_START_ADDR;
}

__inline u32 GetALinkStartAddr(void)
{
	return ALINK_START_ADDR + 1;
}

#endif
#endif
/**************************Copyright BestFu 2014-05-14*************************/
