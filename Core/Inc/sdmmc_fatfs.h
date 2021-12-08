/**
  ******************************************************************************
  * @file    sdmmc.h
  * @brief   This file contains all the function prototypes for
  *          the sdmmc.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SDMMC_FATFS_H__
#define __SDMMC_FATFS_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "fatfs.h"


typedef struct __attribute__ ((__packed__))
{
	uint8_t ch_dat[8];	
}CH_DAT_type;

typedef struct __attribute__ ((__packed__))
{
	CH_DAT_type dat[64];	
}DAT_type;

typedef struct __attribute__ ((__packed__))
{
	char dat[41];	
}DAT_ascii_type;


/* USER CODE END Private defines */
extern FATFS hdas_fs;
extern FIL hdas_fil;
extern char hdas_filename[64];
extern char FsWriteBuf[DATA_NUM][DATA_LEN_SIGNAL];
extern uint32_t byteswritten;

/* USER CODE BEGIN Prototypes */
void Fatfa_GetFree(void);
uint32_t Fatfa_GetFree_2(void);
uint8_t Fatfs_RW_test(void);
void Fatfs_RW_test2(void);

void sprintf_data_to_buf(uint8_t flag);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SDMMC_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
