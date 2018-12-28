/***************************Copyright BestFu 2014-05-14*************************
文	件：	FaultManage.c
说	明：	异常管理代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.30  
修  改:     暂无
*******************************************************************************/
#include "FaultManage.h"
#include "Thread.h"
#include "Instruct.h"
#include "UserData.h"
#include "Wdg.h"
#include "Upload.h"


/*******************************************************************
函 数 名：	Fault_Upload
功能说明： 	异常上报函数
参	  数： 	grade:	异常等级
			event:	异常事件
			fun:	处理函数
返 回 值：	无
*******************************************************************/
void Fault_Upload(FaultGrade_t grade, Fault_t event, void *fun)
{
	switch (grade)
	{
		case FAULT_0: 	//最高级别错误
						Upload_Fault(EEPROM_FAIL);
						Thread_Logout(&Instruct_Run);	//注销运行指示灯
						break;	//返回跳出
		case FAULT_1:	//通过重置可以恢复
						WDG_FeedOFF();		//不在喂狗，设备自动复位
						break;
		case FAULT_2:	//通过注册函数重置恢复
						if (fun != NULL)
						{	
							Thread_Login(ONCEDELAY, 0, 5, fun);
						}
						break;
		default : 		break;
	}	
	
	if (event != EEPROM_W_ERR && event != EEPROM_R_ERR)
	{
		FaultData_Save(event);
	}
}

/**************************Copyright BestFu 2014-05-14*************************/
