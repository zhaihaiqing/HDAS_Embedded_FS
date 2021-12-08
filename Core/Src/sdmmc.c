/**
  ******************************************************************************
  * @file    sdmmc.c
  * @brief   This file provides code for the configuration
  *          of the SDMMC instances.
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
#include "sdmmc.h"

/* USER CODE BEGIN 0 */
uint32_t sdioclkdiv=0;
/* USER CODE END 0 */

SD_HandleTypeDef hsd2;

HAL_SD_CardInfoTypeDef SDCardInfo;

/* SDMMC2 init function */

uint8_t MX_SDMMC2_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC2_Init 0 */

  /* USER CODE END SDMMC2_Init 0 */

  /* USER CODE BEGIN SDMMC2_Init 1 */

  /* USER CODE END SDMMC2_Init 1 */
  hsd2.Instance = SDMMC2;
  hsd2.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd2.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd2.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd2.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd2.Init.ClockDiv = 5;
  hsd2.Init.TranceiverPresent = SDMMC_TRANSCEIVER_NOT_PRESENT;
  if (HAL_SD_Init(&hsd2) != HAL_OK)
  {
			Sys_Status.sdmmc2=1;
			log_info("SDMMC Init Err\r\n");
			return HAL_ERROR;
			//Error_Handler();
  }
	else
	{
			Sys_Status.sdmmc2=0;
			log_info("SDMMC Init OK\r\n");
			sdioclkdiv=hsd2.Init.ClockDiv;
			return HAL_OK;
	}
  /* USER CODE BEGIN SDMMC2_Init 2 */
	//log_info("SDMMC Init OK\r\n");
  /* USER CODE END SDMMC2_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(sdHandle->Instance==SDMMC2)
  {
  /* USER CODE BEGIN SDMMC2_MspInit 0 */

  /* USER CODE END SDMMC2_MspInit 0 */
  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SDMMC;
    PeriphClkInitStruct.SdmmcClockSelection = RCC_SDMMCCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* SDMMC2 clock enable */
    __HAL_RCC_SDMMC2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SDMMC2 GPIO Configuration
    PB4 (NJTRST)     ------> SDMMC2_D3
    PG11     ------> SDMMC2_D2
    PD6     ------> SDMMC2_CK
    PA0     ------> SDMMC2_CMD
    PB15     ------> SDMMC2_D1
    PB14     ------> SDMMC2_D0
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_15|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_SDIO2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_SDIO2;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF11_SDIO2;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_SDIO2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* SDMMC2 interrupt Init */
    HAL_NVIC_SetPriority(SDMMC2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
  /* USER CODE BEGIN SDMMC2_MspInit 1 */

  /* USER CODE END SDMMC2_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDMMC2)
  {
  /* USER CODE BEGIN SDMMC2_MspDeInit 0 */

  /* USER CODE END SDMMC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDMMC2_CLK_DISABLE();

    /**SDMMC2 GPIO Configuration
    PB4 (NJTRST)     ------> SDMMC2_D3
    PG11     ------> SDMMC2_D2
    PD6     ------> SDMMC2_CK
    PA0     ------> SDMMC2_CMD
    PB15     ------> SDMMC2_D1
    PB14     ------> SDMMC2_D0
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4|GPIO_PIN_15|GPIO_PIN_14);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);

    /* SDMMC2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDMMC2_IRQn);
  /* USER CODE BEGIN SDMMC2_MspDeInit 1 */

  /* USER CODE END SDMMC2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Get_CardInfo(void)
{
	double CardCapacity=0;
	float sdmmc_speed=0;
	
	
	HAL_SD_CardCIDTypeDef  SDCardCID;
	HAL_SD_CardCSDTypeDef  SDCardCSD;
	
	
	HAL_SD_GetCardCID(&hsd2,&SDCardCID);
	HAL_SD_GetCardCSD(&hsd2,&SDCardCSD);
	
	HAL_SD_GetCardInfo(&hsd2,&SDCardInfo);
	
	//SDCardInfo.CardType
	
	RCC_OscInitStruct.PLL.PLLM = 5;
	
	sdmmc_speed=((HDAS_HSE/RCC_OscInitStruct.PLL.PLLM)*RCC_OscInitStruct.PLL.PLLN)/RCC_OscInitStruct.PLL.PLLQ;
	
//	log_info("sdmmc_speed=%f\r\n",sdmmc_speed);
//	log_info("PLLM=%d\r\n",RCC_OscInitStruct.PLL.PLLM);
//	log_info("PLLN=%d\r\n",RCC_OscInitStruct.PLL.PLLN);
//	log_info("PLLQ=%d\r\n",RCC_OscInitStruct.PLL.PLLQ);
//	log_info("HDAS_HSE=%d\r\n",HDAS_HSE);
	
	log_info("SDCard CLK:%fMHz\r\n",(sdmmc_speed/1000000)/(sdioclkdiv*2));
	
	log_info("SDCardInfo.CardType:%d\r\n",SDCardInfo.CardType);
	CardCapacity=(double)SDCardInfo.BlockNbr*SDCardInfo.BlockSize/1024/1024/1024;
	log_info("SDCardCID.CardCapacity:%.2fGB\r\n",CardCapacity);	
	log_info("SDCardInfo.CardVersion:%d\r\n",SDCardInfo.CardVersion);
	log_info("SDCardInfo.Class:%d\r\n",SDCardInfo.Class);
	log_info("SDCardInfo.RelCardAdd:%d\r\n",SDCardInfo.RelCardAdd);
	log_info("SDCardInfo.BlockNbr:%d\r\n",SDCardInfo.BlockNbr);
	log_info("SDCardInfo.BlockSize:%d\r\n",SDCardInfo.BlockSize);
	log_info("SDCardInfo.LogBlockNbr:%d\r\n",SDCardInfo.LogBlockNbr);
	log_info("SDCardInfo.LogBlockSize:%d\r\n",SDCardInfo.LogBlockSize);
	
	log_info("SDCardCID.ManufacturerID:%d\r\n",SDCardCID.ManufacturerID);
	
}
















/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
