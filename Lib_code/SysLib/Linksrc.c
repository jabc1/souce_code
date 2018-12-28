/***************************Copyright BestFu 2014-05-14*************************
文	件：	Linksrc.c
说	明：	源的执行入口和整个联动的初始化工作源文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	暂无
*******************************************************************************/

#include "linkall.h"

//#if (SLINK_SRC_VALUE_EN > 0)
//SrcOldValTable SrcOld;
//#endif

/*******************************************************************************
函 数 名：	LinkInit
功能说明： 	所有联动的初始化
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void LinkInit(void)
{
#if (ALINK_EN > 0)
	ALinkInit();
#endif
#if (SLINK_EN > 0)
	SLinkInit();
#endif
#if (CLINK_EN > 0)
	CLinkInit();
#endif
}

/*******************************************************************************
函 数 名：	LinkClr
功能说明： 	清除所有联动
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void LinkClr(void)
{
#if (ALINK_EN > 0)
	ALinkClr();
#endif
#if (SLINK_EN > 0)
	SLinkClr();
#endif
#if (CLINK_EN > 0)
	CLinkClr();
#endif
}

/*******************************************************************************
函 数 名：	CmpSampAndLinkValue
功能说明： 	比较采样值与联动的值，不同条件下，是否满足范围
参	  数： 	Cndt为0x01-0x08，表示不同的值条件
            Lvtmp1为联动表中值1,当Cndt为1-6时，即为判断的目标数值；
                               当Cndt为7-8时，即为判断的左值；
            Lvtmp2为联动表中值2,当Cndt为1-6时，数据无效；
                               当Cndt为7-8时，即为判断的右值；
            Svtmp为采样值，当前设备的采样值。
返 回 值：	1表示满足条件，0表示不满足条件
*******************************************************************************/
u8 CmpSampAndLinkValue(u8 Cndt, int Lvtmp1, int Lvtmp2, int Svtmp)
{
	switch(Cndt)
	{
		case CLARGE:        if(Lvtmp1 <  Svtmp)return TRUE; else return FALSE;
		case CLARGEOREQUAL: if(Lvtmp1 <= Svtmp)return TRUE; else return FALSE;
		case CSMALL:        if(Lvtmp1 >  Svtmp)return TRUE; else return FALSE;
		case CSMALLOREQUAL: if(Lvtmp1 >= Svtmp)return TRUE; else return FALSE;
		case CEQUAL:        if(Lvtmp1 == Svtmp)return TRUE; else return FALSE;
		case CNOTEQUAL:     if(Lvtmp1 != Svtmp)return TRUE; else return FALSE;
		case CINRANGE:      if((Lvtmp1 <= Svtmp) && (Lvtmp2 >= Svtmp))
								return TRUE; 
							else 
								return FALSE;
		case COUTRANGE:		if((Lvtmp1 > Svtmp) || (Lvtmp2 < Svtmp))
								return TRUE; 
							else 
								return FALSE;
		default:            return FALSE;
	}
}

/*******************************************************************************
函 数 名：	GetConditionReverse
功能说明： 	从一个条件号，获取其反面条件号
参	  数： 	SrcCndt(source condition)缩写
            对应的反向逻辑是数学简单的判断
返 回 值：	表示反面的条件号
*******************************************************************************/
u8 GetConditionReverse(u8 SrcCndt)
{
	switch(SrcCndt)
	{
		case CEQUAL:         return CNOTEQUAL;
		case CNOTEQUAL:      return CEQUAL;
		case CLARGE:         return CSMALLOREQUAL;
		case CSMALLOREQUAL:  return CLARGE;
		case CSMALL:         return CLARGEOREQUAL;
		case CLARGEOREQUAL:  return CSMALL;	
		case CINRANGE:       return COUTRANGE;
		case COUTRANGE:      return CINRANGE;
		default:             return FALSE;
	}
}

/*******************************************************************************
函 数 名：	GetConditionOffset
功能说明： 	获取偏移量，在不同的条件下，偏移量的方向不同，在灵敏变化的采样值中有
            个偏移量在中间，可以保证联动的触发有规律，不乱触发
参	  数： 	SrcCndt(source condition)缩写
            表示对应的反向逻辑是数学简单的判断
            sv1为联动表中值1，当Cndt为1-6时，即为判断的目标数值；
                             当Cndt为7-8时，即为判断的左值；
            sv2为联动表中值1, 当Cndt为1-6时，数据无效；
                             当Cndt为7-8时，即为判断的右值；

            此处的OFFSET不能太大，大了没意义，小了效果不明显，取值需要谨慎，尤其
            在范围内和范围外的时候，不能让取值与偏移量OFFSET计算完后出现矛盾。
返 回 值：	1表示成功，0表示失败
*******************************************************************************/
u8 GetConditionOffset(u8 SrcCndt, int *sv1, int *sv2)
{
	switch(SrcCndt)
	{
		case CLARGE:         
		case CLARGEOREQUAL:  *sv1 += OFFSET;    break;
		case CSMALL:         
		case CSMALLOREQUAL:  *sv1 -= OFFSET;    break;
		case CINRANGE:       *sv1 += OFFSET;    *sv2 -= OFFSET;  break;
		case COUTRANGE:      *sv1 += OFFSET;    *sv2 -= OFFSET;  break;
		default:             return FALSE;
	}
	return TRUE;
}

#if (SLINK_SRC_VALUE_EN > 0)
///*******************************************************************************
//函 数 名：	FindOldValueTableFull
//功能说明： 	上一次采样值列表是否已经满了，按唯一单元号与属性号来存
//参	  数： 	unitID-单元ID号，propID-属性ID号
//返 回 值：	1表示未满，0表示已满
//*******************************************************************************/
//u8 FindOldValueTableFull(u8 unitID, u8 propID)
//{
//	if(SrcOld.Cnt >= MAX_SRC_OLD_CNT)
//	{
//		return FALSE;
//	}
//	return TRUE;
//}

///*******************************************************************************
//函 数 名：	FindOldValueTableItem
//功能说明： 	寻找unitID,propID对应的上次采样值是否存在，存在返回对应位置号
//            不存在的话，返回OxFF。
//参	  数： 	unitID-单元ID号，propID-属性ID号
//返 回 值：	0-254返回对应的位置号，255表示不存在
//*******************************************************************************/
//u8 FindOldValueTableItem(u8 unitID, u8 propID)
//{
//	u8 i;
//	for(i=0; i<SrcOld.Cnt; i++)
//	{
//		if( SrcOld.src[i].UnitID == unitID &&
//			SrcOld.src[i].PropID == propID )
//		{
//			return i;
//		}
//	}
//	return 0xFF;
//}

///*******************************************************************************
//函 数 名：	AddOldValueTableItem
//功能说明： 	将旧值存于队列中
//参	  数： 	unitID-单元ID号，propID-属性ID号
//返 回 值：	0-254返回对应的位置号，255表示不存在
//*******************************************************************************/
//u8 AddOldValueTableItem(u8 unitID, u8 propID)
//{
//	if(FindOldValueTableFull(unitID, propID))
//	{
//		return FALSE;
//	}
//	else
//	{
//		SrcOld.src[SrcOld.Cnt].UnitID   = unitID;
//		SrcOld.src[SrcOld.Cnt].PropID   = propID;
//		SrcOld.src[SrcOld.Cnt].Value    = 0;
//		SrcOld.Cnt++;
//	}
//	return TRUE;
//}
#endif

/*******************************************************************************
函 数 名：	LinkSrcExe
功能说明： 	源的事件产生或数值变化，遍历联动表，若条件满足执行联动
参	  数： 	unitID，单元ID号
            type,   类型，是事件型还是数值型
            propID, 属性号
            num，   事件型时表示动作号，数值型时表示源类型编号
返 回 值：	1表示成功，0表示失败
*******************************************************************************/
u8 LinkSrcExe(u8 unitID, u8 propID, u8 type, int num)
{
#if(ALINK_EN > 0 || SLINK_EN > 0 || CLINK_EN > 0)
	u8 i;
#endif

#if(ALINK_EN > 0)
	for(i=0;i<ALink.Cnt;i++)
	{
		if(ALink.linkitem[i].UseFlag == 0x01)
		{
			if(type == SRCEVENT)
			{
	#if(SLINK_SRC_EVENT_EN > 0)
				u32 value=0;
				value = DWCHAR( ALink.linkitem[i].SValue[3],
								ALink.linkitem[i].SValue[2],
								ALink.linkitem[i].SValue[1],
								ALink.linkitem[i].SValue[0]);
				
				if( ALink.linkitem[i].SUnitID == unitID && 
					ALink.linkitem[i].SPropID == propID)
				{
					if(CmpSampAndLinkValue(ALink.linkitem[i].SCondition, value, 0, num))
					{
						value = DWCHAR( ALink.linkitem[i].DDevID[3],
										ALink.linkitem[i].DDevID[2],
										ALink.linkitem[i].DDevID[1],
										ALink.linkitem[i].DDevID[0]);
						Msg_Send((ObjectType_t)ALink.linkitem[i].DType, 
								value, 
								WCHAR(ALink.linkitem[i].DAddrT[1],ALink.linkitem[i].DAddrT[0]), 
								ALink.linkitem[i].UnitID, 
								ALink.linkitem[i].DestCmd, 
								ALink.linkitem[i].DestLen, 
								ALink.linkitem[i].data);
					}
				}
	#endif
			}
			else if(type == SRCVALUE)
			{
	#if(ALINK_SRC_VALUE_EN > 0)
				if( ALink.linkitem[i].SUnitID == unitID && 
					ALink.linkitem[i].SPropID == propID )
				{
					CheckALinkValueAndExe((u8 *)&ALink.linkitem[i], ALink.linkitem[i].LinkNum, num);
				}
	#endif
			}
		}
	}
#endif
	return TRUE;
}
/**************************Copyright BestFu 2014-05-14*************************/
