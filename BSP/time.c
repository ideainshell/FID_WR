#include "includes.h"
#define WORKTIME 3
M_TIME this_time;
M_DAY  work_day[WORKTIME*2];
const int month_day1[12]={31,29,31,30,31,30,31,31,30,31,30,31};
const int month_day2[12]={31,28,31,30,31,30,31,31,30,31,30,31};
//判断是否为闰年，因为这里设计原因，年是最大255，所以不考虑400年以上的情况
uint8_t Is_Leap(unsigned char year)
{
	if((year%4==0&&year%100!=0)||year%400==0)
			return 1;
	else 
		return 0;
}
void Time_Init(uint16_t year,uint16_t MaD,uint32_t day)
{
	uint32_t i=0;
	this_time.year=year;
	
	this_time.month=MaD>>8;
	
	this_time.day=MaD&0xFF;
	
	for(i=day/86400;i>0;i--)
	  Time_add_Day();
	
	day=day%86400;
	
	this_time.hour=day/3600;
	
	this_time.min=day%3600/60;
	
	this_time.sec=day%3600%60;
	
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
	PWR_BackupAccessCmd(ENABLE);   

  RTC_WaitForLastTask();  
	RTC_SetCounter(day);		
  RTC_WaitForLastTask();
	
	BKP_WriteBackupRegister(BKP_DR2, this_time.year); 
	BKP_WriteBackupRegister(BKP_DR3, this_time.month<<8|this_time.day);
 }
 
//加一天
void Time_add_Day()
{
		this_time.day++;
		if(Is_Leap(this_time.year))//根据年  闰年 判断天
		{
			if(this_time.day>month_day1[this_time.month-1])
			{
				this_time.day=1;
				this_time.month++;
			}
		}
		else
		{
			if(this_time.day>month_day2[this_time.month-1])
			{
				this_time.day=1;
				this_time.month++;
			}
		}
		if(this_time.month>12)
		{
			this_time.month=1;
			this_time.year++;
		}
	 BKP_WriteBackupRegister(BKP_DR2, this_time.year); 
	 BKP_WriteBackupRegister(BKP_DR3, this_time.month<<8|this_time.day);
}
//获取时间
M_TIME Time_Get(void)
{
	uint32_t temp=RTC_GetCounter();
	this_time.hour=temp/3600;
	if(this_time.hour==24)
	{
		this_time.hour =0;
	}
	this_time.min =temp%3600/60;
	this_time.sec =temp%3600%60;
	return this_time;
}
void Time_Str_Get( char *Stime)
{
	Time_Get();
	Stime[0]=this_time.year%100/10+'0';
	Stime[1]=this_time.year%10+'0';
	
	Stime[2]='-';
	
	Stime[3]=this_time.month/10+'0';
	Stime[4]=this_time.month%10+'0';
	
	Stime[5]='-';
	
	Stime[6]=this_time.day/10+'0';
	Stime[7]=this_time.day%10+'0';
	
	Stime[8]=' ';
	
	Stime[9]=this_time.hour/10+'0';
	Stime[10]=this_time.hour%10+'0';
	
	Stime[11]=':';
	
	Stime[12]=this_time.min/10+'0';
	Stime[13]=this_time.min%10+'0';
	
	Stime[14]=':';
	
	Stime[15]=this_time.sec/10+'0';
	Stime[16]=this_time.sec%10+'0';
	
	Stime[17]='\0';

}
void Date_Str_Get( char *Stime)
{
	Stime[0]=this_time.year%100/10+'0';
	Stime[1]=this_time.year%10+'0';
	
	Stime[2]='-';
	
	Stime[3]=this_time.month/10+'0';
	Stime[4]=this_time.month%10+'0';
	
	Stime[5]='-';
	
	Stime[6]=this_time.day/10+'0';
	Stime[7]=this_time.day%10+'0';
	
	Stime[8]='\0';

}
void day_Str_Get( char *Stime)
{
	Stime[0]=this_time.hour/10+'0';
	Stime[1]=this_time.hour%10+'0';
	
	Stime[2]=':';
	
	Stime[3]=this_time.min/10+'0';
	Stime[4]=this_time.min%10+'0';
	
	Stime[5]=':';
	
	Stime[6]=this_time.sec/10+'0';
	Stime[7]=this_time.sec%10+'0';
	
	Stime[8]='\0';
}

void T2Str(M_TIME *ptime,char *Stime)
{
	Stime[0]=ptime->year%100/10+'0';
	Stime[1]=ptime->year%10+'0';
	
	Stime[2]='-';
	
	Stime[3]=ptime->month/10+'0';
	Stime[4]=ptime->month%10+'0';
	
	Stime[5]='-';
	
	Stime[6]=ptime->day/10+'0';
	Stime[7]=ptime->day%10+'0';
	
	Stime[8]='\0';
}

void D2Str(M_TIME *ptime,uint8_t *id,char *Stime)
{
	Stime[0]=ptime->hour/10+'0';
	Stime[1]=ptime->hour%10+'0';
	
	Stime[2]=':';
	
	Stime[3]=ptime->min/10+'0';
	Stime[4]=ptime->min%10+'0';
	
	Stime[5]=':';
	
	Stime[6]=ptime->sec/10+'0';
	Stime[7]=ptime->sec%10+'0';
	
	Stime[8]=',';
	
	Stime[9] =(*id)/10+'0';
	Stime[10]=(*id)%10+'0';
	
	Stime[11]='\n';
	Stime[12]='\0';
}
