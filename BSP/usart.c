#include "usart.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 

void _ttywrch(int ch) 
{ 
ch = ch; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)ch);   
	return ch;
}
#endif 
 

#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	 
	u8 RevFlag=0;
	u8 Revtemp[UART_MAX_LEN];
u8 UartBuf[UART_MAX_LEN] = { 0 };
u8 UartCount = 0, UartLen = 0, UartStart = FALSE, UartComp = FALSE;
//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10


   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;// ����Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
#if EN_USART1_RX		  //���ʹ���˽���  
   //Usart1 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
#endif
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}
 u8 len=0;
u16 crctemp=0;
void USART1_IRQHandler(void)                	//����1�жϷ������
{	
	OS_ERR err;
	OSIntEnter();
	 	u8 ReByte;
	  u8 i;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
	{

		ReByte=USART_ReceiveData(USART1);               //����
	  Revtemp[len]=ReByte;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE); //���־		
		if(len==0)//��һλ
		{
			if(ReByte==0xEF)	len++;
			else 
			{
				len=0;
				RevFlag=2;  //�����־
			}
    }
		else if(len==1) //�ڶ�λ  ��ͷ
		{
			if(ReByte==0x01)len++;
			else 
			{
				len=0;
				RevFlag=2;
			}
    }
		else if(len>=2&&len<9)//��ַλ ָ��λ ���ݳ���
		{
					len++;
		}
		else if(len>=9&&len<=Revtemp[8]+8)//����λ���������һ���ֽ���
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
			//USART_RX_STA=1;//�ɹ���ɱ�־
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

