#include "main.h"

#include "sdmmc.h"
#include "fatfs.h"
#include "sdmmc_fatfs.h"

#include "usb_device.h"


ALIGN(RT_ALIGN_SIZE)

uint8_t sample_f=0;

uint8_t hdas_sample_flag=0;//������ǲ����Ƿ�ʼ��0Ϊֹͣ��1Ϊ��ʼ
uint8_t is_hdas_test_ok=0;

struct rt_semaphore storage_sem;

//���ݲɼ��߳�
//static struct rt_thread das_thread;				//����ɨ���߳̿��ƿ�
//static char das_thread_stack[2048];				//�̶߳�ջ
//#define DAS_THREAD_PRIORITY         8			//�߳����ȼ�������ɨ��Ϊ������ȼ�
//#define DAS_THREAD_TIMESLICE        100			//�̵߳�ʱ��Ƭ��С

//���ݴ洢�߳�
static struct rt_thread storage_thread;				//����ɨ���߳̿��ƿ�
static char storage_thread_stack[2048];				//�̶߳�ջ
#define STORAGE_THREAD_PRIORITY         10			//�߳����ȼ�������ɨ��Ϊ������ȼ�
#define STORAGE_THREAD_TIMESLICE        100			//�̵߳�ʱ��Ƭ��С

////������߳�
//static struct rt_thread order_thread;				//�����ɨ���߳̿��ƿ�
//static char order_thread_stack[1024];				//�̶߳�ջ
//#define ORDER_THREAD_PRIORITY          11			//�߳����ȼ�������ɨ��Ϊ������ȼ�
//#define ORDER_THREAD_TIMESLICE         100			//�̵߳�ʱ��Ƭ��С

//���������߳�
static struct rt_thread key1_thread;			//�¶Ȳɼ��߳̿��ƿ�
static char key1_thread_stack[2048];				//�̶߳�ջ
#define KEY1_THREAD_PRIORITY          8		//�߳����ȼ�������ɨ��Ϊ������ȼ�
#define KEY1_THREAD_TIMESLICE         100		//�̵߳�ʱ��Ƭ��С

//���������߳�
static struct rt_thread key2_thread;			//�¶Ȳɼ��߳̿��ƿ�
static char key2_thread_stack[2048];				//�̶߳�ջ
#define KEY2_THREAD_PRIORITY          9		//�߳����ȼ�������ɨ��Ϊ������ȼ�
#define KEY2_THREAD_TIMESLICE         100		//�̵߳�ʱ��Ƭ��С

//ι���߳�
//static struct rt_thread wdi_thread;				//���ȿ����߳̿��ƿ�
//static char wdi_thread_stack[4096];				//�̶߳�ջ
//#define WDI_THREAD_PRIORITY          13			//�߳����ȼ�������ɨ��Ϊ������ȼ�
//#define WDI_THREAD_TIMESLICE         100		//�̵߳�ʱ��Ƭ��С

//LED1��ʾ�߳�
static struct rt_thread led1_thread;			//����ʱ�߳̿��ƿ�
static char led1_thread_stack[1024];				//�̶߳�ջ
#define LED1_THREAD_PRIORITY          11		//�߳����ȼ�������ɨ��Ϊ������ȼ�
#define LED1_THREAD_TIMESLICE         100		//�̵߳�ʱ��Ƭ��С

//LED2��ʾ�߳�
//static struct rt_thread led2_thread;				//���ȿ����߳̿��ƿ�
//static char led2_thread_stack[512];				//�̶߳�ջ
//#define LED2_THREAD_PRIORITY          15			//�߳����ȼ�������ɨ��Ϊ������ȼ�
//#define LED2_THREAD_TIMESLICE         100		//�̵߳�ʱ��Ƭ��С



static struct rt_thread test_thread;				//����ɨ���߳̿��ƿ�
static char test_thread_stack[2048];				//�̶߳�ջ
#define TEST_THREAD_PRIORITY         15			//�߳����ȼ�������ɨ��Ϊ������ȼ�
#define TEST_THREAD_TIMESLICE        100			//�̵߳�ʱ��Ƭ��С



int hdas_test_thread_init(void)
{
	rt_thread_init(&test_thread,					//�߳̿��ƿ�
                   "test_thread",					//�߳̿��ƿ�����
                   test_thread_entry,				//�߳���ں���
                   RT_NULL,							//�߳���ں����Ĳ���
                   &test_thread_stack[0],			//�߳�ջ��ʼ��ַ
                   sizeof(test_thread_stack),		//�߳�ջ��С
                   TEST_THREAD_PRIORITY, 			//�߳����ȼ�
									 TEST_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
									 
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
//		if(val)//Ӳ���д���
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
		
		is_hdas_test_ok=1;		//�����Ѿ����
		
		hdas_thread_init();	//�����̣߳���ʼ����ϵͳ
		
		while(1)
		{
			HAL_GPIO_TogglePin(WDI_GPIO_Port,WDI_Pin);
			rt_thread_mdelay(200);
		}
		
	
}




int hdas_thread_init(void)
{		
	

				   
	rt_thread_init(&storage_thread,					//�߳̿��ƿ�
                   "storage_thread",					//�߳̿��ƿ�����
                   storage_thread_entry,				//�߳���ں���
                   RT_NULL,						//�߳���ں����Ĳ���
                   &storage_thread_stack[0],			//�߳�ջ��ʼ��ַ
                   sizeof(storage_thread_stack),		//�߳�ջ��С
                   STORAGE_THREAD_PRIORITY, 			//�߳����ȼ�
									 STORAGE_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
				   

				   
	rt_thread_init(&key1_thread,					//�߳̿��ƿ�
                   "key1_thread",					//�߳̿��ƿ�����
                   key1_thread_entry,				//�߳���ں���
                   RT_NULL,							//�߳���ں����Ĳ���
                   &key1_thread_stack[0],			//�߳�ջ��ʼ��ַ
                   sizeof(key1_thread_stack),		//�߳�ջ��С
                   KEY1_THREAD_PRIORITY, 			//�߳����ȼ�
									 KEY1_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
									 
	rt_thread_init(&key2_thread,					//�߳̿��ƿ�
                   "key2_thread",					//�߳̿��ƿ�����
                   key2_thread_entry,				//�߳���ں���
                   RT_NULL,							//�߳���ں����Ĳ���
                   &key2_thread_stack[0],			//�߳�ջ��ʼ��ַ
                   sizeof(key2_thread_stack),		//�߳�ջ��С
                   KEY2_THREAD_PRIORITY, 			//�߳����ȼ�
									 KEY2_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
				   

	rt_thread_init(&led1_thread,					//�߳̿��ƿ�
                   "led1_thread",					//�߳̿��ƿ�����
                   led1_thread_entry,				//�߳���ں���
                   RT_NULL,							//�߳���ں����Ĳ���
                   &led1_thread_stack[0],			//�߳�ջ��ʼ��ַ
                   sizeof(led1_thread_stack),		//�߳�ջ��С
                   LED1_THREAD_PRIORITY, 			//�߳����ȼ�
									 LED1_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
//	rt_thread_init(&led2_thread,					//�߳̿��ƿ�
//                   "led2_thread",					//�߳̿��ƿ�����
//                   led2_thread_entry,				//�߳���ں���
//                   RT_NULL,							//�߳���ں����Ĳ���
//                   &led2_thread_stack[0],			//�߳�ջ��ʼ��ַ
//                   sizeof(led2_thread_stack),		//�߳�ջ��С
//                   LED2_THREAD_PRIORITY, 			//�߳����ȼ�
//									 LED2_THREAD_TIMESLICE);		//�߳�ʱ��Ƭ��С
	
		
	rt_sem_init(&storage_sem,"storage_sem",0,RT_IPC_FLAG_FIFO);//�������ڴ洢���ݵ��ź���
									 
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
		//�ȴ��洢�ź���
		//DMA�����ʱ��������ռ��ǰ�벿�ֽ���sprintf�����б���
		//DMA���ʱ��������ռ�ĺ�벿�ֽ���sprintf�����б���
		
		rt_sem_control(&storage_sem,RT_IPC_CMD_RESET,RT_NULL);//�ȴ��ź���
		rt_sem_take(&storage_sem,RT_WAITING_FOREVER);
		
		if(DMAdat_flag==0)//DMA�������ɣ���ǰ�벿�ַ���SD������
		{
			sprintf_data_to_buf(0);//
			
		}
		else if(DMAdat_flag==1)//DMA����ȫ��ɣ�����벿�ַ���SD��������ͬʱ�����ļ��洢
		{
			
			sprintf_data_to_buf(1);
			
			retSD = f_write(&hdas_fil, FsWriteBuf, sizeof(FsWriteBuf), &byteswritten);	
			f_lseek(&hdas_fil, hdas_fil.fptr);											 
			
			signal_file_write_count++;
			if(signal_file_write_count==FS_WRITE_NUM)//д��������ˣ������ļ���С������328MB�����ȹر�ԭ�ļ������½��ļ�
			{
				signal_file_write_count=0;
				retSD = f_close(&hdas_fil);
				if(retSD){log_info("close error : %d\r\n",retSD);}
				//else{log_info("close sucess!!! \r\n");}
	
	
				if(Fatfa_GetFree_2()>=500)//1����ȡʣ������,��������������ļ�������1�����½�һ���ļ�
				{
					uint32_t len=0;
					
					file_number++;
					
					len=sprintf(hdas_filename,"hdas_data%d.txt",file_number);
					retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
					if(retSD) {log_info("open file error : %d\r\n",retSD);}		
					else {log_info("Creat file:hdas_data%d.txt",file_number);}
				}
				else											//2�����ʣ��������������ֹͣ������ͬʱ�ر��ļ���ж��SD��
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
				
				
				MX_USB_DEVICE_DeInit();	//�ر�USB
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
				
				
				AD7606_INIT();			 //��ʼ�ɼ�������SD������ʽ��U�̣���ʼǰ��λ���в���������ʼ��DMA��TIM��AD��
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
				
				AD7606_Stop();//ֹͣ�ɼ���ж��SD��
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
				MX_USB_DEVICE_Init();//��ʼ��USB
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
	if(is_hdas_test_ok==1)//Ӳ���������
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
				
				
				AD7606_INIT();			 //��ʼ�ɼ�������SD������ʽ��U�̣���ʼǰ��λ���в���������ʼ��DMA��TIM��AD��
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
				
			AD7606_Stop();//ֹͣ�ɼ���ж��SD��
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


//void sample(int argc, char **argv)//push on 1 10    [����,����,����]
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










