#include "includes.h"
#define NEEDCaliTime 0
//�������ȼ�
#define START_TASK_PRIO		3
//�����ջ��С
#define START_STK_SIZE 		512
//������ƿ�
OS_TCB StartTaskTCB;
//�����ջ
CPU_STK START_TASK_STK[START_STK_SIZE];
//������
void Start_Task(void *p_arg);

//�������ȼ�
#define FM_TASK_PRIO		4
//�����ջ��С
#define FM_STK_SIZE 		512
//������ƿ�
OS_TCB FmTaskTCB;
//�����ջ
CPU_STK FM_TASK_STK[FM_STK_SIZE];
void Task_FN(void *p_arg);

//�������ȼ�
#define SF_TASK_PRIO		5
//�����ջ��С
#define SF_STK_SIZE 		512
//������ƿ�
OS_TCB SfTaskTCB;
//�����ջ
CPU_STK SF_TASK_STK[SF_STK_SIZE];
void Task_SF(void *p_arg);

//�������ȼ�
#define WRITE_TASK_PRIO		6
//�����ջ��С
#define WRITE_STK_SIZE 		512
//������ƿ�
OS_TCB WriteTaskTCB;
//�����ջ
CPU_STK WRITE_TASK_STK[WRITE_STK_SIZE];
void WRITE_LOG(void *p_arg);

//�������ȼ�
#define LED_TASK_PRIO		7
//�����ջ��С
#define LED_STK_SIZE 		512
//������ƿ�
OS_TCB LedTaskTCB;
//�����ջ
CPU_STK LED_TASK_STK[LED_STK_SIZE];
void Task_LED(void *p_arg);



OS_SEM share_Usart;//���ڵ�֡���ս�����־
OS_SEM Re_Finger;  //����Աָ�Ʊ�־
OS_SEM Write_SD;   //SD�����־

OS_MEM F_Log;
CPU_INT08U F_log_M[500][20];

OS_TMR systime;//���ڼ���ڴ濨
void systime_CB(OS_TMR *p_tmr, void *p_arg);

FATFS fs;

/* 
 * ��������main
 * ����  : ������
 * ����  ����
 * ���  : ��
 */
int main(void)
{			
		
	OS_ERR err;		
	/*��������*/		
	OSInit(&err);		//��ʼ��UCOSIII
	//������ʼ����
	OSTaskCreate((OS_TCB 	* )&StartTaskTCB,		//������ƿ�
				 (CPU_CHAR	* )"start task", 		//��������
				 (OS_TASK_PTR )Start_Task, 			//������
				 (void		* )0,					//���ݸ��������Ĳ���
				 (OS_PRIO	  )START_TASK_PRIO,     //�������ȼ�
				 (CPU_STK   * )&START_TASK_STK[0],	//�����ջ����ַ
				 (CPU_STK_SIZE)START_STK_SIZE / 10,	//�����ջ�����λ
				 (CPU_STK_SIZE)START_STK_SIZE,		//�����ջ��С
				 (OS_MSG_QTY  )0,					//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ,Ϊ0ʱ��ֹ������Ϣ
				 (OS_TICK	  )0,					//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
				 (void   	* )0,					//�û�����Ĵ洢��
				 (OS_OPT      )OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR, //����ѡ��
				 (OS_ERR 	* )&err);				//��Ÿú�������ʱ�ķ���ֵ
	OSStart(&err);  //����UCOSIII
	while (1);						 
								 
  

}

void Start_Task(void *p_arg)
{
	OS_ERR err;
	CPU_SR_ALLOC();
	p_arg = p_arg;
  
	NVIC_Configuration();
	uart_init(57600); 
  LED_GPIO_Config();  /* LED �˿ڳ�ʼ�� */
	
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
	OLED_Refresh_Gram();//������ʾ��OLED	

#if OS_CFG_STAT_TASK_EN > 0u
	OSStatTaskCPUUsageInit(&err);  	//ͳ������
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN		//���ʹ���˲����жϹر�ʱ��
	CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  //��ʹ��ʱ��Ƭ��ת��ʱ��
	//ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
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
		
		OSTaskCreate((OS_TCB     *)&LedTaskTCB,     //������ƿ�ָ��          
                 (CPU_CHAR   *)"LED",		 //��������
                 (OS_TASK_PTR )Task_LED, 	 //�������ָ��
                 (void       *)0,			 //���ݸ�����Ĳ���parg
                 (OS_PRIO     )LED_TASK_PRIO,			 //�������ȼ�
                 (CPU_STK    *)&LED_TASK_STK[0],	 //�����ջ����ַ
                 (CPU_STK_SIZE)LED_STK_SIZE/10,			 //��ջʣ�ྯ����
                 (CPU_STK_SIZE)LED_STK_SIZE,			 //��ջ��С
                 (OS_MSG_QTY  )0,			 //�ɽ��յ������Ϣ������
                 (OS_TICK     )0,			 //ʱ��Ƭ��תʱ��
                 (void       *)0,			 //������ƿ���չ��Ϣ
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //����ѡ��
                 (OS_ERR     *)&err);		 //����ֵ
		
    OSTaskCreate((OS_TCB     *)&FmTaskTCB,     //������ƿ�ָ��          
                 (CPU_CHAR   *)"fingerPrint",		 //��������
                 (OS_TASK_PTR )Task_FM, 	 //�������ָ��
                 (void       *)0,			 //���ݸ�����Ĳ���parg
                 (OS_PRIO     )FM_TASK_PRIO,			 //�������ȼ�
                 (CPU_STK    *)&FM_TASK_STK[0],	 //�����ջ����ַ
                 (CPU_STK_SIZE)FM_STK_SIZE/10,			 //��ջʣ�ྯ����
                 (CPU_STK_SIZE)FM_STK_SIZE,			 //��ջ��С
                 (OS_MSG_QTY  )0,			 //�ɽ��յ������Ϣ������
                 (OS_TICK     )0,			 //ʱ��Ƭ��תʱ��
                 (void       *)0,			 //������ƿ���չ��Ϣ
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //����ѡ��
                 (OS_ERR     *)&err);		 //����ֵ
								 
		  OSTaskCreate((OS_TCB     *)&SfTaskTCB,     //������ƿ�ָ��          
                 (CPU_CHAR   *)"fingerPrint get",		 //��������
                 (OS_TASK_PTR )Task_SF, 	 //�������ָ��
                 (void       *)0,			 //���ݸ�����Ĳ���parg
                 (OS_PRIO     )SF_TASK_PRIO,			 //�������ȼ�
                 (CPU_STK    *)&SF_TASK_STK[0],	 //�����ջ����ַ
                 (CPU_STK_SIZE)SF_STK_SIZE/10,			 //��ջʣ�ྯ����
                 (CPU_STK_SIZE)SF_STK_SIZE,			 //��ջ��С
                 (OS_MSG_QTY  )0,			 //�ɽ��յ������Ϣ������
                 (OS_TICK     )0,			 //ʱ��Ƭ��תʱ��
                 (void       *)0,			 //������ƿ���չ��Ϣ
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //����ѡ��
                 (OS_ERR     *)&err);		 //����ֵ
								 
			 OSTaskCreate((OS_TCB     *)&WriteTaskTCB,     //������ƿ�ָ��          
                 (CPU_CHAR   *)"write to SD",		 //��������
                 (OS_TASK_PTR )WRITE_LOG, 	 //�������ָ��
                 (void       *)0,			 //���ݸ�����Ĳ���parg
                 (OS_PRIO     )WRITE_TASK_PRIO,			 //�������ȼ�
                 (CPU_STK    *)&WRITE_TASK_STK[0],	 //�����ջ����ַ
                 (CPU_STK_SIZE)WRITE_STK_SIZE/10,			 //��ջʣ�ྯ����
                 (CPU_STK_SIZE)WRITE_STK_SIZE,			 //��ջ��С
                 (OS_MSG_QTY  )0,			 //�ɽ��յ������Ϣ������
                 (OS_TICK     )0,			 //ʱ��Ƭ��תʱ��
                 (void       *)0,			 //������ƿ���չ��Ϣ
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),	 //����ѡ��
                 (OS_ERR     *)&err);		 //����ֵ
								 
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
