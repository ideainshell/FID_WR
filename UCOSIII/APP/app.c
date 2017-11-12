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
 * 函数名：Task_LED
 * 描述  : LED流水灯	
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED(void *p_arg)
{
	OS_ERR err;
    (void)p_arg;                		// 'p_arg' 并没有用到，防止编译器提示警告
	  OSTmrStart(&systime,&err);
    while (1)
    {
			Time_Str_Get(show_time_str);   //显示时间
			OLED_ShowString(12,18,show_time_str,12);
			OLED_Refresh_Gram();//更新显示到OLED	
#ifdef CPU_USE_DEBUG
			OSTaskStkChk (&FmTaskTCB,    &FM_F, &FM_U,&err);
			OSTaskStkChk (&SfTaskTCB,    &SF_F, &SF_U,&err);
			OSTaskStkChk (&WriteTaskTCB, &WR_F, &WR_U,&err);
			OSTaskStkChk (&LedTaskTCB,   &LE_F, &LE_U,&err);
#endif
        LED1( ON );                  //系统正常运行指示灯
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
		 get_Image(); //获取图像
		 OSSemPend(&share_Usart,FR_TIMEOUT,OS_OPT_PEND_BLOCKING,0,&err);
			if(err==OS_ERR_TIMEOUT)
				break;
		 if(IS_OK(Revtemp)==1)
		 {
			 genchar_buff1();//生成特征
			 OSSemPend(&share_Usart,500,OS_OPT_PEND_BLOCKING,0,&err);
			 if(err==OS_ERR_TIMEOUT)
				break;
			if(IS_OK(Revtemp)==1)
			{
				 get_search();//确定是否为管理员指纹
				 OSSemPend(&share_Usart,300,OS_OPT_PEND_BLOCKING,0,&err);
				 if(err==OS_ERR_TIMEOUT)
				   break;
				 temp=IS_OK(Revtemp);
				 if(temp==2)//用户指纹则开门禁，并加入队列
				 {
					 LED3(ON);
					 JDQ(ON);
					 p_time=Time_Get();
					 ADD_Record(p_time,(Revtemp[10]<<8)|Revtemp[11],&List);
				 }
				 else if(temp==3)//管理员指纹~开启录指纹指示灯
				 {
					 LED4(ON);
					 OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
					 OSSemPost(&Re_Finger,OS_OPT_PEND_BLOCKING,&err);
				 }
			}
		 }
		 		 OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err);//两次指纹扫描之间100毫秒间隔 
					 LED3(OFF);
					 JDQ(OFF);//确保门禁和指示灯都关闭
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
		OSSemPend(&Re_Finger,0,OS_OPT_PEND_BLOCKING,0,&err);//等待管理员指纹登录
		OSTaskSuspend(&FmTaskTCB,&err);//挂起指纹扫描任务
		while(1)
		{
			while(1)
		 {
			get_Image();
			OSSemPend(&share_Usart,100,OS_OPT_PEND_BLOCKING,0,&err);
			 if(err==OS_ERR_TIMEOUT)
				 break;
			if(IS_OK(Revtemp)==1)//两次获取采集指纹，获取正常指纹
			{
				if(flag==0)//需要两个特征图像
				  genchar_buff1();
				else if(flag==1)
				  genchar_buff2();	
			  OSSemPend(&share_Usart,100,OS_OPT_PEND_BLOCKING,0,&err);
				
			  if(IS_OK(Revtemp)==1)//确保两次特征生成正常
			  {
				  get_search();//确认是否是管理员指纹
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
				get_regmodel();//生成模板
				OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
				 if(IS_OK(Revtemp)==1)
			  {
					get_ModelNum();//获取模板数量
					OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
					if(IS_OK(Revtemp)==5)
					{
						get_storemodel(((Revtemp[10]<<8)|Revtemp[11])-1);//存储模板
						OSSemPend(&share_Usart,0,OS_OPT_PEND_BLOCKING,0,&err);
						if(IS_OK(Revtemp)==1)
					  {					
						  OLED_ShowString(0,44,"YES",16);
			        OLED_Refresh_Gram();//更新显示到OLED	
						  JDQ(ON);
						  OSTimeDlyHMSM(0,0,0,500,OS_OPT_TIME_HMSM_STRICT,&err);
						  JDQ(OFF);
					  }
					}
				}
			}
		OSTaskResume(&FmTaskTCB,&err);	//恢复门禁任务
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
	  OLED_Refresh_Gram();//更新显示到OLED
		OSSemPend(&Write_SD,0,OS_OPT_PEND_BLOCKING,0,&err);
		LED4(ON);
		OLED_ShowString(12,44,"SDCard mounted",16);
	  OLED_Refresh_Gram();//更新显示到OLED	
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
	  OLED_Refresh_Gram();//更新显示到OLED
	  OSTimeDlyHMSM(0,0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);
	}
	
}
