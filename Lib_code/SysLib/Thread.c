/***************************Copyright BestFu 2014-05-14*************************
文	件：	Thread.c
说	明：	线程实现函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.18  
修　改：	2014.08.30 //修改单次延时注册线程，延时零毫秒不能执行问题
*******************************************************************************/
#include "BestFuLib.h"
#include "Thread.h"
#include "SysTick.h"


#define THREAD_TOTAL    (thread.runCnt + thread.hangCnt)    //线程总数

static ThreadManage_t   thread;

static u8 Thread_Check(u32 start, u32 end, void *fun);

/*******************************************************************************
  函数名:  Thread_Init
功能说明:  线程数据初始化
    参数:  无
  返回值:  无
*******************************************************************************/
void Thread_Init(void)
{
    *(u32*)&thread.runCnt = 0;   //线程总数置空
}

/*******************************************************************************
  函数名:  Thread_Login
功能说明:  线程注册 
    参数:  fun:     线程名
           执行描述
  返回值:  注册结果, 
*******************************************************************************/
u32 Thread_Login(ThreadCon_t con, u16 cnt, u32 frep, void * fun)
{
	u32 i;
	
    ERRR(THREAD_TOTAL >= THREAD_NUM, return 0);     //线程空间
	if (Thread_Check(0, THREAD_TOTAL, fun) == 0)		//线程没有出现在线程表中
	{
		memmove(&thread.thread[thread.runCnt+1], &thread.thread[thread.runCnt],     //空闲一个空间，给新建线程
                    sizeof(Thread_t)*(thread.hangCnt));   
		i = thread.runCnt;
		thread.runCnt++;
		thread.thread[i].flag = 0;
	}	
	else 
	{
		Thread_Wake(fun);	//线程换醒
		i = Thread_Check(0, THREAD_TOTAL, fun) - 1;
	}	
   
    thread.thread[i].con = con;
    thread.thread[i].fun = fun;
    if (con == ONCEDELAY)   //单次延时线程
    {
			if (0 == frep)
			{
				frep++;
			}
			cnt = 1;
			thread.thread[i].flag = 0;
    }
	thread.thread[i].cnt = cnt;
	thread.thread[i].frep = frep;
	thread.thread[i].countTime = 0; //yanhuan adding 2015/10/28
    
    return 1;
}

/*******************************************************************************
  函数名:  Thread_Logout
功能说明:  线程注销 
    参数:  fun:     线程名
  返回值:  无
*******************************************************************************/
void Thread_Logout(void * fun)
{
    u32 i = Thread_Check(0, THREAD_TOTAL, fun);

    if (i)    //找到线程位置
    {
        memmove(&thread.thread[i-1], &thread.thread[i], 
                    sizeof(Thread_t)*(THREAD_TOTAL - i));
        
        if (i > thread.runCnt)  //线程位于运行空间
        {
			thread.hangCnt--;
        }
        else
        {
            thread.runCnt--; 
        }
    }
}

/*******************************************************************************
  函数名:  Thread_Hang
功能说明:  线程挂起 
    参数:  fun:     线程名
  返回值:  无
*******************************************************************************/
void Thread_Hang(void * fun)
{
    u32 i = Thread_Check(0, thread.runCnt, fun);

    if (i)    //线程位于运行线程空间
    {
        Thread_t th;
        
        memcpy(&th, &thread.thread[i-1], sizeof(Thread_t)); 
        memmove(&thread.thread[i-1], &thread.thread[i], 
                    sizeof(Thread_t)*(thread.runCnt - i));
        memcpy(&thread.thread[--thread.runCnt], &th, sizeof(Thread_t));
        thread.hangCnt++;
    }
}

/*******************************************************************************
  函数名:  Thread_Wake
功能说明:  线程换醒
    参数:  fun:     线程名
  返回值:  无
注    意:  本功能只能对已有线程的操作 
*******************************************************************************/
void Thread_Wake(void * fun)
{
    u32 i = Thread_Check(thread.runCnt, THREAD_TOTAL, fun);

    if (i)    //找到线程位置
    {
        if ((i - thread.runCnt) > 1)    //i不是紧靠运行线程
        {
            Thread_t th;
            memcpy(&th, &thread.thread[i-1], sizeof(Thread_t)); 
            memmove(&thread.thread[thread.runCnt + 1], &thread.thread[thread.runCnt], //腾出存储空间
                        sizeof(Thread_t)*(i - thread.runCnt));
            memcpy(&thread.thread[thread.runCnt], &th,  sizeof(Thread_t));
        }
        thread.runCnt++;
        thread.hangCnt--;
    }
}

/*******************************************************************************
  函数名:  Thread_Process
功能说明:  线程处理
    参数:  无
  返回值:  无
*******************************************************************************/
void Thread_Process(void)
{
    Thread_t *pThread;

    for (pThread = &thread.thread[0]; pThread < &thread.thread[thread.runCnt]; pThread++)
    {
        if (pThread->flag)
        {
			void *fun = pThread->fun;;
			
            pThread->flag = 0;
            switch (pThread->con)
            {
                case ONCEDELAY:	pThread->flag = 1;
                case MANY:      if (!pThread->cnt--)
								{	
                                    Thread_Logout(fun);
									break;
                                }
                     
				case FOREVER:   ((fun1)(fun))();
                     break;				
                case RESULT:  	if (((fun2)(fun))() == 0)  //线程运行返回失败
								{
									Thread_Logout(fun);
								}
                    break;
                default : break;
            }
        }
    }
}
    
/*******************************************************************************
  函数名:  Thread_RunCheck
功能说明:  线程运行条件核对
    参数:  无
  返回值:  无
*******************************************************************************/
void Thread_RunCheck(void)
{
    Thread_t *pThread;

    for (pThread = &thread.thread[0]; pThread < &thread.thread[thread.runCnt]; pThread++)
    {
        if ((++pThread->countTime)>=(pThread->frep))//yanhuan modify 2015/10/28
        {
          pThread->countTime = 0;  
					pThread->flag = 1;
        }
    }
}

/*******************************************************************************
  函数名:   Thread_Check
功能说明:   线程核对
    参数:   fun:     线程名
            start:  起始线程位置
            end:    终止线程位置
  返回值:   0(没有)/(1~THREAD_UNM)线程所处位置
*******************************************************************************/
u8 Thread_Check(u32 start, u32 end, void *fun)
{
    u32 i; 
    
    for (i = start; i < end;)
    {
        if (thread.thread[i++].fun == fun)
        {
            return i;
        }
    }
    
    return 0;
}

/*******************************************************************************
函 数 名:  ThreadState_t Thread_Query(void *fun)
功能说明:  线程查询函数
参    数:  fun：线程函数指针
返 回 值:  NO_EXIST_STATE:不在任务链表中;RUNNING_STATE：在运行链表中;PENDING_STATE：在挂起链表中
*******************************************************************************/
ThreadState_t Thread_Query(void *fun)    //Jay Add 2015.11.17
{
	u8 Taskpos = Thread_Check(0, THREAD_TOTAL, fun);
	if(0 == Taskpos)
	{
		return NO_EXIST_STATE;
	}
	else if(Taskpos < thread.runCnt + 2)
	{
		return RUNNING_STATE;
	}
	else
	{
		return PENDING_STATE;
	}
}

/**************************Copyright BestFu 2014-05-14*************************/

