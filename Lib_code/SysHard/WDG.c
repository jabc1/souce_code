/***************************Copyright BestFu 2014-05-14*************************
文	件：	WDG.c
说	明：	看门狗相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.05.14 
修　改：	暂无
*******************************************************************************/
#include "wdg.h"
#include "GPIO.h"
#include "stm32f10x.h"
#include "UnitCfg.h"
#include "Thread.h"
#include "SysHard.h"


/*******************************************************************************
  函数名:  	WDG_Init	
功能说明:  	看门狗初始化函数
	参数:  	prer: 	分频数:0~7(只有低3位有效!)	
			rlr:	重装载寄存器值:(0~2048)
  返回值:  	无
	注意：	时间计算(大概):Tout=((4*2^prer)*rlr)/40 (ms)	
			当外部定义HardWDG，则启用硬件看门狗，喂狗功能，基本喂狗时间看硬件参数
*******************************************************************************/
void WDG_Init(u8 prer,u16 rlr) 
{
	IWDG->KR  = 0X5555;		//使能对IWDG->PR和IWDG->RLR的写		 										  
  	IWDG->PR  = prer;  		//设置分频系数   
  	IWDG->RLR = rlr;  		//从加载寄存器 IWDG->RLR  
	IWDG->KR  = 0XAAAA;		//reload											   
  	IWDG->KR  = 0XCCCC;		//使能看门狗	
	
#if HardWDG
	GPIOx_Cfg(WDG_PORT, WDG_PIN, OUT_PP_2M);
#endif
	
	Thread_Login(FOREVER, 0, 800, &WDG_Feed);	//注册喂狗
}

/*******************************************************************************
函 数 名：	WDG_Feed
功能说明： 	喂狗函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void WDG_Feed(void)
{
	IWDG->KR=0XAAAA;	//喂软件狗
	
#if HardWDG
		GPIOx_Rvrs(WDG_PORT, WDG_PIN);//喂硬件狗
#endif

}

/*******************************************************************************
函 数 名：	WDG_FeedOFF
功能说明： 	关闭喂狗
参	  数： 	无
返 回 值：	无
注	  意:	调用本函数后系统将被看门狗拉复位，并不可取消
*******************************************************************************/
void WDG_FeedOFF(void)
{
	Thread_Logout(&WDG_Feed);
}

/*******************************************************************************
函 数 名:  	HardWDGTest_Attr
功能说明:  	硬件看门狗测试
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t HardWDGTest_Attr(u8 *data)
{
#if HardWDG
	GPIOx_Cfg(WDG_PORT, WDG_PIN, IN_FLOATING);
	return COMPLETE;
#else
	return CMD_EXE_ERR;
#endif
}

/**************************Copyright BestFu 2014-05-14*************************/
