/*******************************************************************************
**	文    件: HT1621.c
**  功    能：段码液晶驱动IC
**  编    译：Keil uVision5 V5.10
**	芯    片: STM32F1xx
**  版    本：V1.0.0
**  编    写：Seven
**  创建日期：2014.11.17
**  修改日期：2014.11.17
**  说    明： 
********************************************************************************/

#include "HT1621.H"

//HT1621	操作模式	仅高三位有效
#define MODE_READ		0xC0	// 110
#define MODE_WRITE		0xA0	// 101
#define MODE_COMMAND	0x80	// 100


//HT1621 	各种操作有效位长度
typedef enum
{
	LEN_MODE = 3,
	LEN_DATA = 4,
	LEN_ADDR = 6,	
	LEN_CMD	 = 9			
}Ht1621_BitLen;

/* HT1621 Commond    */
typedef enum
{
	SYSDIS = 0x00,
	SYSEN  = 0x01,		//打开振荡器
	LCDOFF = 0x02,		//关闭偏压发生器
	LCDON  = 0x03,		//打开偏压发生器
	BIAS   = 0x29,		//4个公共口  1/2偏压
	//BIAS   = 0x20,	//2个公共口  1/2偏压
	RC256  = 0x18		//内部时钟
}HT1621_CMD;

//8bit x 3 液晶段码
//0123456789AbCdEF
const u8 DisSegTap[20] = {0xAF,0x6,0xCB,0x4F,0x66,0x6D,0xED,0x7,0xEF,0x6F,0xE7,0xEC,0xA9,0xCE,0xE9,0xE1};

const u8 DisSegTap2[10] = { 0xEB,0x60,0xC7,0xE5,0x6C,0xAD,0xAF,0xE0,0xEF,0xED};




void HT1621_Write_HBit(u8 Data, Ht1621_BitLen BitLen);
void HT1621_Write_LBit(u8 Data, Ht1621_BitLen BitLen);
void HT1621_SendCommand(HT1621_CMD Command);

/*******************************************************************************
**函	数:  	HT1621_Init()
**功	能:  	HT1621初始化
**参    数:  	null
**返	回:  	null
**说	明：	
********************************************************************************/
//#pragma arm section code=".ARM.__at_0x08019000"
void HT1621_Init(void) 
{
//  u8 i = 0;
	GPIOx_Cfg(PORT_HT1621_CS  , PIN_HT1621_CS  , OUT_PP_50M);
	//GPIOx_Cfg(PORT_HT1621_RD  , PIN_HT1621_RD  , OUT_PP_50M);
	GPIOx_Cfg(PORT_HT1621_WR  , PIN_HT1621_WR  , OUT_PP_50M);
	GPIOx_Cfg(PORT_HT1621_DATA, PIN_HT1621_DATA, OUT_PP_50M);
	HT1621_DATA_1;
	HT1621_WR_1;
	//HT1621_RD_1;
	HT1621_CS_1; 

	HT1621_SendCommand(BIAS);
	HT1621_SendCommand(SYSEN);
	HT1621_SendCommand(LCDON);   
    
//    for(i=0;i<32;i++)
//        HT1621_SendDisdata(i,0x00);
}
//#pragma arm section
void HT1621_lowPower(void)
{
	HT1621_SendCommand(LCDOFF);  
	HT1621_SendCommand(SYSDIS);	
}
/*******************************************************************************
**函	数:  	HT1621_Delay()
**功	能:  	HT1621 延时函数   控制传输速度
**参    数:  	null
**返	回:  	null
**说	明：	
********************************************************************************/
void HT1621_Delay(void)
{
	delay_us(5);
}

/*******************************************************************************
**函	数:  	HT1621_Write_HBit()
**功	能:  	写数据 从高位开始
**参    数:  	Data		--要发生的数据（高位有效）
				BitLen		--要发生的位长度
**返	回:  	null
**说	明：	适用于写 命令
				上升沿数据有效
********************************************************************************/
void HT1621_Write_HBit(u8 Data, Ht1621_BitLen BitLen)
{
    u8 i;
	for( i=BitLen; i>0; i--)
	{
		HT1621_WR_0;
	    if(Data&0x80)	HT1621_DATA_1;    
		else			HT1621_DATA_0;
		Data = Data<<1;
		HT1621_Delay();
		HT1621_WR_1;
		HT1621_Delay();
	}
}

/*******************************************************************************
**函	数:  	HT1621_Write_Lbit()
**功	能:  	写数据  从低位开始
**参    数:  	Data		--要发送的数据（高位有效）
				BitLen		--要发送的位长度
**返	回:  	null
**说	明：	适用于数据
				上升沿数据有效
********************************************************************************/
void HT1621_Write_LBit(u8 Data, Ht1621_BitLen BitLen)
{
    u8 i;
	for( i=BitLen; i>0; i--)
	{
		HT1621_WR_0;
	    if(Data&0x01)	HT1621_DATA_1;    
		else			HT1621_DATA_0;
		Data = Data>>1;
		HT1621_Delay();
		HT1621_WR_1;
		HT1621_Delay();
	}
}

/*******************************************************************************
**函	数:  	HT1621_SendCommand()
**功	能:  	发生命令
**参    数:  	Command		--要发送的数据（高位有效）
**返	回:  	null
**说	明：	适用于数据
				上升沿数据有效
********************************************************************************/
void HT1621_SendCommand(HT1621_CMD Command)
{
    HT1621_CS_0;
	HT1621_Delay();
    HT1621_Write_HBit(MODE_COMMAND ,LEN_MODE);
    HT1621_Write_HBit(Command	   ,LEN_CMD );
	HT1621_CS_1;
	HT1621_Delay();
}

/*******************************************************************************
**函	数:  	HT1621_SendDisdata()
**功	能:  	发送显示数据
**参    数:  	Data		--要发送的数据（高位有效）
				BitLen		--要发送的位长度
**返	回:  	null
**说	明：	适用于数据
				上升沿数据有效
********************************************************************************/
void HT1621_SendDisdata(u8 Address, u8 Disdata)
{
    HT1621_CS_0;
	HT1621_Delay();
    HT1621_Write_HBit(MODE_WRITE,LEN_MODE);
    HT1621_Write_HBit(Address<<2,LEN_ADDR);
	HT1621_Write_LBit(Disdata	,LEN_DATA);
	//....     //可连续写
	HT1621_CS_1;
	HT1621_Delay();
}



//==============================================================================
/*
idea:当前只使用  COM0段

*/
void HT1621_WriteBit(u8 Address,u8 Len,u8 Data)
{
	u8 i;
	u8 tmp=Data;
    HT1621_CS_0;
	HT1621_Delay();
    HT1621_Write_HBit(MODE_WRITE,LEN_MODE);
    HT1621_Write_HBit(Address<<2,LEN_ADDR);
	for(i=0;i<Len;i++)
	{
		if(tmp&0x01) HT1621_Write_LBit(1,LEN_DATA);
		else         HT1621_Write_LBit(0,LEN_DATA);
		tmp>>=1;
	}
	HT1621_CS_1;
	HT1621_Delay();
}

	
/*******************************************************************************
**函	数:		HT1621_DispNum()
**功	能:  	HT1621指定位置显示数字
**参    数:  	Seat		--要显示的位置
				Num			--要要显示的字符
**返	回:  	null
**说	明：	
----------------------------------------------------
	idea :	以SEAT 为单位，写液晶屏
			一个SEAT分两段写
			
********************************************************************************/
void HT1621_DispNum(u8 Seat , u8 Num)
{
	if(Seat ==1)
	{
		HT1621_SendDisdata( 4,DisSegTap[Num]>>4);
        HT1621_SendDisdata( 5,DisSegTap[Num]);
	}
	else if(Seat ==2)
	{
		HT1621_SendDisdata( 6,DisSegTap[Num]>>4);
		HT1621_SendDisdata( 7,DisSegTap[Num]);	
	}
	else if(Seat ==3)
	{
		HT1621_SendDisdata( 1, (DisSegTap2[Num]>>4)|0X01 );  // 
		HT1621_SendDisdata( 2,DisSegTap2[Num]);	
	}
}




