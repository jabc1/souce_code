/*******************************************************************************
**	文    件: LCD_Show.c
**  功    能：段码液晶显示
**  编    译：Keil uVision5 V5.10
**	芯    片: STM32F1xx
**  版    本：V1.0.0
**  编    写：Seven
**  创建日期：2014.11.17
**  修改日期：2014.11.17
**  说    明： 
********************************************************************************/
#include "config.h"
#include "HT1621.h"
#include "LCD_Show.h"
#include "AirUnit.h"
#include "Thread.h"
#include "AirAttr.h"
#include "Polling.h"

const u8 DisModeTap[5] = {0x08,0x04,0x02,0x01 };    //空调模式
const u8 DisWindTap[4] = {0x08,0x04,0x02,0x01 };    //空调风速

static u8 LcdReg0=0x05;
extern AirStatus_t gAirStatus;
extern airlib_t air_config;
extern Air_Para_t gAirConfigPara;
/*******************************************************************************
**功	能:  	显示温度
**参    数:  	T 显示的温度值   0~99
**              flag  0:设置温度   1：室内温度
**返	回:  	null
********************************************************************************/
void LCD_ShowTemper(u8 T ,u8 flag)
{
		if((T>99)||(flag>1)) return;
    
    if(flag ==0)    HT1621_SendDisdata( 4,(DisSegTap[T/10]>>4) |0x01);
    else            HT1621_SendDisdata( 4,DisSegTap[T/10]>>4);
	
    HT1621_SendDisdata( 5,DisSegTap[T/10]);
	
    if(flag ==1)    HT1621_SendDisdata( 6,(DisSegTap[T%10]>>4) |0x01);
    else            HT1621_SendDisdata( 6,DisSegTap[T%10]>>4);
	
	HT1621_SendDisdata( 7,DisSegTap[T%10]);	
}

/*******************************************************************************
**功	能:  	显示模式
**参    数:  	0~4
**返	回:  	null
********************************************************************************/
void LCD_ShowMode(u8 mode)
{
    if(mode > 4 ) return;
        
    if(mode == 4)
    {
        LcdReg0 |= 0x02;
        HT1621_SendDisdata(0,LcdReg0);  
        HT1621_SendDisdata(3,0);        
    }
    else
    {
        LcdReg0 &= 0xFD;
        HT1621_SendDisdata(0,LcdReg0);  
        HT1621_SendDisdata(3,DisModeTap[mode]);
    }
}

/*******************************************************************************
**功	能:  	显示风速
**参    数:  	0~3
**返	回:  	null
********************************************************************************/
void LCD_ShowWindSpeed(u8 wind)
{
    if(wind > 3 ) return;
    
    HT1621_SendDisdata(31,DisWindTap[wind]);
}

/*******************************************************************************
**功	能:  	显示分割线
**参    数:  	null
**返	回:  	null
********************************************************************************/
void LCD_ShowLine(void)
{
    HT1621_SendDisdata(0,0x5);
}

/*******************************************************************************
**功	能:  	定时器图标开关
**参    数:  	0：关闭    1：开启 
**返	回:  	null
********************************************************************************/
void LCD_ShowTimeLogo(u8 sta)
{
    if(sta)
    {
        LcdReg0 |= 0x08;
        HT1621_SendDisdata(0,LcdReg0);
    }
    else
    {
        LcdReg0 &= 0xFE;
        HT1621_SendDisdata(0,LcdReg0);
    }
}

//0:关闭定时时间
//1~9:显示定时时间
void LCD_ShowTime(u8 t)
{
    if(t==0)
    {
        HT1621_SendDisdata( 1,0  );  // 
		HT1621_SendDisdata( 2,0);	
    }
    else 
    {   
        HT1621_DispNum(3,t); 
    }
}
/*******************************************************************************
**功	能:  	关闭多余显示
**参    数:  	null
**返	回:  	null
********************************************************************************/
void LCD_ShowPowerOff(void)
{
    LCD_ShowLine(); 
    HT1621_SendDisdata(1,0);
    HT1621_SendDisdata(2,0);
    HT1621_SendDisdata(3,0);
    HT1621_SendDisdata(31,0);
}

/*******************************************************************************
**功	能:  	显示初始化
**参    数:  	0~9 
**返	回:  	null
********************************************************************************/
void LCD_Init(void)
{
    HT1621_Init();
    LCD_ShowLine();
}
/*******************************************************************************
**功	能:  	显示所有图标
**参    数:  	void
**返	回:  	void
********************************************************************************/
void LCD_ShowALL(void)
{
	LCD_ShowLine(); 
	LCD_ShowTemper(88,0);
	HT1621_SendDisdata(6,0x0F);
  HT1621_SendDisdata(0,0x0F);
	HT1621_SendDisdata(1,0x0F);
  HT1621_SendDisdata(2,0x0F);
  HT1621_SendDisdata(3,0x0F);
	HT1621_SendDisdata(31,0x0F);
	Read_Air_Protocol();
	Config_Data_Init();
	Thread_Login(ONCEDELAY, 0, 3000, Air_StatusShow_thread);
	Thread_Login(FOREVER, 0, 50, Air_run);
}

/*******************************************************************************
**功	能:  	读取空调协议
**参    数:  	void
**返	回:  	void
********************************************************************************/
void Read_Air_Protocol(void)
{
	u8 i;
	memcharget(&air_config,&i,AIR_EXPLAIN,sizeof(airlib_t));
	RS485_Gateway_Thread();
}
/*******************************************************************************
**功	能:  	读取空调协议
**参  数:  	void
**返	回:  	void
********************************************************************************/
void RS485_Gateway_Thread(void)
{
	if(Air_RS485_Gateway == gAirConfigPara.Air_type)
	{
		Thread_Login(FOREVER, 0, 100, RS485_Gateway);
	}
}
/**************************Copyright BestFu 2014-05-14*************************/
