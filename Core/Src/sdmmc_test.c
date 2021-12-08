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
#include "main.h"

/* USER CODE BEGIN 0 */
#include "sdmmc.h"
//#include "fatfs.h"



typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#define BLOCK_SIZE 512 // SD卡块大小
#define NUMBER_OF_BLOCKS 8 // 测试块数量 (小于 15) 
#define WRITE_READ_ADDRESS 0x00002000 // 测试读写地址

ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint32_t Buffer_Block_Tx[BLOCK_SIZE*NUMBER_OF_BLOCKS]);
ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint32_t Buffer_Block_Rx[BLOCK_SIZE*NUMBER_OF_BLOCKS]);



//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t FsReadBuf[1024]);
//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t FsWriteBuf[1024])={"This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs,This is STM32 working with FatFs\r\n"};

//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t g_TestBuf[BUF_SIZE]);

//__align(4) uint32_t Buffer_Block_Tx[BLOCK_SIZE*NUMBER_OF_BLOCKS]; // 写数据缓存
//__align(4) uint32_t Buffer_Block_Rx[BLOCK_SIZE*NUMBER_OF_BLOCKS]; // 读数据缓存
HAL_StatusTypeDef sd_status; // HAL库函数操作 SD卡函数返回值：操作结果
TestStatus test_status; // 数据测试结果









uint8_t eBuffercmp(uint32_t* pBuffer, uint32_t BufferLength)
{
	while (BufferLength--) 
	{ /* SD卡擦除后的可能值为 0xff或 0 */ 
		if ((*pBuffer != 0xFFFFFFFF) && (*pBuffer != 0)) 
		{ 
			return 0; 
		} 
		pBuffer++; 
	} return 1;
}

void SD_EraseTest(void)
{
	sd_status=HAL_SD_Erase(&hsd2,WRITE_READ_ADDRESS,WRITE_READ_ADDRESS+NUMBER_OF_BLOCKS*4);
	log_info("erase status:%d\r\n",sd_status);
	HAL_Delay(500);
	if (sd_status == HAL_OK)
	{
		//sd_status = HAL_SD_ReadBlocks(&hsd2,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
		sd_status = HAL_SD_WriteBlocks_DMA(&hsd2,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS);
		log_info("erase read status:%d\r\n",sd_status);
		test_status = eBuffercmp(Buffer_Block_Rx,BLOCK_SIZE*NUMBER_OF_BLOCKS);
		if(test_status == PASSED) 
			log_info("》擦除测试成功！ ！\r\n" ); 
		else 
			log_info("》擦除不成功，数据出错！ ！\r\n" );
	}
	else
	{
		log_info("》擦除测试失败！部分 SD不支持擦除，只要读写测试通过即可 \r\n" );
		
	}
}


void Fill_Buffer(uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset) 
{ 
	uint32_t index = 0; /* 填充数据 */ 
	for (index = 0; index < BufferLength; index++ ) 
	{ 
		pBuffer[index] = index + Offset; 
	} 
}

uint8_t Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength) 
{ 
	while (BufferLength--) 
	{ 
		if(BufferLength%50==0) 
		{ 
			log_info("buf:0x%08X 0x%08X\r\n",*pBuffer1,*pBuffer2); 
		} 
		if (*pBuffer1 != *pBuffer2) 
		{ 
			return 0; 
		} 
		pBuffer1++; 
		pBuffer2++; 
	} 
	return 1;
}

void SD_Write_Read_Test(void)
{
	int i,j = 0;
	Fill_Buffer(Buffer_Block_Tx,BLOCK_SIZE*NUMBER_OF_BLOCKS, 0x6666);
	//sd_status = HAL_SD_WriteBlocks(&hsd2,(uint8_t *)Buffer_Block_Tx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
	sd_status = HAL_SD_WriteBlocks_DMA(&hsd2,(uint8_t *)Buffer_Block_Tx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS);
	log_info("write status:%d\r\n",sd_status);
	
	HAL_Delay(500);
	
	//sd_status = HAL_SD_ReadBlocks(&hsd2,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff); 
	sd_status = HAL_SD_ReadBlocks_DMA(&hsd2,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS); 
	log_info("read status:%d\r\n",sd_status);
	test_status = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE*NUMBER_OF_BLOCKS/4);
	if(test_status == PASSED)
	{
		log_info("》读写测试成功！ ！\r\n" );
		for(i=0;i<BLOCK_SIZE*NUMBER_OF_BLOCKS/4;i++)
		{
			if(j==8)
			{
				log_info("\r\n");
				j=0;
			}
			log_info("%08x ",Buffer_Block_Rx[i]);
			j++;
		}
		log_info("\r\n");
	}
	else
	{
		log_info("》读写测试失败！ ！\r\n " );
	}	
}





/* USER CODE END 0 */




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
