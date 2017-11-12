/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "includes.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	OSIntEnter(); 	 //����ͳ���жϵ�Ƕ�ײ�������Ƕ�ײ���+1
	OSTimeTick();	 //ͳ��ʱ�䣬�������񣬶���ʱ�����ʱ��1
	OSIntExit();	 //��Ƕ�ײ�����1�����˳��ж�ǰ�����������
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
/*
*********************************************************************************************************
*	�� �� ��: SDIO_IRQHandler
*	����˵��: This function handles WWDG interrupt request.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SDIO_IRQHandler(void)
{
  OSIntEnter(); 
	SD_ProcessIRQSrc();
	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
	OSIntExit();
}

/*
*********************************************************************************************************
*	�� �� ��: SD_SDIO_DMA_IRQHANDLER
*	����˵��: This function handles WWDG interrupt request.
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
//void SD_SDIO_DMA_IRQHANDLER(void)
//{
//  	CPU_SR_ALLOC();
//	
//	CPU_CRITICAL_ENTER();  
//	OSIntNestingCtr++;
//	CPU_CRITICAL_EXIT();

//	SD_ProcessDMAIRQ();

//	/* ��os_core.c�ļ��ﶨ��,����и������ȼ������������,��ִ��һ�������л� */
//	OSIntExit();
//}

void RTC_IRQHandler(void)
{
	OSIntEnter(); 
	CPU_SR_ALLOC();	
	CPU_CRITICAL_ENTER(); 
	extern __IO uint32_t TimeDisplay;
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

    /* Toggle GPIO_LED pin 6 each 1s */
    //GPIO_WriteBit(GPIO_LED, GPIO_Pin_6, (BitAction)(1 - GPIO_ReadOutputDataBit(GPIO_LED, GPIO_Pin_6)));

    /* Enable time update */
    TimeDisplay = 1;

    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
    /* Reset RTC Counter when Time is 23:59:59 */
    if (RTC_GetCounter() == 0x0001517F)
    {
			PWR_BackupAccessCmd(ENABLE);   
      /* Wait until last write operation on RTC registers has finished */  
      RTC_WaitForLastTask();  
			
      RTC_SetCounter(0x0);		
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
			Time_add_Day();
    }
  }
	CPU_CRITICAL_EXIT();
	OSIntExit();
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
