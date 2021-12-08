/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sdmmc.h"
#include "fatfs.h"
#include "sdmmc_test.h"
#include "sdmmc_fatfs.h"

#include "usb_device.h"
//#include "usart.h"
//#include "gpio.h"


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_Get_CPU_RCC_Clock(void);
void CPU_CACHE_Enable(void);
void MPU_Config( void );


/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
Sys_Status_type Sys_Status;

uint8_t DMAdat_flag=0;//用来标记是半完成中断，还是全部完成中断



void clc_watchdog(void)
{
	HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin);
}


//void get_reset_status(void)
//{
//	RCC_GetFlagStatus(RCC_FLAG_BORRST);
//	if(READ_BIT(RCC->CSR,RCC_FLAG_BORRST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_BORRST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_PINRST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_PINRST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_PORRST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_PORRST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_SFTRST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_SFTRST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_IWDG1RST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_IWDG1RST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_WWDG1RST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_WWDG1RST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_LPWR1RST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_LPWR1RST\r\n");
//	}
//	if(READ_BIT(RCC->CSR,RCC_FLAG_LPWR2RST)!=RESET)
//	{
//		log_info("RESET Src:RCC_FLAG_LPWR2RST\r\n");
//	}
//	 __HAL_RCC_CLEAR_RESET_FLAGS();
//}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
uint32_t file_number=0; //文件编号

int main(void)
{
	
	
	uint32_t i=0,j=0;
	
//	uint32_t signal_file_write_count=0;
	
	//MPU_Config();
	//CPU_CACHE_Enable();

  //HAL_Init();
  //SystemClock_Config();

  //MX_GPIO_Init();
  //MX_ADC1_Init();
  //MX_UART8_Init();
	
	log_info("[HDAS]Base Hardware Init OK!\r\n");
	//get_reset_status();
	
  MX_SDMMC2_SD_Init();
	clc_watchdog();
	MX_FATFS_Init();
	clc_watchdog();
	
	MX_DMA_Init();
  MX_TIM2_Init(AD7606_SAMPLE_RATE,10);
  MX_FMC_Init();
	clc_watchdog();
	
	HAL_Get_CPU_RCC_Clock();
	
	rt_thread_mdelay(100);
	clc_watchdog();
	
	Get_CardInfo();
	clc_watchdog();
	
//SD_EraseTest(); 
//SD_Write_Read_Test();
//Fatfa_GetFree();
//Fatfs_RW_test();	
//Fatfs_RW_test2();
//rt_thread_mdelay(5000);
		
	//在此处判断各个测试项是否正常，打印相关信息
	
	clc_watchdog();
	
	
		
		Fatfs_RW_test();//FATFS文件系统速度测试
		
		//USB代码初始化块
		{
			{
				int ix=20;
			
				while(ix)
				{
					rt_thread_mdelay(100);
					clc_watchdog();
					ix--;
				}
			}
			
			rt_thread_mdelay(200);
			log_info("MX_USB_Device Init\r\n");
			MX_USB_DEVICE_Init();
			log_info("MX_USB_Device Init Over!!!\r\n");
		
			{
				int ix=35;
			
				while(ix)
				{
					rt_thread_mdelay(100);
					clc_watchdog();
					ix--;
				}
			}
		}
		
		//硬件测试块
		
		{
				uint8_t val=0;
				
			
				val=Sys_Status.uart8+Sys_Status.i2c+Sys_Status.sdmmc2+Sys_Status.fatfs;
				
				if(val)//硬件有错误
				{
					if(Sys_Status.uart8)log_info("Uart8 Init Error!!!\r\n");
					else log_info("Uart8 Init OK.\r\n");
					
					if(Sys_Status.i2c)log_info("I2C1 Init Error!!!\r\n");
					else log_info("I2C1 Init OK.\r\n");
					
					if(Sys_Status.sdmmc2)log_info("Sdmmc2 Init Error!!!\r\n");
					else log_info("Sdmmc2 Init OK.\r\n");
					
					if(Sys_Status.fatfs==1)log_info("Fatfs Mount Error!!!\r\n");
					else log_info("Fatfs Mount OK.\r\n");
					
					if(Sys_Status.fatfs==2)log_info("Fatfs f_mkfs Error!!!\r\n");
					else log_info("Fatfs f_mkfs OK.\r\n");
					
					if(Sys_Status.fatfs==3)log_info("Fatfs Error,File system write speed is not enough!!!\r\n");
					else log_info("Fatfs OK.\r\n");
					
					while(1)
					{
						clc_watchdog();
						LED1_ON();
						LED2_ON();
						rt_thread_mdelay(250);
						clc_watchdog();
						LED1_OFF();
						LED2_OFF();
						rt_thread_mdelay(250);
					}
				}
				else
				{
					log_info("Uart8 Init OK.\r\n");
					log_info("I2C1 Init OK.\r\n");
					log_info("Sdmmc2 Init OK.\r\n");
					log_info("Fatfs Mount OK.\r\n");
					log_info("Fatfs f_mkfs OK.\r\n");
					log_info("Fatfs OK.\r\n");
				}
		}
		
		
		
		hdas_test_thread_init();	//执行测试程序，创建测试线程，测试结束，创建任务线程，开始RTOS
		
		
		while(1)
		{
			rt_thread_mdelay(100);
		}
	
	
//  while (1)
//  {
//	  //HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
//		//等待存储信号量
//		//DMA半完成时，将缓存空间的前半部分进行sprintf，进行保存
//		//DMA完成时，将缓存空间的后半部分进行sprintf，进行保存
//		
//		rt_sem_control(&storage_sem,RT_IPC_CMD_RESET,RT_NULL);//等待信号量
//		rt_sem_take(&storage_sem,RT_WAITING_FOREVER);
//		
//		if(DMAdat_flag==0)//DMA传输半完成，将前半部分放入SD缓冲区
//		{
//			sprintf_data_to_buf(0);//
//			
//		}
//		else if(DMAdat_flag==1)//DMA传输全完成，将后半部分放入SD缓冲区，同时开启文件存储
//		{
//			
//			sprintf_data_to_buf(1);
//			
//			retSD = f_write(&hdas_fil, FsWriteBuf, sizeof(FsWriteBuf), &byteswritten);	
//			f_lseek(&hdas_fil, hdas_fil.fptr);											 
//			
//			signal_file_write_count++;
//			if(signal_file_write_count==FS_WRITE_NUM)//写入次数够了（单个文件大小限制在328MB），先关闭原文件，再新建文件
//			{
//				signal_file_write_count=0;
//				retSD = f_close(&hdas_fil);
//				if(retSD){log_info("close error : %d\r\n",retSD);}
//				//else{log_info("close sucess!!! \r\n");}
//	
//	
//				if(Fatfa_GetFree_2()>=500)//1：获取剩余容量,如果容量够，则文件号增加1，并新建一个文件
//				{
//					uint32_t len=0;
//					
//					file_number++;
//					
//					len=sprintf(hdas_filename,"hdas_data%d.txt",file_number);
//					retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
//					if(retSD) {log_info("open file error : %d\r\n",retSD);}		
//					else {log_info("Creat file:hdas_data%d.txt",file_number);}
//				}
//				else											//2：如果剩余容量不够，则停止采样，同时关闭文件，卸载SD卡
//				{
//					log_info("SD Card Available space <500MB,AD7606 Sampling has stopped,Please replace the SD card and Reset Device!!!\r\n");
//					AD7606_Stop();
//					hdas_sample_flag=0;
//				
//				
//					retSD = f_close(&hdas_fil);
//					if(retSD){log_info("close error : %d\r\n",retSD);}
//					else{log_info("close sucess!!! \r\n");}
//	
//					f_mount(NULL, "", 1);
//				}
//			}
//			
//			log_info(".");
//		}
//  }
  /* USER CODE END 3 */
}





/**
  * @brief System Clock Configuration
  * @retval None
  */
RCC_OscInitTypeDef RCC_OscInitStruct = {0};
 #if 0
void SystemClock_Config(void)
{
  
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 160;
	//RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}
#else
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	//RCC_OscInitTypeDef RCC_OscInitStruct;
	HAL_StatusTypeDef ret = HAL_OK;

	/* 锁住SCU(Supply configuration update) */
	MODIFY_REG(PWR->CR3, PWR_CR3_SCUEN, 0);

	/* 
      1、芯片内部的LDO稳压器输出的电压范围，可选VOS1，VOS2和VOS3，不同范围对应不同的Flash读速度，
         详情看参考手册的Table 12的表格。
      2、这里选择使用VOS1，电压范围1.15V - 1.26V。
    */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

	/* 使能HSE，并选择HSE作为PLL时钟源，如果USB使用HSI48的话，这里要开启*/
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
//	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
//	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
//	RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
//	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
//	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
//	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
//		
//	RCC_OscInitStruct.PLL.PLLM = 5;
//	RCC_OscInitStruct.PLL.PLLN = 160;
//	RCC_OscInitStruct.PLL.PLLP = 2;
//	RCC_OscInitStruct.PLL.PLLR = 2;
//	RCC_OscInitStruct.PLL.PLLQ = 4;		
//		
//	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
//	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;





	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;


	ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);		
	if(ret != HAL_OK)
	{
        Error_Handler();
	}

	/* 
       选择PLL的输出作为系统时钟
       配置RCC_CLOCKTYPE_SYSCLK系统时钟
       配置RCC_CLOCKTYPE_HCLK 时钟，对应AHB1，AHB2，AHB3和AHB4总线
       配置RCC_CLOCKTYPE_PCLK1时钟，对应APB1总线
       配置RCC_CLOCKTYPE_PCLK2时钟，对应APB2总线
       配置RCC_CLOCKTYPE_D1PCLK1时钟，对应APB3总线
       配置RCC_CLOCKTYPE_D3PCLK1时钟，对应APB4总线     
    */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
								 RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
	
	/* 此函数会更新SystemCoreClock，并重新配置HAL_InitTick */
	ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
	if(ret != HAL_OK)
	{
        Error_Handler();
	}

    /*
      使用IO的高速模式，要使能IO补偿，即调用下面三个函数 
      （1）使能CSI clock
      （2）使能SYSCFG clock
      （3）使能I/O补偿单元， 设置SYSCFG_CCCSR寄存器的bit0
    */
	__HAL_RCC_CSI_ENABLE() ;

	__HAL_RCC_SYSCFG_CLK_ENABLE() ;

	HAL_EnableCompensationCell();

   /* AXI SRAM的时钟是上电自动使能的，而D2域的SRAM1，SRAM2和SRAM3要单独使能 */	
#if 0
	__HAL_RCC_D2SRAM1_CLK_ENABLE();
	__HAL_RCC_D2SRAM2_CLK_ENABLE();
	__HAL_RCC_D2SRAM3_CLK_ENABLE();
#endif
	
	{
		/*
			USB工作需要48MHz的时钟，可以由PLL1Q，PLL3Q和HSI48提供
			PLL1Q用于给SDMMC提供时钟
		    PLL3Q给LTDC提供时钟，也可以跟USB共用，不过得更USB设置相同的频率才可一起用。
		    HSI48可以供USB独享，就是精度不是很高。
		*/
		RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
#if 0
		/* PLL3-Q for USB Clock = 48M */
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
		PeriphClkInitStruct.PLL3.PLL3M = 5;
		PeriphClkInitStruct.PLL3.PLL3N = 48;
		PeriphClkInitStruct.PLL3.PLL3P = 2;
		PeriphClkInitStruct.PLL3.PLL3Q = 5;
		PeriphClkInitStruct.PLL3.PLL3R = 2;
		PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_2;
		PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
		PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
		PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct)!= HAL_OK)
		{
			Error_Handler(__FILE__, __LINE__);
		}	
#else
		PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
		PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
		if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
		{
			Error_Handler(); 
		}	
#endif
	}	

}
#endif
/* USER CODE BEGIN 4 */

void MPU_Config( void )
{
	MPU_Region_InitTypeDef MPU_InitStruct;

	/* 禁止 MPU */
	HAL_MPU_Disable();

	/* 配置AXI SRAM的MPU属性为Write through, read allocate，no write allocate */
	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress      = 0x24000000;
	MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
	MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
	
//	/* 配置FMC扩展IO的MPU属性为Device或者Strongly Ordered */
//	MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
//	MPU_InitStruct.BaseAddress      = 0x60000000;
//	MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_64KB;	
//	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
//	MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
//	MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;	/* 不能用MPU_ACCESS_CACHEABLE;会出现2次CS、WE信号 */
//	MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
//	MPU_InitStruct.Number           = MPU_REGION_NUMBER1;
//	MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
//	MPU_InitStruct.SubRegionDisable = 0x00;
//	MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
//	
//	HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
	/* 配置SRAM3的属性为Write through, read allocate，no write allocate */
//    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
//    MPU_InitStruct.BaseAddress      = 0x38000000;
//    MPU_InitStruct.Size             = ARM_MPU_REGION_SIZE_64KB;	
//    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
//    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
//    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
//    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
//    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
//    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
//    MPU_InitStruct.SubRegionDisable = 0x00;
//    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;

//    HAL_MPU_ConfigRegion(&MPU_InitStruct);

	/*使能 MPU */
	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}

//void HAL_Get_CPU_RCC_Clock(void)
//{
//	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_SysClockFreq:%fMHz\r\n",1.0*HAL_RCC_GetSysClockFreq()/1000000);
//	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_HCLKFreq:%fMHz\r\n",1.0*HAL_RCC_GetHCLKFreq()/1000000);
//	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_PCLK1Freq:%fMHz\r\n",1.0*HAL_RCC_GetPCLK1Freq()/1000000);
//	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_PCLK2Freq:%fMHz\r\n",1.0*HAL_RCC_GetPCLK2Freq()/1000000);	
//}

void HAL_Get_CPU_RCC_Clock(void)
{
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_SysClockFreq:%dMHz\r\n",HAL_RCC_GetSysClockFreq()/1000000);
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_HCLKFreq:%dMHz\r\n",HAL_RCC_GetHCLKFreq()/1000000);
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_PCLK1Freq:%dMHz\r\n",HAL_RCC_GetPCLK1Freq()/1000000);
	log_info("[HAL_Get_CPU_RCC_Clock]HAL_RCC_PCLK2Freq:%dMHz\r\n",HAL_RCC_GetPCLK2Freq()/1000000);	
	
	//log_info("[%s][%s][%d]Error_Handler\r\n",__FILE__,__func__,__LINE__);
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  //__disable_irq();
  //while (1)
  //{
  //}
	//log_info("[%s]Error_Handler\r\n",__func__);
	
	return;
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
