
/* Includes ------------------------------------------------------------------*/
#include "FMC_TIM_AD7606.h"
#include "tim.h"


/* USER CODE BEGIN PV */

//__align(16) int16_t g_sAd7606Buf[AD7606_BUFSIZE];

//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) int16_t g_sAd7606Buf[AD7606_BUFSIZE]);

__align(16) int16_t g_sAd7606Buf[AD7606_BUFSIZE];

/* DMA传输完成回调函数 */
void AD7606_DmaCplCb(DMA_HandleTypeDef *hdma)
{
	int i=0;
	DMAdat_flag=1;
	rt_sem_release(&storage_sem);	//发送信号量，完成
	
//	for(i=0;i<2048;i++)
//	i=0;
//		log_info("%d:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\r\n",i,g_sAd7606Buf[i*8+0],g_sAd7606Buf[i*8+1],g_sAd7606Buf[i*8+2],g_sAd7606Buf[i*8+3],g_sAd7606Buf[i*8+4],g_sAd7606Buf[i*8+5],g_sAd7606Buf[i*8+6],g_sAd7606Buf[i*8+7]);
//	i=3852;
//		log_info("%d:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\r\n",i,g_sAd7606Buf[i*8+0],g_sAd7606Buf[i*8+1],g_sAd7606Buf[i*8+2],g_sAd7606Buf[i*8+3],g_sAd7606Buf[i*8+4],g_sAd7606Buf[i*8+5],g_sAd7606Buf[i*8+6],g_sAd7606Buf[i*8+7]);
//	i=7948;
//		log_info("%d:0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\r\n",i,g_sAd7606Buf[i*8+0],g_sAd7606Buf[i*8+1],g_sAd7606Buf[i*8+2],g_sAd7606Buf[i*8+3],g_sAd7606Buf[i*8+4],g_sAd7606Buf[i*8+5],g_sAd7606Buf[i*8+6],g_sAd7606Buf[i*8+7]);
	//log_info("%d,%d\r\n",g_sAd7606Buf[7],g_sAd7606Buf[3582*8+7]);
	LED2_TOGGLE();
	
	
}

/* DMA半传输完成回调函数 */
void AD7606_DmaHalfCplCb(DMA_HandleTypeDef *hdma)
{
	DMAdat_flag=0;
	rt_sem_release(&storage_sem);	//发送信号量，完成
	
	//HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
}

void AD7606_INIT(void)
{
	AD7606_SetOS(AD_OS_NO);	 /* 无过采样 */
	AD7606_SetInputRange(0); /* 0表示输入量程为正负5V, 1表示正负10V */
	AD7606_Reset();			 /* 复位 */
}

/*
*********************************************************************************************************
* 函 数 名: AD7606_SetOS
* 功能说明 : 配置AD7606数字滤波器，也就设置过采样倍率。
*			  通过设置 AD7606_OS0、OS1、OS2口线的电平组合状态决定过采样倍率。
*			  启动AD转换之后，AD7606内部自动实现剩余样本的采集，然后求平均值输出。
*
*			  过采样倍率越高，转换时间越长。
* 0、无过采样时，AD转换时间 = 3.45us - 4.15us
* 1、2倍过采样时       = 7.87us - 9.1us
* 2、4倍过采样时       = 16.05us - 18.8us
* 3、8倍过采样时       = 33us - 39us
* 4、16倍过采样时      = 66us - 78us
* 5、32倍过采样时      = 133us - 158us
* 6、64倍过采样时      = 257us - 315us
*
* 形    参: _ucOS: 过采样倍率, 0 - 6
* 返 回 值 : 无
*********************************************************************************************************
*/
void AD7606_SetOS(uint8_t _ucOS)
{
	switch (_ucOS)
	{
	case AD_OS_X2: 
		OS2_0();
		OS1_0();
		OS0_1();
		break;

	case AD_OS_X4: 
		OS2_0();
		OS1_1();
		OS0_0();
		break;

	case AD_OS_X8: 
		OS2_0();
		OS1_1();
		OS0_1();
		break;

	case AD_OS_X16: 
		OS2_1();
		OS1_0();
		OS0_0();
		break;

	case AD_OS_X32: 
		OS2_1();
		OS1_0();
		OS0_1();
		break;

	case AD_OS_X64: 
		OS2_1();
		OS1_1();
		OS0_0();
		break;

	case AD_OS_NO: 
	     default : 
		OS2_0();
		OS1_0();
		OS0_0();
		break;
	}
}

/*
*********************************************************************************************************
* 函 数 名 : AD7606_SetInputRange
* 功能说明  : 配置AD7606模拟信号输入量程。
* 形    参: _ucRange: 0 表示正负5V   1表示正负10V
* 返 回 值 : 无
*********************************************************************************************************
*/
void AD7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		RANGE_0(); /* 设置为正负5V */
	}
	else
	{
		RANGE_1(); /* 设置为正负10V */
	}
}

/*
*********************************************************************************************************
* 函 数 名 : AD7606_Reset
* 功能说明  : 硬件复位AD7606，复位之后恢复到正常工作状态。
* 形    参: 无
* 返 回 值 : 无
*********************************************************************************************************
*/
void AD7606_Reset(void)
{
	RESET_0(); /* 退出复位状态 */

	RESET_1(); /* 进入复位状态 */
	RESET_1(); /* 仅用于延迟。 RESET复位高电平脉冲宽度最小50ns。 */
	RESET_1();
	RESET_1();

	RESET_0(); /* 退出复位状态 */
}

void AD7606_Start(void)
{
	Tim2_pwm_enable();
}

void AD7606_Stop(void)
{
	Tim2_pwm_disable();
}
