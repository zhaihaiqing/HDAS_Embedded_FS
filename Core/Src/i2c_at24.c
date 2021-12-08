/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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



#define AT24CXX_DEV_ADDR 		0xa0	//器件地址
#define AT24CXX_PAGE_SIZE		32				//每页32个字节
#define AT24CXX_DELAY_MS    rt_thread_mdelay	//ms级延时


void AT24CXX_WriteByte(uint16_t reg_address,uint8_t data)
{
    static HAL_StatusTypeDef i;
    i = HAL_I2C_Mem_Write(&hi2c1,AT24CXX_DEV_ADDR,reg_address,I2C_MEMADD_SIZE_16BIT,&data,1,100);
    AT24CXX_DELAY_MS(10);    //AT24C64的最长写入时间是5ms
}


uint8_t AT24CXX_ReadByte(uint16_t reg_address)
{
    uint8_t data = 0;
    uint8_t temp[2];
    temp[0] = reg_address>>8;
    temp[1] = reg_address;
    //HAL_I2C_Master_Transmit(&hi2c1,AT24CXX,temp,2,100);
    //HAL_I2C_Master_Receive(&hi2c1,AT24CXX,&data,1,100);

    HAL_I2C_Mem_Read(&hi2c1,AT24CXX_DEV_ADDR,reg_address,I2C_MEMADD_SIZE_16BIT,&data,2,100);
    return data;
}


void AT24CXX_Write(uint16_t WriteAddr,uint8_t *pBuffer,uint8_t WriteNum)
{
    while(WriteNum--)
    {
        AT24CXX_WriteByte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
}


void AT24CXX_Read(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t ReadNum)
{
    while(ReadNum--)
    {
        *pBuffer++=AT24CXX_ReadByte(ReadAddr++);    
    }
}


uint8_t EEWrite(uint16_t addr,uint8_t *pbuff, uint16_t length)
{
	unsigned char temp;
	temp=addr%AT24CXX_PAGE_SIZE;
	if(temp)
	{
		temp=AT24CXX_PAGE_SIZE-temp;
		if(length>=temp)
		{
			HAL_I2C_Mem_Write( &hi2c1, AT24CXX_DEV_ADDR, addr, I2C_MEMADD_SIZE_16BIT, pbuff, temp, 1000 );
			length -= temp;
			addr += temp;
			pbuff += temp;
		}
		else
		{
			if(HAL_I2C_Mem_Write( &hi2c1, AT24CXX_DEV_ADDR, addr, I2C_MEMADD_SIZE_16BIT, pbuff, length, 1000 ) == ERROR)return ERROR;
			length=0;
		}
	}
	
	while(length)
	{
		if(length>=AT24CXX_PAGE_SIZE)
		{
			if(HAL_I2C_Mem_Write( &hi2c1, AT24CXX_DEV_ADDR, addr, I2C_MEMADD_SIZE_16BIT, pbuff, AT24CXX_PAGE_SIZE, 1000 ) == ERROR)return ERROR;
			length-=AT24CXX_PAGE_SIZE;
			addr+=AT24CXX_PAGE_SIZE;
			pbuff+=AT24CXX_PAGE_SIZE;
		}
		else
		{
			if(HAL_I2C_Mem_Write( &hi2c1, AT24CXX_DEV_ADDR, addr, I2C_MEMADD_SIZE_16BIT, pbuff, length, 1000 ) == ERROR)return ERROR;
			length=0;
		}
		AT24CXX_DELAY_MS(10);
	}
	
	return SUCCESS;
}

uint8_t EERead(uint16_t addr,uint8_t *pbuff, uint16_t length)
{
		if(HAL_I2C_Mem_Read(&hi2c1,AT24CXX_DEV_ADDR,addr,I2C_MEMADD_SIZE_16BIT,pbuff,length,1000)==HAL_OK)
		{
			return SUCCESS;
		}
		else
		{
			return ERROR;
		}
}



void AT24Cxx_Test(void)
{
	uint8_t TestData=0;
	
	AT24CXX_Read(255,&TestData,1);
	log_info("TestData:%d\r\n",TestData);
	
	TestData=0XAB;
	AT24CXX_Write(255,&TestData,1);
	
	TestData=0;
	AT24CXX_Read(255,&TestData,1);
	log_info("TestData:%d\r\n",TestData);
	
}




/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
