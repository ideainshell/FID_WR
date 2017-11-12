#include "includes.h"
#define CPU_USE_DEBUG 1
#define FR_TIMEOUT 200
		LIST List;
 char show_time_str[25];
#ifdef CPU_USE_DEBUG
 CPU_STK_SIZE FM_U,FM_F;
 CPU_STK_SIZE SF_U,SF_F;
 CPU_STK_SIZE WR_U,WR_F;
 CPU_STK_SIZE LE_U,LE_F;
#endif
/* 
 * ��������Task_LED
 * ����  : LED��ˮ��	
 * ����  ����
 * ���  : ��
 */
void Task_LED(void *p_arg)
{
	OS_ERR err;
    (void)p_arg;                		// 'p_arg' ��û���õ�����ֹ��������ʾ����
	  OSTmrStart(&systime,&err);
    while (1)
    {
			Time_Str_Get(show_time_str);   //��ʾʱ��
			OLED_ShowString(12,18,show_time_str,12);
			OLED_Refresh_Gram();//������ʾ��OLED	
#ifdef CPU_USE_DEBUG
			OSTaskStkChk (&FmTaskTCB,    &FM_F, &FM_U,&err);
			OSTaskStkChk (&SfTaskTCB,    &SF_F, &SF_U,&err);
			OSTaskStkChk (&WriteTaskTCB, &WR_F, &WR_U,&err);
			OSTaskStkChk (&LedTaskTCB,   &LE_F, &LE_U,&err);
#endif
        LED1( ON );                  //ϵͳ��������ָʾ��
        OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
        LED1( OFF);
		    OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
    }
}

void Task_FM(void *p_arg)
{
  OS_ERR err;
	int temp;
  (void)p_arg;
	M_TIME p_time;
  List_Init(&List);
	while(1)
	{
		while(1)
		{
		 get_Image(); //��ȡͼ��
		 OSSemPend(&share_Usart,FR_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
			if(err==OS_ERR_TIMEOUT)
				break;
		 if(IS_OK(Revtemp)==1)
		 {
			 genchar_buff1();//��������
			 OSSemPend(&share_Usart,500,OS_OPT_PEND_BLOCKING,0,&err);
			 if(err==OS_ERR_TIMEOUT)
				break;
			if(IS_OK(Revtemp)==1)
			{
				 get_search();//ȷ���Ƿ�Ϊ����Աָ��
				 OSSemPend(&share_Usart,300,OS_OPT_PEND_BLOCKING,0,&err);
				 if(err==OS_ERR_TIMEOUT)
				   break;
				 temp=IS_OK(Revtemp);
				 if(temp==2)//�û�ָ�����Ž������������
				 {
					 LED3(ON);
					 JDQ(ON);
					 p_time=Time_Get();
					 ADD_Record(p_time,(Revtemp[10]<<8)|Revtemp[11],&List);
				 }
				 else if(temp==3)//����Աָ��~����¼ָ��ָʾ��
				 {
					 LED4(ON);
					 OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
					 OSSemPost(&Re_Finger,OS_OPT_PEND_BLOCKING,&err);
				 }
			}
		 }
		 		 OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);//����ָ��ɨ��֮��100������ 
					 LED3(OFF);
					 JDQ(OFF);//ȷ���Ž���ָʾ�ƶ��ر�
	 }
	}
}
void Task_SF(void *p_arg)
{
	OS_ERR err;
	int temp;
	int flag=0;
	(void)p_arg;
	while(1)
	{
		OSSemPend(&Re_Finger,0,OS_OPT_PEND_BLOCKING,0,&err);//�ȴ�����Աָ�Ƶ�¼
		OSTaskSuspend(&FmTaskTCB,&err);//����ָ��ɨ������
		while(1)
		{
			while(1)
		 {
			get_Image();
			OSSemPend(&share_Usart,100,OS_OPT_PEND_BLOCKING,0,&err);
			 if(err==OS_ERR_TIMEOUT)
				 break;
			if(IS_OK(Revtemp)==1)//���λ�ȡ�ɼ�ָ�ƣ���ȡ����ָ��
			{
				if(flag==0)//��Ҫ��������ͼ��
				  genchar_buff1();
				else if(flag==1)
				  genchar_buff2();	
			  OSSemPend(&share_Usart,100,OS_OPT_PEND_BLOCKING,0,&err);
				
			  if(IS_OK(Revtemp)==1)//ȷ������������������
			  {
				  get_search();//ȷ���Ƿ��ǹ���Աָ��
				  OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
				  temp=IS_OK(Revtemp);
				  if(temp==3)
				  {
					  LED4(OFF);
					  break;
				  }
				  else if(temp==4||temp==2)
			    {
					  flag++;
				   }	 
			   }
			 }
			OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
			 if(flag==2)
			{
				flag=0;
				get_regmodel();//����ģ��
				OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
				 if(IS_OK(Revtemp)==1)
			  {
					get_ModelNum();//��ȡģ������
					OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
					if(IS_OK(Revtemp)==5)
					{
						get_storemodel(((Revtemp[10]<<8)|Revtemp[11])-1);//�洢ģ��
						OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
						if(IS_OK(Revtemp)==1)
					  {					
						  OLED_ShowString(0,44,"YES",16);
			        OLED_Refresh_Gram();//������ʾ��OLED	
						  JDQ(ON);
						  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
						  JDQ(OFF);
					  }
					}
				}
			}
		OSTaskResume(&FmTaskTCB,&err);	//�ָ��Ž�����
		}
	 }
  }
}

FIL frd;
FRESULT res;
unsigned int br=1;

char show_date_str[10];
char show_day_str[13];

char filename[13];
void WRITE_LOG(void *p_arg)
{
		OS_ERR err;
	(void)p_arg;
	while(1)
	{
		OLED_ShowString(12,44,"              ",16);
	  OLED_Refresh_Gram();//������ʾ��OLED
		OSSemPend(&Write_SD,0,OS_OPT_PEND_BLOCKING,0,&err);
		LED4(ON);
		OLED_ShowString(12,44,"SDCard mounted",16);
	  OLED_Refresh_Gram();//������ʾ��OLED	
		OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
		while(List.count>0)
		{
	   T2Str(&(List.head->item.S_Time),filename);
		 strcat(filename,".csv");
	  res = f_open(&frd,filename,FA_CREATE_NEW | FA_WRITE );
	  if ( res == FR_OK )
 	  { 
		  D2Str(&(List.head->item.S_Time),&(List.head->item.ID),show_day_str);
		  res = f_write(&frd, show_day_str, strlen(show_day_str), &br); 
   		f_close(&frd);      
  	}
   else if ( res == FR_EXIST)
 	 {
		 D2Str(&(List.head->item.S_Time),&(List.head->item.ID),show_day_str);
		 res = f_open(&frd,filename, FA_WRITE| FA_OPEN_APPEND);
		 res = f_write(&frd, show_day_str, strlen(show_day_str), &br); 
   		f_close(&frd);
 	 }
	 Del_Item(&List);
  }
		LED4(OFF);
		OLED_ShowString(12,44,"  Log Writed  ",16);
	  OLED_Refresh_Gram();//������ʾ��OLED
	  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	
}
