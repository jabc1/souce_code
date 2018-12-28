/***************************Copyright BestFu 2014-05-14*************************
文	件：	LinkWRDC.h
说	明：	分项的写入、读取、删除和查询工作源文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	暂无
*******************************************************************************/
#ifndef _LinkWRDC_
#define _LinkWRDC_

#include "linkall.h"

#define    SIMPLESTLINK       1  //最简单联动
#define    SIMPLELINK         2  //简单联动
#define    COMPLEXLINK        3  //复杂联动

#define    READLINK           1  // 读取联动
#define    WRITELINK          2  // 写入联动
#define    DELLINK            3  // 删除联动
#define    QUERYLINK          4  // 查询联动
#define    MODIFYLINK         5  // 修改联动

u8 CheckLinkFull(u8 type);
u8 QueryLink(u8 type, u8 num);
u8 QueryAllLinkFromTable(u8 type, u8 *len, u8 *data);
u8 ReadLinkFromTable(u8 type, u8 num, u8 *len, u8 *data);
u8 WriteLinkToTable(u8 type, u8 num, u8 linknum, u8 addflag, u8 *data);
u8 DelAllLinkFromTable(u8 type);
u8 DelLinkFromTable(u8 type, u8 num);
u8 ChkLinkIsExist(u8 type, u8 *linknum, u8 *data);
u8 GetLinkNum(u8 type, u8 *linknum);
u8 WriteLinkItem(u8 type, u8 *data);
u8 ReadLinkItem(u8 type, u8 num, u8 *len, u8 *data);
u8 QueryLinkItem(u8 type, u8 num, u8 *len, u8 *data);
u8 DelLinkItem(u8 type, u8 num);

u8 ModifyLinkItem(u8 type, u8 linknum, u8 *data);
#endif
/**************************Copyright BestFu 2014-05-14*************************/
