/***************************Copyright BestFu 2014-05-14*************************
�ļ�:	Polling.h
˵��:	RS485��ѯ
����:	Keil uVision4 V4.72.0.0
�汾:	v1.0
��д:	Unarty
����:	2015.09.07
�޸�:	����
*******************************************************************************/
#ifndef __POLLING_H
#define __POLLING_H	

#include "BF_type.h"
#include "SysTick.h"
#include "AirUnit.h"
/******************************************************************************/
#define	SEND_DELAY							300		//����һ�������ٷ���һ�������ʱ�䣬ms
#define	POLL_ADDR_LEN						20
#define	SEND_DATA_LEN						100
#define	RECV_DATA_LEN						100
#define SEND_PROTOCOL_DATA_LEN 	80
#define ORDER_NUM							 	13
#define	abst(a,b)								((a)>(b)?((a)-(b)):((a)+(TIME_MAX-(b))))

/******************************************************************************/
typedef struct
{
	u8 Air_type;								//��幤������ 0:485���� 1:�¿����������Э�鹤�� 2:������������ֱ�ӿ��ƿյ�
	u8 Local_Addr;							//��屾��ĵ�ַ
	u8 Air_Indoor_Addr;					//���ڻ���ַ
	u8 Air_Indoor_Addr_Len;			//���ڻ���ַ��С
	u8 Poll_Addr[POLL_ADDR_LEN];//�����Ϊ����ʱ����ѯ��ַ�б�
}Air_Para_t;


typedef struct
{
	u8 En;						//ʹ�ܣ���Э���Ƿ����ⲿ�ֵ�����
	u8 Lenth;					//���ݳ��ȣ����ⲿ�����ݵĳ���
	u8 CMD_data[4];		//�ⲿ�ֵ����ݣ������ݵĹ̶����֣�������Щ���ݱ�־���仯�����ݲ������
}CMD_t;
typedef struct
{
	u8 En;						//ʹ�ܣ���Э���Ƿ����ⲿ�ֵ�����
	u8 Lenth;					//���ݳ��ȣ����ⲿ�����ݵĳ���
	u8 CMD_data[10];	//�ⲿ�ֵ����ݣ������ݵĹ̶����֣�������Щ���ݱ�־���仯�����ݲ������
}CMD_Mode_t;
typedef struct
{
	u8 En;						//ʹ�ܣ���Э���Ƿ����ⲿ�ֵ�����
	u8 Lenth;					//���ݳ���								
}CMD_DATA_t;
typedef struct
{
	CMD_t Soi;													//֡ͷ��־
	CMD_t Addr;													//�յ���ַ
	CMD_t Cmd;													//���ܴ���
	CMD_t Addr_sta;											//���ڻ���ʼ��ַ
	CMD_t Addr_len;											//���ڻ��Ĵ�������
	CMD_t Fram_len;											//���ݴ�С
	CMD_t On_Off_Data;									//���ػ�����
	CMD_Mode_t Mode_Data;								//�յ�ģʽ����
	CMD_DATA_t Temperatue_Data;					//�¶�����
	CMD_Mode_t Wind_Speed_Data;					//��������
	CMD_t Wind_Data;										//ɨ������
	CMD_DATA_t Check;										//У��
	CMD_t Eoi;													//������־
	u8 order[ORDER_NUM];
}CMD_fram_t;
typedef struct
{
	u8 En;															//���ռ��ʹ��
	CMD_t Soi;													//֡ͷ��־
	CMD_t Addr;													//�յ���ַ
	CMD_t Cmd;													//���ܴ���
	CMD_t Addr_sta;											//���ڻ���ʼ��ַ
	CMD_t Addr_len;											//���ڻ��Ĵ�������
	CMD_t Fram_len;											//���ݴ�С
	CMD_t On_Off_Data;									//���ػ�����
	CMD_Mode_t Mode_Data;								//�յ�ģʽ����
	CMD_DATA_t Temperatue_Data;					//�¶�����
	CMD_Mode_t Wind_Speed_Data;					//��������
	CMD_t Wind_Data;										//ɨ������
	CMD_DATA_t Check;										//У��
	CMD_t Eoi;													//������־
	u8 order[ORDER_NUM];
}Recv_CMD_fram_t;
typedef struct
{
	u8 name[16];			//Ʒ����
	u8 type[16];			//�ͺ�
	u8 Check_Mode;		//У��ģʽ	0��CRC16	1���ۼӺ�
	u8 CRC_Mode;			//CRC��������������	0��С��		1�����
	u8 Run_CMD[5];		//���ػ���ģʽ���¶ȣ����٣�ɨ��ִ����һ�������5��
}RS485Explain_t;
typedef struct
{
	RS485Explain_t device_set;				//485Э�����Ϣ
	CMD_fram_t first_cmd;							//��һ���������
	CMD_fram_t second_cmd;						//�ڶ����������					
	CMD_fram_t third_cmd;							//�������������
	CMD_fram_t forth_cmd;							//�������������
	CMD_fram_t fifth_cmd;							//�������������
	Recv_CMD_fram_t Recv_first_cmd;		//��һ�������������
	Recv_CMD_fram_t Recv_second_cmd;	//�ڶ��������������					
	Recv_CMD_fram_t Recv_third_cmd;		//�����������������
	Recv_CMD_fram_t Recv_forth_cmd;		//�����������������
	Recv_CMD_fram_t Recv_fifth_cmd;		//�����������������
}airlib_t;

typedef struct
{
	u8 Addr;													//�յ���ַ
	u8 Cmd;														//���ܴ���
	u8 len;														//���ݴ�С
	u8 data;													//����
	u16 crc;													//CRCУ��
}Poll_t;

typedef enum
{
	RS485_POLL_ASK = 0x01,				//��ѯѯ���Ƿ������ݷ���
	RS485_Data_Ready = 0x02,			//������׼���ã����Է���
	RS485_Data_Nothing = 0x03,		//�����ݿ��Է���
	RS485_Send_Allow = 0x04,			//����������
	RS485_Send_Finish = 0x05,			//���ݷ������
	RS485_RESERVE = 0xFF,					//����
}CommMeaning_t;									//���������ͨ��Э������

typedef enum
{
	Air_RS485_Gateway = 0x00,			//RS485����
	Air_Slave = 0x01,							//�¿���壬������һ��Э������
	Air_Control = 0x02,						//�¿���壬����������ֱ�ӿ��ƿյ�
	Air_RESERVE = 0xFF,						//����
}Air_type_t;	
/******************************************************************************/
extern volatile u8 Device_type;
extern airlib_t air_config;
extern CMD_fram_t send_data_cmd;
extern u8 Send_Protocol_Data[5][SEND_PROTOCOL_DATA_LEN];
extern u8 Send_Protocol_Cnt[5];
extern u8 Send_Communication_Data[SEND_DATA_LEN];
extern u8 Recv_Communication_Data[RECV_DATA_LEN];
extern u8 list_order;
extern volatile u8 Send_Data_Flag;//�������ݷ��ͱ�־
extern AirStatus_t gAirStatus_bak;
extern Air_Para_t gAirConfigPara;
extern volatile u8 Recv_Finish_Flag;
extern u8 Air_Indoor_Addr;
extern volatile u32 UPST;
extern volatile u32 Send_Data_UPST;
extern volatile u32 GW_Recv_Data_UPST;
/******************************************************************************/
extern void RS485_Gateway(void);
extern void Device_Polling(void);
extern void Comm_gateway(void);
extern void Send_Protocol_CMD(CMD_fram_t cmd, u8 num);
extern void Send_Device_CMD(u8 addr,u8 cmd,u8 len,u8* data);
extern void Send_Data_To_RS485(u8 *data, u8 len);
extern void MS_Device_Polling(void);
extern void Recv_Device_Ack(u8* data);
extern void Device_Polling_Send(void);
extern void _SetTimeing_ms(u32 *T,unsigned int time);
extern int _CheckTime(u32 *T);
extern u8 Check_Recv_Data(Poll_t data);
extern void Config_Data_Init(void);
extern void Clr_Memery(u8 *data, u8 cnt);
extern u8 Next_addr(u8 order);
extern void Select_CMD(u8 num);
extern void AIR_LibNameGet(airlib_t const *explain, u8 *len, u8 *data);
#endif

/**************************Copyright BestFu 2015-09-07*************************/
