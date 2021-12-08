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
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* USER CODE BEGIN 0 */
#include "sdmmc.h"
#include "fatfs.h"
#include "sdmmc_fatfs.h"

//测试文件
#define TEST_FILE_LEN			(4*1024*1024)	/* 用于测试的文件长度 */
#define TEST_BUF_SIZE					(16*1024)		/* 每次读写SD卡的最大数据长度 */


//

FATFS hdas_fs; // Work area (file system object) for logical drive 
FIL hdas_fil; // file objects 

uint32_t byteswritten; /* File write counts */ 
uint32_t bytesread; /* File read counts */ 

//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t g_TestBuf[TEST_BUF_SIZE]);			//测试缓冲区

ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t FsReadBuf[1024]);					//读数据缓冲空间
//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) int16_t FsWriteBuf_A[5120*8*2]); //双缓冲的A
//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) uint8_t FsWriteBuf_B[DATA_BUFSIZE]);	//双缓冲的B


ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) char FsWriteBuf[DATA_NUM][DATA_LEN_SIGNAL]);



//DAT_ascii_type dat_ascii[DATA_NUM];

char hdas_filename[64];

extern int16_t g_sAd7606Buf[AD7606_BUFSIZE];



//计算剩余容量
//查找最后一个文件标号，以便新建文件时不覆盖之前的文件


//void Fatfa_GetFree(void)
//{
//	FATFS *fatfs;
//	
//	DWORD fre_clust, fre_sect, tot_sect;
//	float total_size=0,free_size=0;
//	
//	retSD = f_mount(fatfs, "", 1);
//	if(retSD){log_info("mount error : %d \r\n",retSD);Error_Handler();}
//	else{log_info("mount sucess!!! \r\n");}
//	
//	
//	retSD = f_getfree("", &fre_clust, &fatfs);
//	
//	tot_sect = (fatfs->n_fatent - 2) * fatfs->csize;
//	fre_sect = fre_clust * fatfs->csize;
//	
//	total_size=(1.0*tot_sect/2)/1024;
//	free_size=(1.0*fre_sect/2)/1024;
//	
//	Sys_Status.SD_CardCap=total_size;
//	Sys_Status.SD_FreeCap=free_size;
//	
//	log_info("Total space:%dMB Available space:%dMB\r\n", (uint32_t)Sys_Status.SD_CardCap, (uint32_t)Sys_Status.SD_FreeCap);
//	
//	f_mount(NULL, "", 1);
//	
//}

/*

*/
uint32_t Fatfa_GetFree_2(void)//文件系统已经存在
{
	FATFS *fs;
	
	DWORD fre_clust, fre_sect, tot_sect;
	float total_size=0,free_size=0;
	
	
	retSD = f_getfree("", &fre_clust, &fs);
	
	if(retSD)
	{
		return 0;
	}
	
	
	tot_sect = (fs->n_fatent - 2) * fs->csize;
	fre_sect = fre_clust * fs->csize;
	
	total_size=(1.0*tot_sect/2)/1024;
	free_size=(1.0*fre_sect/2)/1024;
	
	Sys_Status.SD_CardCap=total_size;
	Sys_Status.SD_FreeCap=free_size;
	
	log_info("SD Total space:%dMB Available space:%dMB ", (uint32_t)Sys_Status.SD_CardCap, (uint32_t)Sys_Status.SD_FreeCap);
	
	return (uint32_t)free_size;
	
}



/*	工作流程
		1：上电
		2：完成各项参数初始化，硬件，自检查项，文件系统初始化话，并对写入数据进行比对，写入速度进行测试（大于3.2MB/S），ADC初始化等，左侧指示灯闪烁，
		3：等待开始采样命令/按键，右侧指示灯开始闪烁
		4：完成文件系统格式化（fat32）
		5：开始采集,数据采集时，采用双缓冲机制，系统定义两个205KB的缓冲区，采集数据首先向A缓冲区存放，A缓冲区存满之后，立即向B缓冲区存数据，同时开始文件的写入（DMA），A\B依次交替进行数据存储。
		6：如果检测到停止按钮、SD卡剩余空间不足500M、SD卡被取下，则停止采样、停止记录数据，并同频次、同相位闪烁双绿灯
*/




uint8_t Fatfs_RW_test(void)
{
	//创建文件
	//测试写入速度
	//判断卡类型
	//判断数据是否正确
	//返回成功
	
	uint32_t systick=0,len=0;
	char lrcr[2]="\r\n";
	short ch[8]={3,65,888,1999,5555,6066,6115,3276};//所采集的数据放大1000倍，再加3276
	float speed=0;
	BYTE work[_MAX_SS];
	
	//log_info("dat_ascii_len:%d\r\n",sizeof(dat_ascii));
	//log_info("FsWriteBuf_A len:%d\r\n",sizeof(FsWriteBuf_A));
	//log_info("FsWriteBuf len:%d\r\n",sizeof(FsWriteBuf));
	
	log_info("\r\n ****** FatFs Test ******\r\n\r\n");
	clc_watchdog();
	/*##1 Register the file system object to the FatFs module ##############*/
	retSD = f_mount(&hdas_fs, "", 1);
	if(retSD)
	{
		log_info("mount error : %d \r\n",retSD);
		Sys_Status.fatfs=1;
		return HAL_ERROR;
		//Error_Handler();
	}
	else{log_info("mount sucess!!! \r\n");}
	
	clc_watchdog();
	
	/*##2 f_mkfs 32KB unit,FAT32 ##############*/
//	retSD=f_mkfs("",FM_FAT32,SDCARD_UNIT_SIZE,work,sizeof(work));
//	if(retSD) 
//	{
//		log_info("f_mkfs error : %d\r\n",retSD);
//		Sys_Status.fatfs=2;
//		return HAL_ERROR;
//	}		
//	else 
//	{
//		log_info("f_mkfs sucess!!! \r\n");
//	}
//	
	/*##3 Create and Open new text file objects with write access ######*/
	clc_watchdog();
	len=sprintf(hdas_filename,"STM32cube_%d.txt",1);
	retSD = f_open(&hdas_fil, hdas_filename, FA_CREATE_ALWAYS | FA_WRITE);
	clc_watchdog();
	if(retSD) {log_info("open file error : %d\r\n",retSD);}		
	else {log_info("open file sucess!!! \r\n");}
	
	{
		for(int i=0;i<DATA_NUM;i++)
		{
			len=sprintf(&FsWriteBuf[i][0],"%4d %4d %4d %4d %4d %4d %4d %4d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
		}
	}
	
	systick=rt_tick_get();
	for (uint32_t i = 0; i < 10; i++)
	{
		retSD = f_write(&hdas_fil, FsWriteBuf, sizeof(FsWriteBuf), &byteswritten);
		f_lseek(&hdas_fil, hdas_fil.fptr);

	}
	clc_watchdog();
	systick=(rt_tick_get()-systick)/10;
	speed=(1.0*sizeof(FsWriteBuf)*1000)/1024/1024/systick;
	log_info("retSD:%d,time:%dms,speed:%.2fMB/s\r\n",retSD,systick,speed);
	if(systick>50)
	{
		log_info("card err\r\n");
		Sys_Status.fatfs=3;
		return HAL_ERROR;
	}
	else
	{
		log_info("card ok\r\n");
	}
	//speed=1.0*((FILE_SIZE/1024/1024) * 1000) / systick;
	//log_info("retSD:%d,used time:%dms,signal wtext_len:%.3fMB,write speed:%.3fMB/s\r\n",retSD,systick,1.0*FILE_SIZE/1024/1024,speed); 
	
	
	/*##4 Close the open text files ################################*/
	retSD = f_close(&hdas_fil);
	if(retSD){log_info("close error : %d\r\n",retSD);}
	else{log_info("close sucess!!! \r\n");}
	
	
	f_mount(NULL, "", 1);
	
	
	
}



//向SD缓冲区中打印数据，0代表前半部分，1代表后半部分
void sprintf_data_to_buf(uint8_t flag)
{
	uint32_t i=0,j=0;
	uint16_t ch[8]={0};
	uint32_t len=0;
	uint32_t tick=0;
	
	if(flag==0)
	{
		//tick=rt_tick_get();
		for(i=0;i<DATA_NUM/2;i++)
		{
			for(j=0;j<8;j++)
			{
				ch[j]=(g_sAd7606Buf[i*8+j]*1112*5/32767)+3276;
			}
		
			//len=sprintf(&FsWriteBuf[i][0],"%4d %4d %4d %4d %4d %4d %4d %4d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
			
			 FsWriteBuf[i][0]=ch[0]/1000+0x30;
       FsWriteBuf[i][1]=(ch[0]/100)%10+0x30;
       FsWriteBuf[i][2]=(ch[0]/10)%10+0x30;
       FsWriteBuf[i][3]=(ch[0]%10)+0x30;

       FsWriteBuf[i][4]=0x20;

       FsWriteBuf[i][5]=ch[1]/1000+0x30;
			 FsWriteBuf[i][6]=(ch[1]/100)%10+0x30;
			 FsWriteBuf[i][7]=(ch[1]/10)%10+0x30;
			 FsWriteBuf[i][8]=(ch[1]%10)+0x30;

			 FsWriteBuf[i][9]=0x20;

			 FsWriteBuf[i][10]=ch[2]/1000+0x30;
			 FsWriteBuf[i][11]=(ch[2]/100)%10+0x30;
			 FsWriteBuf[i][12]=(ch[2]/10)%10+0x30;
			 FsWriteBuf[i][13]=(ch[2]%10)+0x30;

			 FsWriteBuf[i][14]=0x20;

			 FsWriteBuf[i][15]=ch[3]/1000+0x30;
			 FsWriteBuf[i][16]=(ch[3]/100)%10+0x30;
			 FsWriteBuf[i][17]=(ch[3]/10)%10+0x30;
			 FsWriteBuf[i][18]=(ch[3]%10)+0x30;

			 FsWriteBuf[i][19]=0x20;

			 FsWriteBuf[i][20]=ch[4]/1000+0x30;
			 FsWriteBuf[i][21]=(ch[4]/100)%10+0x30;
			 FsWriteBuf[i][22]=(ch[4]/10)%10+0x30;
			 FsWriteBuf[i][23]=(ch[4]%10)+0x30;

			 FsWriteBuf[i][24]=0x20;

			 FsWriteBuf[i][25]=ch[5]/1000+0x30;
			 FsWriteBuf[i][26]=(ch[5]/100)%10+0x30;
			 FsWriteBuf[i][27]=(ch[5]/10)%10+0x30;
			 FsWriteBuf[i][28]=(ch[5]%10)+0x30;

			 FsWriteBuf[i][29]=0x20;

			 FsWriteBuf[i][30]=ch[6]/1000+0x30;
			 FsWriteBuf[i][31]=(ch[6]/100)%10+0x30;
			 FsWriteBuf[i][32]=(ch[6]/10)%10+0x30;
			 FsWriteBuf[i][33]=(ch[6]%10)+0x30;

			 FsWriteBuf[i][34]=0x20;

			 FsWriteBuf[i][35]=ch[7]/1000+0x30;
			 FsWriteBuf[i][36]=(ch[7]/100)%10+0x30;
			 FsWriteBuf[i][37]=(ch[7]/10)%10+0x30;
			 FsWriteBuf[i][38]=(ch[7]%10)+0x30;

			 FsWriteBuf[i][39]=0x0d;
			 FsWriteBuf[i][40]=0x0a;
			
		}
		//tick=rt_tick_get()-tick;
		//log_info("tick:%dms\r\n",tick);
	}
	else
	{
		for(i=DATA_NUM/2;i<DATA_NUM;i++)
		{
			for(j=0;j<8;j++)
			{
				ch[j]=(g_sAd7606Buf[i*8+j]*1112*5/32767)+3276;
			}
		
			//len=sprintf(&FsWriteBuf[i][0],"%4d %4d %4d %4d %4d %4d %4d %4d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
			
			 FsWriteBuf[i][0]=ch[0]/1000+0x30;
       FsWriteBuf[i][1]=(ch[0]/100)%10+0x30;
       FsWriteBuf[i][2]=(ch[0]/10)%10+0x30;
       FsWriteBuf[i][3]=(ch[0]%10)+0x30;

       FsWriteBuf[i][4]=0x20;

       FsWriteBuf[i][5]=ch[1]/1000+0x30;
			 FsWriteBuf[i][6]=(ch[1]/100)%10+0x30;
			 FsWriteBuf[i][7]=(ch[1]/10)%10+0x30;
			 FsWriteBuf[i][8]=(ch[1]%10)+0x30;

			 FsWriteBuf[i][9]=0x20;

			 FsWriteBuf[i][10]=ch[2]/1000+0x30;
			 FsWriteBuf[i][11]=(ch[2]/100)%10+0x30;
			 FsWriteBuf[i][12]=(ch[2]/10)%10+0x30;
			 FsWriteBuf[i][13]=(ch[2]%10)+0x30;

			 FsWriteBuf[i][14]=0x20;

			 FsWriteBuf[i][15]=ch[3]/1000+0x30;
			 FsWriteBuf[i][16]=(ch[3]/100)%10+0x30;
			 FsWriteBuf[i][17]=(ch[3]/10)%10+0x30;
			 FsWriteBuf[i][18]=(ch[3]%10)+0x30;

			 FsWriteBuf[i][19]=0x20;

			 FsWriteBuf[i][20]=ch[4]/1000+0x30;
			 FsWriteBuf[i][21]=(ch[4]/100)%10+0x30;
			 FsWriteBuf[i][22]=(ch[4]/10)%10+0x30;
			 FsWriteBuf[i][23]=(ch[4]%10)+0x30;

			 FsWriteBuf[i][24]=0x20;

			 FsWriteBuf[i][25]=ch[5]/1000+0x30;
			 FsWriteBuf[i][26]=(ch[5]/100)%10+0x30;
			 FsWriteBuf[i][27]=(ch[5]/10)%10+0x30;
			 FsWriteBuf[i][28]=(ch[5]%10)+0x30;

			 FsWriteBuf[i][29]=0x20;

			 FsWriteBuf[i][30]=ch[6]/1000+0x30;
			 FsWriteBuf[i][31]=(ch[6]/100)%10+0x30;
			 FsWriteBuf[i][32]=(ch[6]/10)%10+0x30;
			 FsWriteBuf[i][33]=(ch[6]%10)+0x30;

			 FsWriteBuf[i][34]=0x20;

			 FsWriteBuf[i][35]=ch[7]/1000+0x30;
			 FsWriteBuf[i][36]=(ch[7]/100)%10+0x30;
			 FsWriteBuf[i][37]=(ch[7]/10)%10+0x30;
			 FsWriteBuf[i][38]=(ch[7]%10)+0x30;

			 FsWriteBuf[i][39]=0x0d;
			 FsWriteBuf[i][40]=0x0a;
		}
		
		//log_info("dat:%d,%d,%d,%d,%d,%d,%d,%d\r\n",ch[0],ch[1],ch[2],ch[3],ch[4],ch[5],ch[6],ch[7]);
	}
}











//void Fatfs_RW_test2(void)
//{
//	uint32_t systick=0,len=0;
//	char lrcr[2]="\r\n";
//	short ch[8]={3,65,888,1999,5555,6066,6115,3276};//所采集的数据放大1000倍，再加3276
//	float speed=0;
//	
//	//log_info("dat_ascii_len:%d\r\n",sizeof(dat_ascii));
//	//log_info("FsWriteBuf_A len:%d\r\n",sizeof(FsWriteBuf_A));
//	log_info("FsWriteBuf len:%d\r\n",sizeof(FsWriteBuf));
//	
//	log_info("\r\n ****** FatFs Example ******\r\n\r\n");
//	/*##1 Register the file system object to the FatFs module ##############*/
//	retSD = f_mount(&fs, "", 1);
//	if(retSD){log_info(" mount error : %d \r\n",retSD);Error_Handler();}
//	else{log_info(" mount sucess!!! \r\n");}
//	
//	/*##2 Create and Open new text file objects with write access ######*/
//	retSD = f_open(&fil, filename, FA_CREATE_ALWAYS | FA_WRITE);
//	if(retSD) {log_info(" open file error : %d\r\n",retSD);}		
//	else {log_info(" open file sucess!!! \r\n");}
//	
//	
//	for (uint32_t i = 0; i < sizeof(g_TestBuf); i++)
//	{
//		g_TestBuf[i] = (i / 512) + '0';
//	}
//	
//	
//	systick=HAL_GetTick();
//	for (uint32_t i = 0; i < TEST_FILE_LEN / TEST_BUF_SIZE; i++)
//	{
//		retSD = f_write(&fil, g_TestBuf, sizeof(g_TestBuf), &byteswritten);
//		if (retSD == FR_OK)
//		{
//			if (((i + 1) % 8) == 0)
//			{
//				//log_info(".");
//			}
//		}
//		else
//		{
//			//log_info("文件写失败\r\n");
//			break;
//		}
//	}
//	
//	systick=HAL_GetTick()-systick;
//	//speed=1.0*sizeof(dat_ascii[0].dat)*buf_size*7/1024/1024/(systick/1000.0);
//	speed=1.0*((TEST_FILE_LEN / 1024 / 1024) * 1000) / systick;
//	log_info("retSD:%d,used time:%dms,signal wtext_len:%.3fKB,write speed:%.3fMB/s\r\n",retSD,systick,1.0*TEST_FILE_LEN/1024,speed); 
//	
//	
//	/*##4 Close the open text files ################################*/
//	retSD = f_close(&fil);
//	if(retSD){log_info(" close error : %d\r\n",retSD);}
//	else{log_info(" close sucess!!! \r\n");}
//	
//	f_mount(NULL, "", 1);
//}


/* USER CODE END 0 */




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
