#include "main.h"

#include "sdmmc.h"
#include "fatfs.h"
#include "sdmmc_fatfs.h"

#include "usb_device.h"


ALIGN(RT_ALIGN_SIZE)

uint8_t sample_f=0;

uint8_t hdas_sample_flag=0;//用来标记采样是否开始，0为停止，1为开始
uint8_t is_hdas_test_ok=0;

struct rt_semaphore storage_sem;

//数据采集线程
//static struct rt_thread das_thread;				//按键扫描线程控制块
//static char das_thread_stack[2048];				//线程堆栈
//#define DAS_THREAD_PRIORITY         8			//线程优先级，按键扫描为最高优先级
//#define DAS_THREAD_TIMESLICE        100			//线程的时间片大小

//数据存储线程
static struct rt_thread storage_thread;				//按键扫描线程控制块
static char storage_thread_stack[2048];				//线程堆栈
#define STORAGE_THREAD_PRIORITY         10			//线程优先级，按键扫描为最高优先级
#define STORAGE_THREAD_TIMESLICE        100			//线程的时间片大小

////命令处理线程
//static struct rt_thread order_thread;				//数码管扫描线程控制块
//static char order_thread_stack[1024];				//线程堆栈
//#define ORDER_THREAD_PRIORITY          11			//线程优先级，按键扫描为最高优先级
//#define ORDER_THREAD_TIMESLICE         100			//线程的时间片大小

//按键处理线程
static struct rt_thread key1_thread;			//温度采集线程控制块
static char key1_thread_stack[2048];				//线程堆栈
#define KEY1_THREAD_PRIORITY          8		//线程优先级，按键扫描为最高优先级
#define KEY1_THREAD_TIMESLICE         100		//线程的时间片大小

//按键处理线程
static struct rt_thread key2_thread;			//温度采集线程控制块
static char key2_thread_stack[2048];				//线程堆栈
#define KEY2_THREAD_PRIORITY          9		//线程优先级，按键扫描为最高优先级
#define KEY2_THREAD_TIMESLICE         100		//线程的时间片大小

//喂狗线程
//static struct rt_thread wdi_thread;				//加热控制线程控制块
//static char wdi_thread_stack[4096];				//线程堆栈
//#define WDI_THREAD_PRIORITY          13			//线程优先级，按键扫描为最高优先级
//#define WDI_THREAD_TIMESLICE         100		//线程的时间片大小

//LED1显示线程
static struct rt_thread led1_thread;			//倒计时线程控制块
static char led1_thread_stack[1024];				//线程堆栈
#define LED1_THREAD_PRIORITY          11		//线程优先级，按键扫描为最高优先级
#define LED1_THREAD_TIMESLICE         100		//线程的时间片大小

//LED2显示线程
//static struct rt_thread led2_thread;				//加热控制线程控制块
//static char led2_thread_stack[512];				//线程堆栈
//#define LED2_THREAD_PRIORITY          15			//线程优先级，按键扫描为最高优先级
//#define LED2_THREAD_TIMESLICE         100		//线程的时间片大小



static struct rt_thread test_thread;				//按键扫描线程控制块
static char test_thread_stack[2048];				//线程堆栈
#define TEST_THREAD_PRIORITY         15			//线程优先级，按键扫描为最高优先级
#define TEST_THREAD_TIMESLICE        100			//线程的时间片大小



int hdas_test_thread_init(void)
{
	rt_thread_init(&test_thread,					//线程控制块
                   "test_thread",					//线程控制块名字
                   test_thread_entry,				//线程入口函数
                   RT_NULL,							//线程入口函数的参数
                   &test_thread_stack[0],			//线程栈起始地址
                   sizeof(test_thread_stack),		//线程栈大小
                   TEST_THREAD_PRIORITY, 			//线程优先级
									 TEST_THREAD_TIMESLICE);		//线程时间片大小
									 
	rt_thread_startup(&test_thread); 
	return 0;									 
									 
}

void test_thread_entry(void *par)
{
		uint8_t val;
	
//		Fatfs_RW_test();
//	
//		val=Sys_Status.uart8+Sys_Status.i2c+Sys_Status.sdmmc2+Sys_Status.fatfs;
//		
//		if(val)//硬件有错误
//		{
//			if(Sys_Status.uart8)log_info("Uart8 Init Error!!!\r\n");
//			else log_info("Uart8 Init OK.\r\n");
//			
//			if(Sys_Status.i2c)log_info("I2C1 Init Error!!!\r\n");
//			else log_info("I2C1 Init OK.\r\n");
//			
//			if(Sys_Status.sdmmc2)log_info("Sdmmc2 Init Error!!!\r\n");
//			else log_info("Sdmmc2 Init OK.\r\n");
//			
//			if(Sys_Status.fatfs==1)log_info("Fatfs Mount Error!!!\r\n");
//			else log_info("Fatfs Mount OK.\r\n");
//			
//			if(Sys_Status.fatfs==2)log_info("Fatfs f_mkfs Error!!!\r\n");
//			else log_info("Fatfs f_mkfs OK.\r\n");
//			
//			if(Sys_Status.fatfs==3)log_info("Fatfs Error,File system write speed is not enough!!!\r\n");
//			else log_info("Fatfs OK.\r\n");
//						
//			
//			
//			while(1)
//			{
//				clc_watchdog();
//				LED1_ON();
//				LED2_ON();
//				rt_thread_mdelay(250);
//				clc_watchdog();
//				LED1_OFF();
//				LED2_OFF();
//				rt_thread_mdelay(250);
//			}
//		}
//		else
//		{
//			log_info("Uart8 Init OK.\r\n");
//			log_info("I2C1 Init OK.\r\n");
//			log_info("Sdmmc2 Init OK.\r\n");
//			log_info("Fatfs Mount OK.\r\n");
//			log_info("Fatfs f_mkfs OK.\r\n");
//			log_info("Fatfs OK.\r\n");
//		}
		
		is_hdas_test_ok=1;		//测试已经完成
		
		hdas_thread_init();	//创建线程，开始操作系统
		
		while(1)
		{
			HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin);
			rt_thread_mdelay(200);
		}
		
	
}




int hdas_thread_init(void)
{		
	

				   
	rt_thread_init(&storage_thread,					//线程控制块
                   "storage_thread",					//线程控制块名字
                   storage_thread_entry,				//线程入口函数
                   RT_NULL,						//线程入口函数的参数
                   &storage_thread_stack[0],			//线程栈起始地址
                   sizeof(storage_thread_stack),		//线程栈大小
                   STORAGE_THREAD_PRIORITY, 			//线程优先级
									 STORAGE_THREAD_TIMESLICE);		//线程时间片大小
				   

				   
	rt_thread_init(&key1_thread,					//线程控制块
                   "key1_thread",					//线程控制块名字
                   key1_thread_entry,				//线程入口函数
                   RT_NULL,							//线程入口函数的参数
                   &key1_thread_stack[0],			//线程栈起始地址
                   sizeof(key1_thread_stack),		//线程栈大小
                   KEY1_THREAD_PRIORITY, 			//线程优先级
									 KEY1_THREAD_TIMESLICE);		//线程时间片大小
									 
	rt_thread_init(&key2_thread,					//线程控制块
                   "key2_thread",					//线程控制块名字
                   key2_thread_entry,				//线程入口函数
                   RT_NULL,							//线程入口函数的参数
                   &key2_thread_stack[0],			//线程栈起始地址
                   sizeof(key2_thread_stack),		//线程栈大小
                   KEY2_THREAD_PRIORITY, 			//线程优先级
									 KEY2_THREAD_TIMESLICE);		//线程时间片大小
				   

	rt_thread_init(&led1_thread,					//线程控制块
                   "led1_thread",					//线程控制块名字
                   led1_thread_entry,				//线程入口函数
                   RT_NULL,							//线程入口函数的参数
                   &led1_thread_stack[0],			//线程栈起始地址
                   sizeof(led1_thread_stack),		//线程栈大小
                   LED1_THREAD_PRIORITY, 			//线程优先级
									 LED1_THREAD_TIMESLICE);		//线程时间片大小
//	rt_thread_init(&led2_thread,					//线程控制块
//                   "led2_thread",					//线程控制块名字
//                   led2_thread_entry,				//线程入口函数
//                   RT_NULL,							//线程入口函数的参数
//                   &led2_thread_stack[0],			//线程栈起始地址
//                   sizeof(led2_thread_stack),		//线程栈大小
//                   LED2_THREAD_PRIORITY, 			//线程优先级
//									 LED2_THREAD_TIMESLICE);		//线程时间片大小
	
		
	rt_sem_init(&storage_sem,"storage_sem",0,RT_IPC_FLAG_FIFO);//创建用于存储数据的信号量
									 
	//rt_thread_startup(&das_thread); 
  rt_thread_startup(&storage_thread); 
	//rt_thread_startup(&order_thread); 
	rt_thread_startup(&key1_thread); 
	rt_thread_startup(&key2_thread); 
	//rt_thread_startup(&wdi_thread); 
	rt_thread_startup(&led1_thread);
	//rt_thread_startup(&led2_thread);
		   
	//rt_timer_start(&td_timer1);
					
	return 0;
}


//void das_thread_entry(void *par)
//{
//	uint32_t i=0,j=0;
//	while(1)
//	{
//		
//		
//		sample_f=1;
//		if(sample_f)
//		{
//			j++;
//			for(log_info("\r\n[%d]",j),i=0;i<8;i++)
//			{
//				log_info("%d=%.3fV ",i,(1.0*g_sAd7606Buf[i]*1.11274*5/32767));
//				//log_info("%d=%fV ",i,(1.0*g_sAd7606Buf[i]*1.11274*4.5/32767));
//				
//			}
//		}
//		
//		
//		rt_thread_mdelay(200);
//	}
//}


void storage_thread_entry(void *par)
{
	uint32_t signal_file_write_count=0;
	
	rt_thread_mdelay(10);	
	
		while (1)
  {
	  //HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
		//等待存储信号量
		//DMA半完成时，将缓存空间的前半部分进行sprintf，进行保存
		//DMA完成时，将缓存空间的后半部分进行sprintf，进行保存
		
		rt_sem_control(&storage_sem,RT_IPC_CMD_RESET,RT_NULL);//等待信号量
		rt_sem_take(&storage_sem,RT_WAITING_FOREVER);
		
		if(DMAdat_flag==0)//DMA传输半完成，将前半部分放入SD缓冲区
		{
			sprintf_data_to_buf(0);//
			
		}
		else if(DMAdat_flag==1)//DMA传输全完成，将后半部分放入SD缓冲区，同时开启文件存储
		{
			
			sprintf_data_to_buf(1);
			
			retSD = f_write(&hdas_fil, FsWriteBuf, sizeof(FsWriteBuf), &byteswritten);	
			f_lseek(&hdas_fil, hdas_fil.fptr);											 
			
			signal_file_write_count++;
			if(signal_file_write_count==FS_WRITE_NUM)//写入次数够了（单个文件大小限制在328MB），先关闭原文件，再新建文件
			{
				signal_file_write_count=0;
				retSD = f_close(&hdas_fil);
				if(retSD){log_info("close error : %d\r\n",retSD);}
				//else{log_info("close sucess!!! \r\n");}
	
	
				if(Fatfa_GetFree_2()>=500)//1：获取剩余容量,如果容量够，则文件号增加1，并新建一个文件
				{
					uint32_t len=0;
					
					file_number++;
					
					len=sprintf(hdas_filename,"hdas_data%d.txt",file_number);
					retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
					if(retSD) {log_info("open file error : %d\r\n",retSD);}		
					else {log_info("Creat file:hdas_data%d.txt",file_number);}
				}
				else											//2：如果剩余容量不够，则停止采样，同时关闭文件，卸载SD卡
				{
					log_info("SD Card Available space <500MB,AD7606 Sampling has stopped,Please replace the SD card and Reset Device!!!\r\n");
					AD7606_Stop();
					hdas_sample_flag=0;
				
				
					retSD = f_close(&hdas_fil);
					if(retSD){log_info("close error : %d\r\n",retSD);}
					else{log_info("close sucess!!! \r\n");}
	
					f_mount(NULL, "", 1);
				}
			}
			
			log_info(".");
		}
  }
	
}

//void order_thread_entry(void *par)
//{
//	rt_thread_mdelay(10);
//	//AT24Cxx_Test();
//	while(1)
//	{
//		rt_thread_mdelay(1000);
//	}
//}

void key1_thread_entry(void *par)
{
	rt_thread_mdelay(10);
	while(1)
	{
		if(!READ_KEY1())
		{
			while(!READ_KEY1())
			{
				rt_thread_mdelay(10);
			}
			log_info("Press Key1\r\n");
			if(hdas_sample_flag==0)
			{
				BYTE work[_MAX_SS];
				uint32_t len=0;
				
				hdas_sample_flag=1;
				
				
				MX_USB_DEVICE_DeInit();	//关闭USB
				rt_thread_mdelay(100);
				
				log_info("Start AD7606 Sampling!!!\r\n");
				LED2_ON();
				
				retSD = f_mount(&hdas_fs, "", 1);
				if(retSD){log_info("mount error : %d \r\n",retSD);Error_Handler();}
				else{log_info("mount sucess!!! \r\n");}
				
//				retSD=f_mkfs("",FM_FAT32,SDCARD_UNIT_SIZE,work,sizeof(work));
//				if(retSD) {log_info("f_mkfs error : %d\r\n",retSD);}		
//				else {log_info("f_mkfs sucess!!! \r\n");}
				
				len=sprintf(hdas_filename,"hdas_data%d.txt",file_number);
				retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
				if(retSD) {log_info("open file error : %d\r\n",retSD);}		
				else {log_info("Creat file:hdas_data%d.txt",file_number);}
				
				
				MX_DMA_Init();
				MX_TIM2_Init(AD7606_SAMPLE_RATE,50);
				MX_FMC_Init();
				
				rt_thread_mdelay(10);
				
				
				AD7606_INIT();			 //开始采集，挂载SD卡，格式化U盘，开始前复位所有参数，并初始化DMA、TIM、AD，
				AD7606_Start();
				//hdas_sample_flag=1;
				
			}
		}
		rt_thread_mdelay(10);
	}
}

void key2_thread_entry(void *par)
{
	rt_thread_mdelay(10);
	while(1)
	{		
		if(!READ_KEY2())
		{
			clc_watchdog();
			while(!READ_KEY2())
			{
				rt_thread_mdelay(10);
				clc_watchdog();
			}
			log_info("Press Key2\r\n");
			if(hdas_sample_flag==1)
			{
				log_info("Stop AD7606 Sampling!!!\r\n");
				LED2_OFF();
				clc_watchdog();
				
				AD7606_Stop();//停止采集，卸载SD卡
				hdas_sample_flag=0;
				
				file_number++;
				
				clc_watchdog();
				retSD = f_close(&hdas_fil);
				clc_watchdog();
				if(retSD)
				{
					uint8_t retSD1=0;
					log_info("close error : %d\r\n",retSD);
					rt_thread_mdelay(100);
					clc_watchdog();
					retSD1 = f_close(&hdas_fil);
					clc_watchdog();
					if(retSD1)
					{
						log_info("close error : %d\r\n",retSD1);
					}
					else{log_info("close sucess!!! \r\n");}
				}
				else{log_info("close sucess!!! \r\n");}
	
				f_mount(NULL, "", 1);
				
				{
					int ix=10;

					while(ix)
					{
						rt_thread_mdelay(100);
						clc_watchdog();
						ix--;
					}
				}
				MX_USB_DEVICE_Init();//初始化USB
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
			
		}
		rt_thread_mdelay(10);
	}
}


//void wdi_thread_entry(void *par)
//{
//	rt_thread_mdelay(10);	
//	
//	while(1)
//	{
//		HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin);
//		rt_thread_mdelay(500);
//	}
//}

void led1_thread_entry(void *par)
{
	rt_thread_mdelay(10);	
	//rt_thread_delete(&test_thread);
	
	while(1)
	{
		rt_thread_mdelay(75);
		LED1_TOGGLE();
	}
}

//void led2_thread_entry(void *par)
//{
//		rt_thread_mdelay(10);	
//	while(1)
//	{
//		rt_thread_mdelay(270);
//		LED2_TOGGLE();
//	}
//}





void start_sampling(void)
{
	if(is_hdas_test_ok==1)//硬件测试完成
	{
		rt_thread_mdelay(20);
		
		
		if(hdas_sample_flag==0)
		{
				BYTE work[_MAX_SS];
				uint32_t len=0;
				
				hdas_sample_flag=1;
			
				log_info("HDAS Rx command Start AD7606 Sampling!!!\r\n");
				LED2_ON();
				
				retSD = f_mount(&hdas_fs, "", 1);
				if(retSD){log_info("mount error : %d \r\n",retSD);Error_Handler();}
				else{log_info("mount sucess!!! \r\n");}
				
//				retSD=f_mkfs("",FM_FAT32,SDCARD_UNIT_SIZE,work,sizeof(work));
//				if(retSD) {log_info("f_mkfs error : %d\r\n",retSD);}		
//				else {log_info("f_mkfs sucess!!! \r\n");}
				
				len=sprintf(hdas_filename,"hdas_data%d.txt",file_number);
				retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
				if(retSD) {log_info("open file error : %d\r\n",retSD);}		
				else {log_info("Creat file:hdas_data%d.txt",file_number);}
				
				
				MX_DMA_Init();
				MX_TIM2_Init(AD7606_SAMPLE_RATE,50);
				MX_FMC_Init();
				
				rt_thread_mdelay(10);
				
				
				AD7606_INIT();			 //开始采集，挂载SD卡，格式化U盘，开始前复位所有参数，并初始化DMA、TIM、AD，
				AD7606_Start();				
			}
	}
	else
	{
		log_info("hdas test fiale!\r\n");
	}
}
MSH_CMD_EXPORT(start_sampling, HDAS Start Sampling);


void stop_sampling(void)
{
	if(hdas_sample_flag==1)
	{
			log_info("HDAS Rx command,Stop AD7606 Sampling!!!\r\n");
			LED2_OFF();
				
			AD7606_Stop();//停止采集，卸载SD卡
			hdas_sample_flag=0;
				
				
			retSD = f_close(&hdas_fil);
				if(retSD)
				{
					uint8_t retSD1=0;
					log_info("close error : %d\r\n",retSD);
					rt_thread_mdelay(100);
					retSD1 = f_close(&hdas_fil);
					if(retSD1)
					{
						log_info("close error : %d\r\n",retSD1);
					}
					else{log_info("close sucess!!! \r\n");}
				}
				else{log_info("close sucess!!! \r\n");}
	
			f_mount(NULL, "", 1);
	}
	else
	{
		log_info("hdas has stop!\r\n");
	}
}
MSH_CMD_EXPORT(stop_sampling, HDAS Stop Sampling);


//void sample(int argc, char **argv)//push on 1 10    [开关,方向,步数]
//{
//	if (argc < 2)
//	{
//		rt_kprintf("cmd err");
//	}
//	else
//	{
//		if (!strcmp(argv[1], "start"))
//		{
//			AD7606_Start();
//			sample_f=1;
//		}
//		else if (!strcmp(argv[1], "stop"))
//		{
//			AD7606_Stop();
//			sample_f=0;
//		}
//		else
//		{
//			rt_kprintf("cdm err");
//			return;
//		}
//	}
//}
//MSH_CMD_EXPORT(sample, sample start or stop);










