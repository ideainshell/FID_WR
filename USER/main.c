#include "includes.h"
#define NEEDCaliTime 0
//任务优先级
#define START_TASK_PRIO		3
//任务堆栈大小
#define START_STK_SIZE 		512
//任务控制块
OS_TCB StartTaskTCB;
//任务堆栈
CPU_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void Start_Task(void *p_arg);

//任务优先级
#define FM_TASK_PRIO		4
//任务堆栈大小
#define FM_STK_SIZE 		512
//任务控制块
OS_TCB FmTaskTCB;
//任务堆栈
CPU_STK FM_TASK_STK[FM_STK_SIZE];
void Task_FN(void *p_arg);

//任务优先级
#define SF_TASK_PRIO		5
//任务堆栈大小
#define SF_STK_SIZE 		512
//任务控制块
OS_TCB SfTaskTCB;
//任务堆栈
CPU_STK SF_TASK_STK[SF_STK_SIZE];
void Task_SF(void *p_arg);

//任务优先级
#define WRITE_TASK_PRIO		6
//任务堆栈大小
#define WRITE_STK_SIZE 		512
//任务控制块
OS_TCB WriteTaskTCB;
//任务堆栈
CPU_STK WRITE_TASK_STK[WRITE_STK_SIZE];
void WRITE_LOG(void *p_arg);

//任务优先级
#define LED_TASK_PRIO		7
//任务堆栈大小
#define LED_STK_SIZE 		512
//任务控制块
OS_TCB LedTaskTCB;
//任务堆栈
CPU_STK LED_TASK_STK[LED_STK_SIZE];
void Task_LED(void *p_arg);



OS_SEM share_Usart;//串口单帧接收结束标志
OS_SEM Re_Finger;  //管理员指纹标志
OS_SEM Write_SD;   //SD插入标志

OS_MEM F_Log;
CPU_INT08U F_log_M[500][20];

OS_TMR systime;//周期检测内存卡
void systime_CB(OS_TMR *p_tmr, void *p_arg);

FATFS fs;

/* 
 * 函数名：main
 * 描述  : 主函数
 * 输入  ：无
 * 输出  : 无
 */
int main(void)
{			
		
	OS_ERR err;		
	/*创建任务*/		
	OSInit(&err);		//初始化UCOSIII
	//创建开始任务
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//任务控制块
				 (CPU_CHAR	* )"start task", 		//任务名字
				 (OS_TASK_PTR )Start_Task, 			//任务函数
				 (void		* )0,					//传递给任务函数的参数
				 (OS_PRIO	  )START_TASK_PRIO,     //任务优先级
				 (CPU_STK   * )&START_TASK_STK[0],	//任务堆栈基地址
				 (CPU_STK_SIZE)START_STK_SIZE / 10,	//任务堆栈深度限位
				 (CPU_STK_SIZE)START_STK_SIZE,		//任务堆栈大小
				 (OS_MSG_QTY  )0,					//任务内部消息队列能够接收的最大消息数目,为0时禁止接收消息
				 (OS_TICK	  )0,					//当使能时间片轮转时的时间片长度，为0时为默认长度，
				 (void   	* )0,					//用户补充的存储区
				 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //任务选项
				 (OS_ERR 	* )&err);				//存放该函数错误时的返回值
	OSStart(&err);  //开启UCOSIII
	while (1);						 
								 
  

}

void Start_Task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
  
	NVIC_Configuration();
	uart_init(57600); 
  LED_GPIO_Config();  /* LED 端口初始化 */
	
	if (NEEDCaliTime||BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    RTC_Configuration();
    Time_Adjust(13,48,00);
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
		BKP_WriteBackupRegister(BKP_DR2, 0x07E1); 
		BKP_WriteBackupRegister(BKP_DR3, 0x0B0C);
  }
  else
  {
    RTC_WaitForSynchro();
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    RTC_WaitForLastTask();
  }
  /* Clear reset flags */
  RCC_ClearFlag();
  
	
	Time_Init(BKP_ReadBackupRegister(BKP_DR2),BKP_ReadBackupRegister(BKP_DR3),RTC_GetCounter());
	
	CPU_Init();
	SysTick_init();
	
	OLED_Init();
	OLED_ShowString(0,0,"WUST-ROBOCON FID",16);  
	OLED_Refresh_Gram();//更新显示到OLED	

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);  	//统计任务
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//如果使能了测量中断关闭时间
	CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //当使用时间片轮转的时候
	//使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED, 1, &err);
#endif

    OSSemCreate(&share_Usart,
		             "share_usart",
					        1,
									&err);
		OSSemCreate(&Re_Finger,
		             "Re_finger",
					        1,
									&err);
		OSSemCreate(&Write_SD,
		             "mounted_SD",
					        1,
									&err);
		OSSemSet(&share_Usart,0,&err);
		OSSemSet(&Re_Finger,0,&err);
		OSSemSet(&Write_SD,0,&err);
		
		OSMemCreate ((OS_MEM      *)&F_Log,
                 (CPU_CHAR    *)"F_Log_Mem",
                 (void        *)F_log_M,
                 (OS_MEM_QTY   )500,
                 (OS_MEM_SIZE  )20,
                 (OS_ERR      *)&err);
		
		OSTmrCreate((OS_TMR   *)&systime,
		            (CPU_CHAR *)"time",
								(OS_TICK   )0,
								(OS_TICK   )1000,
								(OS_OPT    )OS_OPT_TMR_PERIODIC,
								(OS_TMR_CALLBACK_PTR)systime_CB,
								(void     *)0,
								(OS_ERR   *)&err);				
			
	  OS_CRITICAL_ENTER();	
		
		OSTaskCreate((OS_TCB     *)&LedTaskTCB,     //任务控制块指针          
                 (CPU_CHAR   *)"LED",		 //任务名称
                 (OS_TASK_PTR )Task_LED, 	 //任务代码指针
                 (void       *)0,			 //传递给任务的参数parg
                 (OS_PRIO     )LED_TASK_PRIO,			 //任务优先级
                 (CPU_STK    *)&LED_TASK_STK[0],	 //任务堆栈基地址
                 (CPU_STK_SIZE)LED_STK_SIZE/10,			 //堆栈剩余警戒线
                 (CPU_STK_SIZE)LED_STK_SIZE,			 //堆栈大小
                 (OS_MSG_QTY  )0,			 //可接收的最大消息队列数
                 (OS_TICK     )0,			 //时间片轮转时间
                 (void       *)0,			 //任务控制块扩展信息
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //任务选项
                 (OS_ERR     *)&err);		 //返回值
		
    OSTaskCreate((OS_TCB     *)&FmTaskTCB,     //任务控制块指针          
                 (CPU_CHAR   *)"fingerPrint",		 //任务名称
                 (OS_TASK_PTR )Task_FM, 	 //任务代码指针
                 (void       *)0,			 //传递给任务的参数parg
                 (OS_PRIO     )FM_TASK_PRIO,			 //任务优先级
                 (CPU_STK    *)&FM_TASK_STK[0],	 //任务堆栈基地址
                 (CPU_STK_SIZE)FM_STK_SIZE/10,			 //堆栈剩余警戒线
                 (CPU_STK_SIZE)FM_STK_SIZE,			 //堆栈大小
                 (OS_MSG_QTY  )0,			 //可接收的最大消息队列数
                 (OS_TICK     )0,			 //时间片轮转时间
                 (void       *)0,			 //任务控制块扩展信息
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //任务选项
                 (OS_ERR     *)&err);		 //返回值
								 
		  OSTaskCreate((OS_TCB     *)&SfTaskTCB,     //任务控制块指针          
                 (CPU_CHAR   *)"fingerPrint get",		 //任务名称
                 (OS_TASK_PTR )Task_SF, 	 //任务代码指针
                 (void       *)0,			 //传递给任务的参数parg
                 (OS_PRIO     )SF_TASK_PRIO,			 //任务优先级
                 (CPU_STK    *)&SF_TASK_STK[0],	 //任务堆栈基地址
                 (CPU_STK_SIZE)SF_STK_SIZE/10,			 //堆栈剩余警戒线
                 (CPU_STK_SIZE)SF_STK_SIZE,			 //堆栈大小
                 (OS_MSG_QTY  )0,			 //可接收的最大消息队列数
                 (OS_TICK     )0,			 //时间片轮转时间
                 (void       *)0,			 //任务控制块扩展信息
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //任务选项
                 (OS_ERR     *)&err);		 //返回值
								 
			 OSTaskCreate((OS_TCB     *)&WriteTaskTCB,     //任务控制块指针          
                 (CPU_CHAR   *)"write to SD",		 //任务名称
                 (OS_TASK_PTR )WRITE_LOG, 	 //任务代码指针
                 (void       *)0,			 //传递给任务的参数parg
                 (OS_PRIO     )WRITE_TASK_PRIO,			 //任务优先级
                 (CPU_STK    *)&WRITE_TASK_STK[0],	 //任务堆栈基地址
                 (CPU_STK_SIZE)WRITE_STK_SIZE/10,			 //堆栈剩余警戒线
                 (CPU_STK_SIZE)WRITE_STK_SIZE,			 //堆栈大小
                 (OS_MSG_QTY  )0,			 //可接收的最大消息队列数
                 (OS_TICK     )0,			 //时间片轮转时间
                 (void       *)0,			 //任务控制块扩展信息
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //任务选项
                 (OS_ERR     *)&err);		 //返回值
								 
    OS_CRITICAL_EXIT();			 
		OSTaskDel(&StartTaskTCB,&err);
}

void systime_CB(OS_TMR *p_tmr, void *p_arg)
{
	OS_ERR err;
	FRESULT res;
	CPU_SR_ALLOC();
	OS_CRITICAL_ENTER();	
	res=f_mount(&fs,"",1);
	 if(res==FR_OK)
	{
	 OSSemPost(&Write_SD,OS_OPT_PEND_BLOCKING,&err);
  }
	OS_CRITICAL_EXIT();
}
