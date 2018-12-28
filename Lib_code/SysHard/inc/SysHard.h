/***************************Copyright BestFu 2014-05-14*************************
��	����    SysHard.h
˵	����    ϵͳӲ����غ���
��	�룺    Keil uVision4 V4.54.0.0
��	����    v2.0
��	д��    Unarty
��	�ڣ�    2014.06.26
�ޡ��ģ�	����
*******************************************************************************/
#ifndef __SYSHARD_H
#define __SYSHARD_H

#include "BF_type.h"
#include "flash.h"
									  
#define STATUS0(GPIOx, pin)  GPIOx_Rst(GPIOx, pin) //��Ӧ�ܽ���0
#define STATUS1(GPIOx, pin)  GPIOx_Set(GPIOx, pin) //��Ӧ�ܽ���1 
#define STATUS_(GPIOx, pin)  GPIOx_Rvrs(GPIOx, pin) //��Ӧ�ܽ�ȡ��

//EEPROM IICͨ�Žӿ�
#define SCL_PORT    GPIO_B      //SCL�˿�
#define SCL_PIN    	pin_6     	//SCL���� 
													    
#define SDA_PORT    GPIO_B      //SDA�˿�
#define SDA_PIN     pin_7      	//SDA����

#define EEPROM_WP		1 //EEPROMд������ 1: ��дʹ�ܱ����ܽţ�0����дʹ�ܱ����ܽ�

#if EEPROM_WP
	#define WP_PORT			GPIO_B	//Write Protect port
	#define WP_PIN			pin_5	//Write Protect pin
#endif 


//�������ģʽ����
#define TEST_KEY_PORT 		GPIO_A
#define TEST_KEY_PIN  		pin_5
#define TEST_VALID_LEVEL	0		//��Ч��ƽ
#if TEST_VALID_LEVEL == 1
	#define TEST_KEY_INIT 	GPIOx_Cfg(TEST_KEY_PORT, TEST_KEY_PIN, IN_UPDOWN);\
							GPIOx_Rst(TEST_KEY_PORT, TEST_KEY_PIN)
	#define TEST_KEY_VAL	(1)//((GPIOx_Get(TEST_KEY_PORT, TEST_KEY_PIN))^RESET_KEY_PIN)
#else
	#define TEST_KEY_INIT 	GPIOx_Cfg(TEST_KEY_PORT, TEST_KEY_PIN, IN_UPDOWN);\
							GPIOx_Set(TEST_KEY_PORT, TEST_KEY_PIN)
	#define TEST_KEY_VAL	(1)//(GPIOx_Get(TEST_KEY_PORT, TEST_KEY_PIN))
#endif


//�û��������ö˿ڶ��� : ע���ڲ���⣬RESET_KEY_VAL ���أ�����Ч�� ������Ч
#define RESET_USER_DATA			1//1�����������û����ݰ�ť��0���ر������û����ݰ�ť
#if RESET_USER_DATA
	#define RESET_KEY_PORT 		GPIO_A
	#define RESET_KEY_PIN  		pin_4
	#define RESET_VALID_LEVEL	0		//��Ч��ƽ
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


//ϵͳ����ָʾ�ƶ˿�
#define RUN_INSTRUCT			0 //1����������ָʾ��0���ر�����ָʾ
#if RUN_INSTRUCT
	#define RUN_PORT	GPIO_A
	#define RUN_PIN		pin_8
	#define RUN_ON		STATUS0(RUN_PORT, RUN_PIN)
	#define RUN_OFF		STATUS1(RUN_PORT, RUN_PIN)
#endif


//Ӳ�����Ź��˿�
#define HardWDG				1 //1������Ӳ�����Ź���0��������Ӳ��ι��
#if HardWDG
	#define	WDG_PORT	GPIO_B		//Ӳ��ι���˿�
	#define WDG_PIN		pin_1		//Ӳ��ι������
#endif

//433 ͨ�Žӿ�
#define nGPIO2_PORT     GPIO_B	//ͨ��æ�����˿�
#define nGPIO2_PIN      pin_11	//ͨ��æ��������

#define SDN_PORT        GPIO_A	//433��λ�˿�
#define SDN_PIN         pin_8	//433��λ����

#define nIRQ_PORT       GPIO_B  //�����ж϶˿�  ע�⣺�ж�IO�޸ĺ���Ҫ�޸��ڲ���һЩ����
#define nIRQ_PIN        pin_10	//�����ж�����

#define nSEL_PORT       GPIO_B  //SPIƬѡ�˿�
#define nSEL_PIN        pin_12	//SPIƬѡ����

#define RF_SWITCH		0		//���ߵ�Դ���� 1:(�����е�Դ����), 0(����û�п���)
#if RF_SWITCH
#define RF_SWITCH_PORT	GPIO_B	//���ߵ�Դ���ض˿�
#define RF_SWITCH_PIN	pin_12	//���ߵ�Դ��������
#define RF_SWITCH_ON	STATUS1(RF_SWITCH_PORT, RF_SWITCH_PIN)	//���ߵ�Դ��
#define RF_SWITCH_OFF	STATUS0(RF_SWITCH_PORT, RF_SWITCH_PIN)	//���ߵ�Դ��
#endif

#define SI4432_CH		(0)	//����433�����ŵ�: 86 / 85 / 84 / 83 / 82 / 81 //������ʹ�����������ֵ
#if (SI4432_CH < 81 || SI4432_CH > 86)
	#undef SI4432_CH
#endif
#define SI4432_THRESHOLD 	(0)	//����433�����ŵ��ż���Threshold��: 60 �� 220. ��ע�� ��0 ʹ���豸�Զ��ż����ã�
#if (SI4432_THRESHOLD == 0)
	#undef SI4432_THRESHOLD 
#endif
//433�����ź�ָʾ�˿�*/
#define SIGNAL		0		//�Ƿ���Ҫ�ź�ָʾ���� 1: ��Ҫ�� 0������Ҫ
#define SIGNAL_PORT	GPIO_A
#define SIGNAL_PIN	pin_8
#define SIGNAL_ON	STATUS0(RUN_PORT, RUN_PIN)
#define SIGNAL_OFF	STATUS1(RUN_PORT, RUN_PIN)

//���嵱ǰflash�豸�Ĵ�С,0��ʾ64kflash�豸,1��ʾ128k flash�豸
#define   CURRENT_DEV_FLASH_SIZE	(1u)	

/***********************************��������***********************************/
void SysHard_Init(void);
u32 HardID_Check(u32 id);
#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
void HardID_Change(void);
#endif

#endif 		//SysHard.h end 
/**************************Copyright BestFu 2014-05-14*************************/
