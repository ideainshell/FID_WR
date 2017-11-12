#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 

void _ttywrch(int ch) 
{ 
ch = ch; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 
 

#if EN_USART1_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART_RX_STA=0;       //接收状态标记	 
	u8 RevFlag=0;
	u8 Revtemp[UART_MAX_LEN];
u8 UartBuf[UART_MAX_LEN] = { 0 };
u8 UartCount = 0, UartLen = 0, UartStart = FALSE, UartComp = FALSE;
//初始化IO 串口1 
//bound:波特率
void uart_init(u32 bound){
    //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
 	USART_DeInit(USART1);  //复位串口1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA10


   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;// 设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART1, &USART_InitStructure); //初始化串口
#if EN_USART1_RX		  //如果使能了接收  
   //Usart1 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
#endif
    USART_Cmd(USART1, ENABLE);                    //使能串口 

}
 u8 len=0;
u16 crctemp=0;
void USART1_IRQHandler(void)                	//串口1中断服务程序
{	
	OS_ERR err;
	OSIntEnter();
	 	u8 ReByte;
	  u8 i;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
	{

		ReByte=USART_ReceiveData(USART1);               //接收
	  Revtemp[len]=ReByte;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //清标志		
		if(len==0)//第一位
		{
			if(ReByte==0xEF)	len++;
			else 
			{
				len=0;
				RevFlag=2;  //错误标志
			}
    }
		else if(len==1) //第二位  包头
		{
			if(ReByte==0x01)len++;
			else 
			{
				len=0;
				RevFlag=2;
			}
    }
		else if(len>=2&&len<9)//地址位 指令位 数据长度
		{
					len++;
		}
		else if(len>=9&&len<=Revtemp[8]+8)//多余位数存在最后一个字节内
		{
			len++;
			if(len==Revtemp[8]+9)
			{
				for(i=6;i<len-1;i++)
				 crctemp=crctemp+Revtemp[i];
				if(Revtemp[Revtemp[8]+8]+Revtemp[Revtemp[8]+7]*256==crctemp)
			  {  
					RevFlag=1;
				   len=0;
					crctemp=0;
				}
				else
				{
					crctemp=0;
					len=0;
					RevFlag=2;
				}
			}
		}
		if(RevFlag==1)
		{
			RevFlag=0;
			//USART_RX_STA=1;//成功完成标志
			OSSemPost(&share_Usart,OS_OPT_POST_ALL,&err);
    }	
		
  }
	OSIntExit();
} 
void USART1_SendByte(unsigned char Data)
{
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, Data);
}

void USART1_SendString(unsigned char *ch)
{
	while(*ch!=0)
	{		
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, *ch);
		ch++;
	}   	
}
void USART1_SendData(unsigned char *ch,unsigned int len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		while(!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
		USART_SendData(USART1, ch[i]);
	}
}
void USART1_SendData_new(unsigned char *ch,unsigned int len)
{
	unsigned char i;
	for(i=0;i<len;i++)
	{
		//while(!USART_GetFlagStatus(USART2, USART_FLAG_TXE));
		USART_SendData(USART1, ch[i]);
	}
}

 
#endif	

