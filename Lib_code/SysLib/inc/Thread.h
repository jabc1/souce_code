/***************************Copyright BestFu 2014-05-14*************************
文	件：	Thread.h
说	明：	线程管理头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.18  
修　改：	暂无
*******************************************************************************/
#ifndef __THREAD_H
#define __THREAD_H

#include "BF_type.h"


#define THREAD_NUM      20      //最多线程总数 

typedef struct
{
    u8 flag;    //运行标识
    u8 con;     //运行条件
    u16 cnt;     //运行次数
    u32 frep;   //运行周期
	u32 countTime ;//线程执行的等待时间 yanhuan adding 2015/10/27
    void *fun;  //运行函数指针
}Thread_t;

typedef struct
{
	u16	runCnt;		//当前线程运行数
	u16 hangCnt;	//线程挂起数
	
    Thread_t thread[THREAD_NUM];
}ThreadManage_t;

typedef enum
{
    FINISH  = 0,     //完成
    REPEAT  = 1,     //线程已注册
    FULL    = 2,     //线程注册空间已满
}ThreadSta_t;       //线程注册结果

typedef enum
{
    ONCEDELAY	= 0,    //单次延时运行
    MANY		= 1,   	//多次
    FOREVER     = 2,    //永久
    RESULT      = 3,    //按结果
}ThreadCon_t;     //线程驻存条件
 
typedef enum
{
	NO_EXIST_STATE = 0,			//不在任务链表中 	
	RUNNING_STATE,   			//运行态 	
	PENDING_STATE				//挂起态 
}ThreadState_t;

typedef void (*fun1)(void); 
typedef u8 (*fun2)(void);
    
void Thread_Init(void);     //线程初始化函数
u32 Thread_Login(ThreadCon_t con, u16 cnt, u32 frep, void * fun);   //线程注册
void Thread_Logout(void *fun);  //线程注销
void Thread_Process(void);      //线程运行
void Thread_RunCheck(void); //线程运行时间核对
void Thread_Hang(void * fun);   //线程挂起
void Thread_Wake(void * fun);   //线程换醒    
ThreadState_t Thread_Query(void *fun);//查询线程当前状态   //Jay Add 2015.11.17

#endif //Thread.h end
/**************************Copyright BestFu 2014-05-14*************************/
