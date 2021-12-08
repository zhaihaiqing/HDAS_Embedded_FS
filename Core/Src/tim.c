/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "tim.h"
#include "FMC_TIM_AD7606.h"

/* USER CODE BEGIN 0 */
#define Time2_prescaler 20000000
/* USER CODE END 0 */

TIM_HandleTypeDef htim2;
DMA_HandleTypeDef hdma_tim2_up;

extern int16_t g_sAd7606Buf[AD7606_BUFSIZE];

/* TIM2 init function */
void MX_TIM2_Init(uint32_t freq,uint8_t pulse1)
{
	uint16_t usPeriod;
	uint16_t usPrescaler;
	uint32_t uiTIMxCLK;
	uint32_t pulse;
	
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
	
	
	uiTIMxCLK=SystemCoreClock/2;
	
	usPrescaler=0;
	usPeriod= uiTIMxCLK / freq - 1;
	pulse = usPeriod - 199;
	
	
	log_info("uiTIMxCLK:%d\r\n",uiTIMxCLK);
	
	htim2.Instance = TIM2;
  htim2.Init.Prescaler = usPrescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = usPeriod;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	

//  htim2.Instance = TIM2;
//  htim2.Init.Prescaler = HAL_RCC_GetSysClockFreq()/2/Time2_prescaler-1;
//  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
//  htim2.Init.Period = Time2_prescaler/freq;
//  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_ITR0;
  if (HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  //sConfigOC.Pulse = Time2_prescaler/freq*pulse/100;
	sConfigOC.Pulse = pulse;
	
	//log_info("sConfigOC.Pulse:%d\r\n",sConfigOC.Pulse);
	
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;    /* 配置互补输出高电平有效 */
	sConfigOC.OCIdleState  = TIM_OCIDLESTATE_SET;     /* 空闲状态时，设置输出比较引脚为高电平 */
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;  /* 空闲状态时，设置互补输出比较引脚为低电平 */
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim2);

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    /* TIM2 DMA Init */
    /* TIM2_UP Init */
    hdma_tim2_up.Instance = DMA1_Stream0;
    hdma_tim2_up.Init.Request = DMA_REQUEST_TIM2_UP;
    hdma_tim2_up.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim2_up.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_up.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_up.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_tim2_up.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_tim2_up.Init.Mode = DMA_CIRCULAR;
    hdma_tim2_up.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim2_up.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_tim2_up.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_tim2_up.Init.MemBurst = DMA_MBURST_INC8;
    hdma_tim2_up.Init.PeriphBurst = DMA_PBURST_INC8;
    /* 复位DMA */
	if (HAL_DMA_DeInit(&hdma_tim2_up) != HAL_OK)
	{
		Error_Handler();
	}
		if (HAL_DMA_Init(&hdma_tim2_up) != HAL_OK)
    {
      Error_Handler();
    }

    //__HAL_LINKDMA(tim_baseHandle,hdma[TIM_DMA_ID_UPDATE],hdma_tim2_up);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */
	/* 注册半传输完成中断和传输完成中断 */
	HAL_DMA_RegisterCallback(&hdma_tim2_up, HAL_DMA_XFER_CPLT_CB_ID, AD7606_DmaCplCb);
	HAL_DMA_RegisterCallback(&hdma_tim2_up, HAL_DMA_XFER_HALFCPLT_CB_ID, AD7606_DmaHalfCplCb);
				/* 启动DMA传输 */
	HAL_DMA_Start_IT(&hdma_tim2_up, (uint32_t)AD7606_BASE, (uint32_t)g_sAd7606Buf, AD7606_BUFSIZE);
  /* USER CODE END TIM2_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspPostInit 0 */
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  /* USER CODE END TIM2_MspPostInit 0 */

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA2     ------> TIM2_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM2_MspPostInit 1 */

  /* USER CODE END TIM2_MspPostInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /* TIM2 DMA DeInit */
    HAL_DMA_DeInit(tim_baseHandle->hdma[TIM_DMA_ID_UPDATE]);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Tim2_pwm_enable(void)
{
		/* 使能定时器中断  */
	__HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_UPDATE);
    /* Start channel 3 */
  if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3) != HAL_OK)
  {
    /* PWM generation Error */
    Error_Handler();
  }

}
void Tim2_pwm_disable(void)
{
		/* 使能定时器中断  */
	__HAL_TIM_DISABLE_DMA(&htim2, TIM_DMA_UPDATE);
    /* Start channel 3 */
  if (HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3) != HAL_OK)
  {
    /* PWM generation Error */
    Error_Handler();
  }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
