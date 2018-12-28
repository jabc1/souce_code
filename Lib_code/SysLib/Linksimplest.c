/***************************Copyright BestFu 2014-05-14*************************
文	件：	Linksimplest.c
说	明：	最简联动源文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	Unarty 2014.11.11 ALinkInit中将EEPROM第一次使用判断去除
*******************************************************************************/

#include "Linkall.h"

#if (ALINK_SRC_VALUE_EN > 0)
ALinkVTable  ALinkV;    //简单联动执行值，用于判断相同条件是否执行过?
//SrcTypeTable SrcType;   //简单联动源上一次的采样值
#endif

#if (ALINK_EN > 0)
ALinkTable   ALink;   //最简联动的RAM映射变量

/*******************************************************************************
函 数 名：	ALinkInit
功能说明： 	最简联动表的初始化
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void ALinkInit(void)
{
	if (FALSE == ReadDataFromEEPROM(GetALinkStartAddr(), sizeof(ALinkTable), 
							(u8 *)&ALink))
	{
		return;
	}
	if(ALink.Cnt > MAX_ALINKITEM)
	{
		ALinkClr();
	}
#if (ALINK_SRC_VALUE_EN > 0)
	ALinkVInit();
#endif
}

/*******************************************************************************
函 数 名：	ALinkClr
功能说明： 	清除最简联动表
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void ALinkClr(void)
{
	u8 data;
	data = EEPROMINITED;
	WriteDataToEEPROM(GetALinkInited(), 1, &data);
	if(ALink.Cnt != 0)
	{
		memset((u8 *)&ALink, 0, sizeof(ALinkTable));
		WriteDataToEEPROM(GetALinkStartAddr(), sizeof(ALinkTable), (u8 *)&ALink);
	}
}

/*******************************************************************************
函 数 名：	ChkALinkIsExist
功能说明： 	匹配联动数据在联动表中是否存在，如果存在把联动号存于linknum指针
参	  数： 	 *linknum-从联动表中找到的联动号存放于此
             *data-联动数据
返 回 值：	1表示成功，0表示失败
*******************************************************************************/
u8 ChkALinkIsExist(u8 *linknum, u8 *data)
{
	u8 i,cnt;
	
	for(i=0;i<ALink.Cnt;i++)
	{
		if(ALink.linkitem[i].UseFlag)
		{// 匹配是否配有相同目标?前2字节和后面13个参数不需要匹配(参数区也匹配，识别一模一样的联动)
			cnt = sizeof(ALinkHead)- 2 + ALink.linkitem[i].DestLen;
			if(!memcmp(data, &ALink.linkitem[i].SUnitID, cnt))
			{
				*linknum = ALink.linkitem[i].LinkNum;
				return TRUE;
			}
		}
	}
	return FALSE;
}

/*******************************************************************************
函 数 名：	GetALinkNum
功能说明： 	获取联动号，从小到大顺序，找最小的值为最新联动号
参	  数： 	*linknum-找到的最小联动号存放于此
返 回 值：	1表示成功，0表示失败
*******************************************************************************/
u8 GetALinkNum(u8 *linknum)
{
	u8 i,j;
	for(i=1;i<254;i++)//联动号范围1-254
	{
		for(j=0;j<ALink.Cnt;j++)
		{
			if(ALink.linkitem[j].UseFlag)
			{
				if(i == ALink.linkitem[j].LinkNum)
				{
					break;
				}
			}
		}
		if(j == ALink.Cnt)
		{
			*linknum = i;
			return TRUE;
		}
	}
	return FALSE;
}

/*******************************************************************************
函 数 名：	QueryALink
功能说明： 	查询data联动号在最简联动表中的位置
参	  数： 	num-要查询的联动号
返 回 值：	EXEERROR表示没找到，0-254表示找到的实际位置
*******************************************************************************/
u8 QueryALink(u8 num)
{
	u8 i;
	for(i=0;i<ALink.Cnt;i++)
	{
		if(ALink.linkitem[i].UseFlag)
		{
			if(num == ALink.linkitem[i].LinkNum)
			{
				return i;
			}
		}
	}
	return EXEERROR;
}

/*******************************************************************************
函 数 名：	CheckALinkFull
功能说明： 	检查最简联动表是否满
参	  数： 	void
返 回 值：	1表示已满，0表示不满
*******************************************************************************/
u8 CheckALinkFull(void)
{
	if(ALink.Cnt >= MAX_ALINKITEM)
		return TRUE;
	else
		return FALSE;
}

/*******************************************************************************
函 数 名：	ReadALink
功能说明： 	从最简联动表中读取联动
参	  数： 	num-要查询的联动位置，读取到的联动数据存放于link指针
            *len-读取到的数据长度
            *data-读取到的数据
返 回 值：	1表示读取成功，0表示读取失败
*******************************************************************************/
u8 ReadALink(u8 num , u8 *len, u8 *data)
{
	u8 cnt;
	cnt = sizeof(ALinkHead) - 2 + ALink.linkitem[num].DestLen;
	memcpy(data, (u8 *)&ALink.linkitem[num].SUnitID, cnt);
	*len = cnt;
	return TRUE;
}

/*******************************************************************************
函 数 名：	WriteALink
功能说明： 	把联动写入最简联动表中
参	  数： 	num-要写入的联动位置，把data指针数据写入num位置
返 回 值：	1表示写入成功，0表示写入失败
*******************************************************************************/
u8 WriteALink(u8 num, u8 linknum, u8 addflag, u8 *data)
{
	ALinkItem *pLink;
	u8 cnt;
	pLink = (ALinkItem *)(data-2);
	if(num <= MAX_ALINKITEM)		//2014.10.08 Unarty Change 解决联动最大数只能是49条不是50条的问题
	{
		cnt = sizeof(ALinkHead)-2+pLink->DestLen;
		memcpy(&ALink.linkitem[num].SUnitID, data, cnt);
		ALink.linkitem[num].UseFlag = 1;
		ALink.linkitem[num].LinkNum = linknum;
		WriteDataToEEPROM(GetALinkStartAddr() + sizeof(ALinkItem)*num, 
				sizeof(ALinkItem), (u8 *)&ALink.linkitem[num]);

		if(addflag)
		{
			ALink.Cnt++;
			WriteDataToEEPROM(GetALinkStartAddr() + ((u32)&ALink.Cnt - (u32)&ALink), 
							sizeof(u8), &ALink.Cnt);
		}
#if (ALINK_SRC_VALUE_EN > 0)
		AddALinkValue(ALink.linkitem[num].LinkNum);
#endif
		
		return TRUE;
	}
	return FALSE;
}

/*******************************************************************************
函 数 名：	DelALink
功能说明： 	把联动从最简联动表中删除
参	  数： 	num-要删除的联动位置
返 回 值：	1表示删除成功，0表示删除失败
*******************************************************************************/
u8 DelALink(u8 num)
{
	if(num != (ALink.Cnt-1))
	{
		memcpy((u8 *)&ALink.linkitem[num],
			(u8 *)&ALink.linkitem[ALink.Cnt-1],sizeof(ALinkItem));
		WriteDataToEEPROM(GetALinkStartAddr() + sizeof(ALinkItem)*num, 
				sizeof(ALinkItem), (u8 *)&ALink.linkitem[num]);
		memset((u8 *)&ALink.linkitem[ALink.Cnt-1], 0, sizeof(ALinkItem));
		WriteDataToEEPROM(GetALinkStartAddr() + sizeof(ALinkItem)*(ALink.Cnt-1), 
				sizeof(ALinkItem), (u8 *)&ALink.linkitem[num]);
	}
	else
	{
		memset((u8 *)&ALink.linkitem[num], 0, sizeof(ALinkItem));
		WriteDataToEEPROM(GetALinkStartAddr() + sizeof(ALinkItem)*num, 
				sizeof(ALinkItem), (u8 *)&ALink.linkitem[num]);
	}
	
#if (ALINK_SRC_VALUE_EN > 0)
	DelALinkValue(ALinkV.LinkV[num].LinkNum);
#endif
	
	if (ALink.Cnt > 0)
	{
		ALink.Cnt--;
		WriteDataToEEPROM(GetALinkStartAddr() + ((u32)&ALink.Cnt - (u32)&ALink), 
				sizeof(u8), (u8 *)&ALink.Cnt);
		return TRUE;
	}
	
	return FALSE;
}

#if (ALINK_SRC_VALUE_EN > 0)
/*****************************************************************************
  函数名:  void ALinkVInit(void)
功能说明:  联动执行值的初始化，可判断相同条件下是否执行过?
    参数:  void
  返回值:  void
*****************************************************************************/
void ALinkVInit(void)
{
   u8 i;
   for(i=0;i<ALink.Cnt;i++)
   {
	   ALinkV.LinkV[i].LinkNum = ALink.linkitem[i].LinkNum;
	   ALinkV.LinkV[i].Value   = 0;
	   ALinkV.LinkV[i].FirstFlag = 0 ; //初次遍历后该值为1 2015/09/10 yanhuan adding
	   ALinkV.LinkV[i].OldValue = 0;
   }
   ALinkV.cnt = ALink.Cnt;
}

/*****************************************************************************
  函数名:  u8 GetALinkValue(u8 linknum)
功能说明:  获取联动号的触发值
    参数:  linknum(linkage number)缩写
           表示每条联动的号码，该数值唯一
  返回值:  返回联动触发值0,1,2目前这三种情况。当没找到该联动时返回错误0xFF
*****************************************************************************/
u8 GetALinkValue(u8 linknum)
{
	u8 i;
	for(i=0;i<ALinkV.cnt;i++)
	{
		if(linknum == ALinkV.LinkV[i].LinkNum)
		{
			return ALinkV.LinkV[i].Value;
		}
	}
	return LINKERROR;
}

/*****************************************************************************
  函数名:  u8 SetALinkValue(u8 linknum, u8 value)
功能说明:  设置联动触发值
    参数:  linknum(linkage number)缩写
           表示每条联动的号码，该数值唯一
           value,表示联动触发值
  返回值:  1表示成功，0表示失败
*****************************************************************************/
u8 SetALinkValue(u8 linknum, u8 value)
{
	u8 i;
	for(i=0;i<ALinkV.cnt;i++)
	{
		if(linknum == ALinkV.LinkV[i].LinkNum)
		{
			ALinkV.LinkV[i].Value = value;
			return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************
  函数名:  u8 AddALinkValue(u8 linknum)
功能说明:  增加联动触发值数组队列中的一项
    参数:  linknum(linkage number)缩写
           表示每条联动的号码，该数值唯一
  返回值:  1表示成功，0表示失败
*****************************************************************************/
u8 AddALinkValue(u8 linknum)
{
	u8 num;
	num = FindALinkValueNum(linknum);
	
	if(num == EXEERROR)
	{
		if(ALinkV.cnt >= MAX_ALINKITEM)
		{
			return FALSE;
		}
		ALinkV.LinkV[ALinkV.cnt].LinkNum = linknum;
		ALinkV.LinkV[ALinkV.cnt].Value   = 0;
		ALinkV.LinkV[ALinkV.cnt].OldValue= 0;
		ALinkV.LinkV[ALinkV.cnt].FirstFlag = 0 ; //增加值联动后，第一次只要满足条件即可触发 2015/09/10 yanhuan adding
		ALinkV.cnt++;
	}
	else
	{
		ALinkV.LinkV[num].Value   = 0;
	}

	return TRUE;
}

/*****************************************************************************
  函数名:  u8 FindALinkValueNum(u8 linknum)
功能说明:  查找联动触发值在数组队列中的位置
    参数:  linknum(linkage number)缩写
           表示每条联动的号码，该数值唯一
  返回值:  0-254表示具体位置，255表示没找到
*****************************************************************************/
u8 FindALinkValueNum(u8 linknum)
{
	u8 i;
	for(i=0;i<ALinkV.cnt;i++)
	{
		if(linknum == ALinkV.LinkV[i].LinkNum)
		{
			return i;
		}
	}
	return EXEERROR;
}

/*****************************************************************************
  函数名:  u8 DelALinkValue(u8 linknum)
功能说明:  删除联动触发值数组队列中的一项
    参数:  linknum(linkage number)缩写
           表示每条联动的号码，该数值唯一
  返回值:  1表示成功，0表示失败
*****************************************************************************/
u8 DelALinkValue(u8 linknum)
{
	u8 num;
	if((num = FindALinkValueNum(linknum)) == EXEERROR)
	{
		return FALSE;
	}
	if(num == ALinkV.cnt-1)
	{
		memset((u8 *)&ALinkV.LinkV[num], 0, sizeof(ALinkVItem));
	}
	else
	{
		memcpy((u8 *)&ALinkV.LinkV[num], 
			(u8 *)&ALinkV.LinkV[ALinkV.cnt-1], sizeof(ALinkVItem));
		memset((u8 *)&ALinkV.LinkV[ALinkV.cnt-1], 0, sizeof(ALinkVItem));
	}
	if(ALinkV.cnt > 0) //修改了联动为1条时删除单条联动没有清掉ALinkV.cnt的问题 2015/09/17 yanhuan adding
	{
		ALinkV.cnt--;
	}
	return TRUE;
}

/*****************************************************************************
  函数名:  void DelAllALinkValue(void)
功能说明:  删除联动触发值所有单元
    参数:  void
  返回值:  void
*****************************************************************************/
void DelAllALinkValue(void)
{
	memset((u8 *)&ALinkV, 0, sizeof(ALinkVTable));
}
/*****************************************************************************
  函数名:  void ALinkValueExe
功能说明:  联动执行
    参数:  temp--联动体
  返回值:  void
*****************************************************************************/
void ALinkValueExe(ALinkItem *temp)
{
	int vtmp = 0 ;
	SetALinkValue(temp->LinkNum, CLINKEXED);
	//执行联动
	//ExeLinkPackage(data, LINKSTD);
	vtmp = DWCHAR( temp->DDevID[3],
					temp->DDevID[2],
					temp->DDevID[1],
					temp->DDevID[0]);

	Msg_Send((ObjectType_t)temp->DType, 
			vtmp, 
			WCHAR(temp->DAddrT[1],temp->DAddrT[0]), 
			temp->UnitID, 
			temp->DestCmd, 
			temp->DestLen, 
			temp->data);
}
/*******************************************************************************
函 数 名：	CheckALinkValueAndExe
功能说明： 	判断ALINK中条件值，是否满足条件，满足条件执行
参	  数：  *data-实际联动项
            linknum-联动号
			newsamp-新的采样值
返 回 值：	1表示执行成功，0表示执行失败
*******************************************************************************/
u8 CheckALinkValueAndExe(u8 *data, u8 linknum, int newsamp)
{
	int sv1,sv2,sv3,sv4;
	u8 screv,num;
	ALinkItem *atmp;

	atmp  = (ALinkItem *)data;
	screv = GetConditionReverse(atmp->SCondition);
	
	memcpy((u8 *)&sv1, (u8*)atmp->SValue, sizeof(int));
	sv3 = sv1;
	sv2 = 0;
	sv4 = 0;
	
	GetConditionOffset(atmp->SCondition, &sv1, &sv2);

	if((num = FindALinkValueNum(linknum)) == 0xFF)
	{
		return FALSE;
	}

	if(CmpSampAndLinkValue((CONDITION)(atmp->SCondition), sv1, sv2, newsamp))
	{
		//判断上一次是否，不满足条件
		if(ALinkV.LinkV[num].FirstFlag)
		{
			if(CmpSampAndLinkValue((CONDITION)screv, sv1, sv2, ALinkV.LinkV[num].OldValue))
			{
				//已触发过了吗?防止抖动
				if(CLINKEXED != GetALinkValue(atmp->LinkNum))
				{
					ALinkValueExe(atmp);
				}
			}
		}
		else //未触发过，该联动未执行过
		{
			ALinkV.LinkV[num].FirstFlag = 1;
			ALinkValueExe(atmp);
		}
		//保存上一次值
		ALinkV.LinkV[num].OldValue = newsamp;
		return TRUE;
	}
	
	GetConditionOffset(screv, &sv3, &sv4);
	
	if(CmpSampAndLinkValue((CONDITION)screv, sv3, sv4, newsamp))
	{
		//判断上一次是否，不满足条件
		if(CmpSampAndLinkValue((CONDITION)(atmp->SCondition), sv3, sv4, ALinkV.LinkV[num].OldValue))
		{
			SetALinkValue(atmp->LinkNum, 0);//清执行标志
			//可以考虑执行反向联动
		}
	}
	
	ALinkV.LinkV[num].OldValue = newsamp;//保存上一次值
	return TRUE;
}
#endif
#endif
/**************************Copyright BestFu 2014-05-14*************************/



