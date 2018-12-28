/***************************Copyright BestFu 2014-05-14*************************
文	件：	Linkcommon.h
说	明：	联动包含头
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	周旺
日	期：	2014.06.30 
修　改：	暂无
*******************************************************************************/
#ifndef LINKCOMMON_H
#define LINKCOMMON_H
#include "BF_type.h"

//联动配置
#define    ALINK_EN           1  //生成最简联动模块，1表示有最简联动，0表示无
#define    MAX_ALINKITEM      50 //支持最简联动数，每条32字节，按硬件资源配
#define    SLINK_SRC_VALUE_EN 0  //源是否包括数值型源，1表示有数值型源，0表示无
#define    SLINK_SRC_EVENT_EN 1  //源是否包括事件型源，1表示有事件型源，0表示无
#define    OFFSET             1  //偏移量，用于计算数值时的条件值范围，按需要配置

#define    SIMPLESTLINK       1  //最简单联动
#define    SIMPLELINK         2  //简单联动
#define    COMPLEXLINK        3  //复杂联动

#define    READLINK           1  // 读取联动
#define    WRITELINK          2  // 写入联动
#define    DELLINK            3  // 删除联动
#define    QUERYLINK          4  // 查询联动
#define    MODIFYLINK         5  // 修改联动

typedef struct
{//20BYTES
	u8	DTLast[4];	  //动作持续时间
	u8	DTDelay[4];   //动作执行延时时间
	u8	TimeTable;	  //执行时间表
	u8   UnitID;       //目标单元ID
	u8	DestCmd;	  //目标命令
	u8	DestLen;	  //目标长度
	u8	data[10];	  //目标参数
}DestTarget;

#define LINKSTD            0x00     //正常联动
#define LINKREV            0x01     //反向联动，只用于复杂联动中的源端
#define EXEERROR           0xFF     //操作失败
#define EEPROMDATAINITED   0xA5     //已初始化

#define WCHAR(H,L)            (((H)<<8)+(L))
#define WBYTE(H,L)            (((H)<<4)+(L))
#define WSINT(H,HM,HL,L)      ((H<<12)+(HM<<8)+(HL<<4)+(L<<0))
#define LOWORD(d)             (d)
#define HIWORD(d)             (d>>8)
#define DWCHAR(H, HM, ML, L)  (u32)((H<<24)+(HM<<16)+(ML<<8)+(L<<0))

#endif
/**************************Copyright BestFu 2014-05-14*************************/
