#include "led.h"

/*
 * ��������LED_GPIO_Config
 * ����  ������LED�õ���I/O��
 * ����  ����
 * ���  ����
 */
void LED_GPIO_Config(void)
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����GPIOE������ʱ��*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE); 

	/*ѡ��Ҫ���Ƶ�GPIOE����*/															   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��GPIOE*/
  	GPIO_Init(GPIOE, &GPIO_InitStructure);		  
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); 

	/*ѡ��Ҫ���Ƶ�GPIOE����*/															   
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��GPIOE*/
  	GPIO_Init(GPIOA, &GPIO_InitStructure);		  

	/* �ر�����led��	*/
	GPIO_ResetBits(GPIOE, GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);	
  GPIO_ResetBits(GPIOA, GPIO_Pin_6);	
}


/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
