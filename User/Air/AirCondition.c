/***************************Copyright BestFu 2014-05-14*************************
文	件：	AirCondition.c
说	明：	空调外部条件函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.11.17 
修　改：	暂无
*******************************************************************************/
#include "AirCondition.h"
#include "Thread.h"
#include "AirAttr.h"
#include "ADC.h"
#include "GPIO.h"

#define R1		47000		//偏压电阻
#define R42		10000		//分压电阻
#define CQVAL   4096        //固定量化值4096，LSB=VDDA/4096     

const u16 RT_Temb_Tab[] =
{
//	184727,175140,166084,157545,149489,141890,134717,127945,121549,115507,	/*-40 - -31*/
//	109798,104402,99300,94474,89908,85588,81498,77626,73958,70483,					/*-30 - -21*/
//	67190,64069,61109,58301,55638,53110,50712,48433,46270,44214,						/*-20 - -11*/
//	42261,40404,38639,36960,35363,33959,32398,31021,29710,28462,						/*-10 - -1*/
	27209,26071,24993,23966,22985,21945,21055,20208,19399,18628,							/*0 - 9*/
	17808,17106,16436,15796,15185,14544,13987,13454,12944,12457,							/*10 - 19*/
	11952,11507,11080,10670,10279,9900, 9561, 9209, 8872, 8548,								/*20 - 29*/
	8176, 7881, 7598, 7327, 7067, 6804, 6564, 6335, 6114, 5903,								/*30 - 39*/
	5692, 5496, 5309, 5130, 4956, 4785, 4625, 4472, 4324, 4182,								/*40 - 49*/
	4045, 3913, 3786, 3664, 3547, 3434, 3324, 3220, 3119, 3021,								/*50 - 59*/
	2927, 2837, 2750, 2665, 2584, 2506, 2430, 2357, 2287, 2219,								/*60 - 69*/
};

u16 RT_Tab[sizeof(RT_Temb_Tab)/sizeof(RT_Temb_Tab[0])];

//const u16 RT_Tab[] = {CQVAL*(27766)/(27766+R1), CQVAL*(26592)/(26592+R1), CQVAL*(25482)/(25482+R1), CQVAL*(24424)/(24424+R1), CQVAL*(23415)/(23415+R1)
//						, CQVAL*(22346)/(22346+R1), CQVAL*(21430)/(21430+R1), CQVAL*(20560)/(20560+R1), CQVAL*(19728)/(19728+R1), CQVAL*(18936)/(18936+R1)  //9
//						, CQVAL*(18094)/(18094+R1), CQVAL*(17374)/(17374+R1), CQVAL*(16687)/(16687+R1), CQVAL*(16031)/(16031+R1), CQVAL*(15404)/(15404+R1)
//						, CQVAL*(14748)/(14748+R1), CQVAL*(14177)/(14177+R1), CQVAL*(13632)/(13632+R1), CQVAL*(13110)/(13110+R1), CQVAL*(12611)/(12611+R1)	//19
//						, CQVAL*(12095)/(12095+R1), CQVAL*(11640)/(11640+R1), CQVAL*(11204)/(11204+R1), CQVAL*(10786)/(10786+R1), CQVAL*(10386)/(10386+R1)
//	
//						, CQVAL*(10000)/(10000+R1), CQVAL*(9660)/(9660+R1), CQVAL*(9308)/(9308+R1), CQVAL*(8970)/(8970+R1), CQVAL*(8647)/(8647+R1)	//29
//	
//						, CQVAL*(8274)/(8274+R1), CQVAL*(7978)/(7978+R1), CQVAL*(7695)/(7695+R1), CQVAL*(7423)/(7423+R1), CQVAL*(7162)/(7162+R1)
//	
//						, CQVAL*(6898)/(6898+R1), CQVAL*(6657)/(6657+R1), CQVAL*(6427)/(6427+R1), CQVAL*(6205)/(6205+R1), CQVAL*(5993)/(5993+R1)	//39
//	
//						, CQVAL*(5781)/(5781+R1), CQVAL*(5584)/(5584+R1), CQVAL*(5395)/(5395+R1), CQVAL*(5215)/(5215+R1), CQVAL*(5040)/(5040+R1)
//	
//						, CQVAL*(4868)/(4868+R1), CQVAL*(4707)/(4707+R1), CQVAL*(4552)/(4552+R1), CQVAL*(4403)/(4403+R1), CQVAL*(4260)/(4260+R1)	//49
//	
//						, CQVAL*(4121)/(4121+R1), CQVAL*(3988)/(3988+R1), CQVAL*(3860)/(3860+R1), CQVAL*(3737)/(3737+R1), CQVAL*(3619)/(3619+R1)
//	
//						, CQVAL*(3504)/(3504+R1), CQVAL*(3394)/(3394+R1), CQVAL*(3288)/(3288+R1), CQVAL*(3186)/(3186+R1), CQVAL*(3087)/(3087+R1)	//59
//	
//						, CQVAL*(2992)/(2992+R1), CQVAL*(2901)/(2901+R1), CQVAL*(2813)/(2813+R1), CQVAL*(2726)/(2726+R1), CQVAL*(2644)/(2644+R1)
//	
//						, CQVAL*(2565)/(2565+R1), CQVAL*(2489)/(2489+R1), CQVAL*(2414)/(2414+R1), CQVAL*(2343)/(2343+R1), CQVAL*(2274)/(2274+R1)	//69
//					};


u32 ADCValue;

AirCondition_t gAirCondition;

static void AirCondition_Timer(void);

static void GetRT_Tab(void)
{
	u8 i;
	for(i=0; i<sizeof(RT_Temb_Tab)/sizeof(RT_Temb_Tab[0]); i++)
	{
		RT_Tab[i] = CQVAL*(RT_Temb_Tab[i]+R42)/(RT_Temb_Tab[i]+R42+R1);
//		RT_Tab[i] = CQVAL*(RT_Temb_Tab[i])/(RT_Temb_Tab[i]+R1);//修改温度之前，太阳岛发货使用,这个定为分支1
	}
}
/*****************************************************************************
函 数 名:  	AirCondition_Init
功能说明:  	空调外部条件初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void AirCondition_Init(void)
{
	Adc_Init();
	GPIOx_Cfg(GPIO_A, pin_0, IN_);		//配置PA0管脚模拟输入
	
	GetRT_Tab();  //add  zyh 2015.04.27
	
	gAirCondition.timer = 0;	//空调定时默认关
	Thread_Login(FOREVER, 0, 50, AirCondition_Temp);	//后台实时更新当前环境温度
}

/*****************************************************************************
函 数 名:  	AirCondition_Temp
功能说明:  	空调外部温度更新
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void AirCondition_Temp(void)
{
	static u8 cnt = 0;

	static u16 Average = 0;
	u16 val;
	
	val = Get_Adc(0);
	
//	if ((Average > val ? Average - val : val - Average) > 200)	//取值波动较大
//	{
//		ADCValue = val;
//		Average = val;
//		cnt = 0;
//	}	
//	else	//取平均值
//	{
//		ADCValue += val;
//		Average = ADCValue/(++cnt);   //取每次的平均值	
//	}
//	
	ADCValue += val;
	if (++cnt >= 10)	//取样十次
	{
		cnt = 0;
		Average = ADCValue/10;
		for (val = 0; val < sizeof(RT_Tab)/sizeof(RT_Tab[0]); val++)
		{
			if (Average >= RT_Tab[val])
			{
				gAirCondition.temp = val + gAirCondition.Correction_temperature;
				break;
			}
		}
		ADCValue = 0;
	}
}

/*****************************************************************************
函 数 名:  	AirCondition_TimerSet
功能说明:  	空调运行定时设置
参    数:  	val:  目标时间值
返 回 值:  	无
*****************************************************************************/
void AirCondition_TimerSet(u8 val)
{
	gAirCondition.timer = val%10;
	Thread_Login(ONCEDELAY, 0, 60*60*1000, AirCondition_Timer);
}

/*****************************************************************************
函 数 名:  	AirCondition_Timer
功能说明:  	空调运行定时
参    数:  	无
返 回 值:  	无
*****************************************************************************/
static void AirCondition_Timer(void)
{
	if (gAirCondition.timer < 2)
	{
		u8 data[4];
	
		/*空调关指令*/
		data[0]	= 0x01;	//单元号
		data[1]	= 0x03;	//属性号
		data[2]	= 0x01;	//参数长度
		data[3]	= 0x00;	//空调关
		Set_AirStatus_Attr((UnitPara_t*)data);
		gAirCondition.timer = 0;
	}
	else
	{
		gAirCondition.timer--;	//更新剩余时间
		Thread_Login(ONCEDELAY, 0, 60*60*1000, AirCondition_Timer);
	}
}

/**************************Copyright BestFu 2014-05-14*************************/
