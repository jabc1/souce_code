/***************************Copyright BestFu 2014-05-14*************************
文	件：	BestFulib.c
说	明：	BestFu自建Lib库文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014-05-30
修  改:     2014-09-27 Unarty Add memfind && memcharget
*******************************************************************************/
#include "BestFuLib.h"


/*******************************************************************************
函 数 名：	memcpy
功能说明： 	内存考备
参	  数： 	dest:	目标内存地址
			src:	源内存地址
			num:	考备内存大小
返 回 值：	无
*******************************************************************************/
void memcpy(void *dest,  const void *src, unsigned int num)
{
	char *pd, *ps;
	
	if (dest != src)	//目标与源不是同一位置
	{
		for (pd = (char*)dest, ps = (char*)src; num > 0; num--)
		{
			*pd++ = *ps++;
		}
	}
}
                                                                                
/*******************************************************************************
函 数 名：	memcmp
功能说明： 	内存比较
参	  数： 	dest:	目标内存地址
			src:	源内存地址
			num:	比较内存大小
返 回 值：	1(相同)/0
*******************************************************************************/
char memcmp(void *dest,  const void *src, unsigned int num)
{
	char *pd, *ps;
	
	if (dest != src)	//目标与源不是同一位置
	{
		for (pd = (char*)dest, ps = (char*)src; num > 0; num--)
		{
			if (*pd++ != *ps++)
			{
				return 1;
			}
		}
	}
		
	return 0;
}

/*******************************************************************************
函 数 名：	memset
功能说明： 	内存设置
参	  数： 	dest:	目标内存地址
			val:	目标值
			num:	比较内存大小
返 回 值：	无
*******************************************************************************/
void memset(void *dest, unsigned char const val, unsigned int num)
{
	char *pd;
	
	for (pd = (char*)dest; num > 0; num--)
	{
		*pd++ = val;
	}
}

/*******************************************************************************
函 数 名：	memmove
功能说明： 	内存移动
参	  数： 	dest:	目标内存地址
			src:	源内存地址
			num:	移动内存大小
返 回 值：	无
*******************************************************************************/
void memmove(void *dest,  const void *src, unsigned int num)
{
	char *pd, *ps;
	
	if (dest < src)
	{
		for (pd = (char*)dest, ps = (char*)src; num > 0; num--)
		{
			*pd++ = *ps++;
		}
	}
	else if (dest > src)
	{
		for (pd = (char*)((unsigned int)dest + num), ps = (char*)((unsigned int)src + num);
				num > 0; num--)
		{
			*(--pd) = *(--ps);
		}
	}
}

/*******************************************************************************
函 数 名:  	memfind
功能说明:  	内存查找
参    数:  	dest:	查找目标
			dLen：	目标有效长度
			src:	查找源
			sLen:	源数据大小
返 回 值:  	NULL(未找到)/目标位于源的地址
*******************************************************************************/
void* memfind(void *dest, unsigned int dLen, void *src, unsigned int sLen)
{
	char *pDest, *pSrc;
	unsigned int i;
	
	for (pDest = dest, pSrc = src; sLen > 0; sLen--, pSrc++)
	{
		if (*pDest == *pSrc++)	//源中找到与目标相同数据
		{
			for (i = 1; i < dLen && pDest[i] == *pSrc++; i++) //比较后面数据是否相等
			{}
			if (i == dLen)	//源中找到与与目标完全相等的数据片
			{
				return (pSrc - i);
			}
		}
	}
	
	return 0;
}

/*****************************************************************************
函 数 名:  	memcharget
功能说明:  	内存字符串获取
参    数:  	dest:	目标存储地址
			len:	返回长度
			src:	内存查找源
			size:	查找源大小
返 回 值:  	无
*****************************************************************************/
void memcharget(void *dest, unsigned char *len, void const *src, unsigned int size)
{
	char *pDest;
	char const *pSrc;
	
	for (*len = 0, pDest = dest, pSrc = src; size > 0; size--)
	{
		*pDest++ = *pSrc++;
		*len += 1;
		if (*pSrc == '\0')
		{
			return;
		}
	}
	*len = 0;
}

///*****************************************************************************
//函 数 名:  	Char_hex
//功能说明:  	字符串转16进制
//参    数:  	dest:	目标存储地址
//			len:	返回长度
//			src:	源字符内容
//			size:	源字符内容大小
//返 回 值:  	1:转换成功/0: 字符串中有非16进制描述字符
//*****************************************************************************/
//char Char_hex(void *dest, unsigned char *len, void const *src, unsigned int size)
//{
//	char *pDest = dest;
//	char const *pSrc = src;
//	int i;
//	
//	for (*len = 0; size > 0; size--)
//	{
//		if (*pSrc >= 'a')
//		{
//			i = *pSrc - 'a' + 10;
//		}
//		else if (*pSrc >= 'A')
//		{
//			i = *pSrc - 'A' + 10;
//		}
//		else if (*pSrc >= '0' && *pSrc <= '9')
//		{
//			i = *pSrc - '0';
//		}
//		else
//		{
//			return 0;
//		}
//		
//		if (i > 15)
//		{
//			return 0;
//		}
//		if (size%2)
//		{
//			*pDest = i<<4;
//		}
//		else
//		{
//			*pDest += i;
//			pDest++;
//			(*len)++;
//		}
//		pSrc++;
//	}

//	return 1;
//}

/**************************Copyright BestFu 2014-05-14*************************/
