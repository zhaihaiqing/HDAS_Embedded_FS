/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "rtthread.h"
#include <stdio.h>
//#include <string.h>
#include "gpio.h"
#include "usart.h"
#include "i2c.h"
#include "i2c_at24.h"
#include "adc.h"

#include "dma.h"
#include "tim.h"
#include "hdas_thread.h"
#include "fmc.h"
#include "FMC_TIM_AD7606.h"
//#include "sdmmc.h"
//#include "fatfs.h"
//#include "sdmmc_fatfs.h"
//#include "adc.h"
//#include "rtc.h"





/****************************************************************************/
#define SDCARD_UNIT_SIZE		64*1024	//��λΪKB


#define AD7606_SAMPLE_RATE 50000			//ADC�����ʣ���λHz


//�ļ��洢����
#define DATA_LEN_SIGNAL		41													//ÿ��������ݹ���41���ַ�������ת����(data*1000)+3276��ֻ�����������֣�4*8+7+2��
#define DATA_NUM					4096												//����д�����������,ע������Ϊż��
#define DATA_BUFSIZE			(DATA_LEN_SIGNAL*DATA_NUM)	//����д�����������82KB
#define FS_WRITE_NUM			2048													//��һ�ļ�д��Ĵ���
#define FILE_SIZE					(DATA_BUFSIZE*FS_WRITE_NUM)	//��һ�ļ��Ĵ�С��MB��328MB

#define AD7606_BUFSIZE     DATA_NUM*8		//DMA���������16*2KB���ݣ�����DATA_NUM*2������


/****************************************************************************/










void Error_Handler(void);

#define	HDAS_HSE	25000000U

#define EMMC_DS_Pin GPIO_PIN_5
#define EMMC_DS_GPIO_Port GPIOD
#define EMMC_RST_Pin GPIO_PIN_15
#define EMMC_RST_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOE
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_5
#define KEY1_GPIO_Port GPIOE
#define KEY2_Pin GPIO_PIN_4
#define KEY2_GPIO_Port GPIOE
#define WDI_Pin GPIO_PIN_12
#define WDI_GPIO_Port GPIOI
#define AD_BUSY_Pin GPIO_PIN_0
#define AD_BUSY_GPIO_Port GPIOJ
#define AD_FRST_Pin GPIO_PIN_15
#define AD_FRST_GPIO_Port GPIOI
#define AD_PSB_Pin GPIO_PIN_1
#define AD_PSB_GPIO_Port GPIOJ
#define POW_CTRL2_Pin GPIO_PIN_6
#define POW_CTRL2_GPIO_Port GPIOA
#define AD_OS2_Pin GPIO_PIN_2
#define AD_OS2_GPIO_Port GPIOB
#define AD_OS1_Pin GPIO_PIN_2
#define AD_OS1_GPIO_Port GPIOJ
#define AD_RST_Pin GPIO_PIN_5
#define AD_RST_GPIO_Port GPIOC
#define AD_OS0_Pin GPIO_PIN_3
#define AD_OS0_GPIO_Port GPIOJ
#define AD_RANG_Pin GPIO_PIN_4
#define AD_RANG_GPIO_Port GPIOJ
/* USER CODE BEGIN Private defines */


#define	READ_KEY1()			HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin)
#define	READ_KEY2()			HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin)

#define LED1_ON()				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
#define LED1_OFF()			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
#define LED1_TOGGLE()		HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);

#define LED2_ON()				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
#define LED2_OFF()			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);
#define LED2_TOGGLE()		HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);

typedef struct __attribute__ ((__packed__))
{
		uint8_t gpio;
		uint8_t adc;
		uint8_t uart8;
		uint8_t sdmmc2;
		uint8_t fatfs;
		uint8_t rtc;	
		uint8_t i2c;
	
		uint32_t SD_ManufacturerID;			//������ID
		uint32_t SD_CardCap;						//MB
		uint32_t SD_FreeCap;						//MB
	
		uint8_t Is_sampling;				//�Ƿ�ʼ����
		uint8_t Is_Storage;					//�Ƿ�ʼ�洢,���β��ã���ʹ���Ƿ��������������ʹ洢��
		uint32_t Samp_Freq;					//����Ƶ��
		uint8_t  Samp_Ch;						//ѡ���ͨ����bit0:7
		
}Sys_Status_type;



extern uint8_t DMAdat_flag;

extern uint32_t file_number;


extern RCC_OscInitTypeDef RCC_OscInitStruct;
extern Sys_Status_type Sys_Status;

void SystemClock_Config(void);

void clc_watchdog(void);


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
