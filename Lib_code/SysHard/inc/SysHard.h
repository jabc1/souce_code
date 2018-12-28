/***************************Copyright BestFu 2014-05-14*************************
文	件：    SysHard.h
说	明：    系统硬件相关函数
编	译：    Keil uVision4 V4.54.0.0
版	本：    v2.0
编	写：    Unarty
日	期：    2014.06.26
修　改：	暂无
*******************************************************************************/
#ifndef __SYSHARD_H
#define __SYSHARD_H

#include "BF_type.h"
#include "flash.h"
									  
#define STATUS0(GPIOx, pin)  GPIOx_Rst(GPIOx, pin) //相应管脚置0
#define STATUS1(GPIOx, pin)  GPIOx_Set(GPIOx, pin) //相应管脚置1 
#define STATUS_(GPIOx, pin)  GPIOx_Rvrs(GPIOx, pin) //相应管脚取反

//EEPROM IIC通信接口
#define SCL_PORT    GPIO_B      //SCL端口
#define SCL_PIN    	pin_6     	//SCL引脚 
													    
#define SDA_PORT    GPIO_B      //SDA端口
#define SDA_PIN     pin_7      	//SDA引脚

#define EEPROM_WP		1 //EEPROM写保护。 1: 有写使能保护管脚，0：无写使能保护管脚

#if EEPROM_WP
	#define WP_PORT			GPIO_B	//Write Protect port
	#define WP_PIN			pin_5	//Write Protect pin
#endif 


//进入测试模式按键
#define TEST_KEY_PORT 		GPIO_A
#define TEST_KEY_PIN  		pin_5
#define TEST_VALID_LEVEL	0		//有效电平
#if TEST_VALID_LEVEL == 1
	#define TEST_KEY_INIT 	GPIOx_Cfg(TEST_KEY_PORT, TEST_KEY_PIN, IN_UPDOWN);\
							GPIOx_Rst(TEST_KEY_PORT, TEST_KEY_PIN)
	#define TEST_KEY_VAL	(1)//((GPIOx_Get(TEST_KEY_PORT, TEST_KEY_PIN))^RESET_KEY_PIN)
#else
	#define TEST_KEY_INIT 	GPIOx_Cfg(TEST_KEY_PORT, TEST_KEY_PIN, IN_UPDOWN);\
							GPIOx_Set(TEST_KEY_PORT, TEST_KEY_PIN)
	#define TEST_KEY_VAL	(1)//(GPIOx_Get(TEST_KEY_PORT, TEST_KEY_PIN))
#endif


//用户数据重置端口定义 : 注意内部检测，RESET_KEY_VAL 返回：零有效， 非零无效
#define RESET_USER_DATA			1//1：开启重置用户数据按钮。0：关闭重置用户数据按钮
#if RESET_USER_DATA
	#define RESET_KEY_PORT 		GPIO_A
	#define RESET_KEY_PIN  		pin_4
	#define RESET_VALID_LEVEL	0		//有效电平
	#if RESET_VALID_LEVEL == 1
		#define RESET_KEY_INIT GPIOx_Cfg(RESET_KEY_PORT, RESET_KEY_PIN, IN_UPDOWN);\
							GPIOx_Rst(RESET_KEY_PORT, RESET_KEY_PIN)
		#define RESET_KEY_VAL	((GPIOx_Get(RESET_KEY_PORT, RESET_KEY_PIN))^RESET_KEY_PIN)
	#else
		#define RESET_KEY_INIT GPIOx_Cfg(RESET_KEY_PORT, RESET_KEY_PIN, IN_UPDOWN);\
							GPIOx_Set(RESET_KEY_PORT, RESET_KEY_PIN)
		#define RESET_KEY_VAL	((GPIOx_Get(RESET_KEY_PORT, RESET_KEY_PIN)))
	#endif
#endif


//系统运行指示灯端口
#define RUN_INSTRUCT			0 //1：开启运行指示。0：关闭运行指示
#if RUN_INSTRUCT
	#define RUN_PORT	GPIO_A
	#define RUN_PIN		pin_8
	#define RUN_ON		STATUS0(RUN_PORT, RUN_PIN)
	#define RUN_OFF		STATUS1(RUN_PORT, RUN_PIN)
#endif


//硬件看门狗端口
#define HardWDG				1 //1：开启硬件看门狗。0：不产生硬件喂狗
#if HardWDG
	#define	WDG_PORT	GPIO_B		//硬件喂狗端口
	#define WDG_PIN		pin_1		//硬件喂狗引脚
#endif

//433 通信接口
#define nGPIO2_PORT     GPIO_B	//通信忙反馈端口
#define nGPIO2_PIN      pin_11	//通信忙反馈引脚

#define SDN_PORT        GPIO_A	//433复位端口
#define SDN_PIN         pin_8	//433复位引脚

#define nIRQ_PORT       GPIO_B  //数据中断端口  注意：中断IO修改后需要修改内部的一些配置
#define nIRQ_PIN        pin_10	//数据中断引脚

#define nSEL_PORT       GPIO_B  //SPI片选端口
#define nSEL_PIN        pin_12	//SPI片选引脚

#define RF_SWITCH		0		//无线电源开关 1:(无线有电源开关), 0(无线没有开关)
#if RF_SWITCH
#define RF_SWITCH_PORT	GPIO_B	//无线电源开关端口
#define RF_SWITCH_PIN	pin_12	//无线电源开关引脚
#define RF_SWITCH_ON	STATUS1(RF_SWITCH_PORT, RF_SWITCH_PIN)	//无线电源开
#define RF_SWITCH_OFF	STATUS0(RF_SWITCH_PORT, RF_SWITCH_PIN)	//无线电源关
#endif

#define SI4432_CH		(0)	//定义433无线信道: 86 / 85 / 84 / 83 / 82 / 81 //其他就使用其软件设置值
#if (SI4432_CH < 81 || SI4432_CH > 86)
	#undef SI4432_CH
#endif
#define SI4432_THRESHOLD 	(0)	//定义433无线信道门槛（Threshold）: 60 ～ 220. （注意 ：0 使用设备自动门槛设置）
#if (SI4432_THRESHOLD == 0)
	#undef SI4432_THRESHOLD 
#endif
//433无线信号指示端口*/
#define SIGNAL		0		//是否需要信号指示功能 1: 需要， 0：不需要
#define SIGNAL_PORT	GPIO_A
#define SIGNAL_PIN	pin_8
#define SIGNAL_ON	STATUS0(RUN_PORT, RUN_PIN)
#define SIGNAL_OFF	STATUS1(RUN_PORT, RUN_PIN)

//定义当前flash设备的大小,0表示64kflash设备,1表示128k flash设备
#define   CURRENT_DEV_FLASH_SIZE	(1u)	

/***********************************函数声明***********************************/
void SysHard_Init(void);
u32 HardID_Check(u32 id);
#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
void HardID_Change(void);
#endif

#endif 		//SysHard.h end 
/**************************Copyright BestFu 2014-05-14*************************/
