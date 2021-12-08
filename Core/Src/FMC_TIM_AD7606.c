
/* Includes ------------------------------------------------------------------*/
#include "FMC_TIM_AD7606.h"
#include "tim.h"


/* USER CODE BEGIN PV */

//__align(16) int16_t g_sAd7606Buf[AD7606_BUFSIZE];

//ALIGN_32BYTES(__attribute__((section (".RAM_D1"))) int16_t g_sAd7606Buf[AD7606_BUFSIZE]);

__align(16) int16_t g_sAd7606Buf[AD7606_BUFSIZE];

/* DMA������ɻص����� */
void AD7606_DmaCplCb(DMA_HandleTypeDef *hdma)
{
	int i=0;
	DMAdat_flag=1;
	rt_sem_release(&storage_sem);	//�����ź��������
	
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

/* DMA�봫����ɻص����� */
void AD7606_DmaHalfCplCb(DMA_HandleTypeDef *hdma)
{
	DMAdat_flag=0;
	rt_sem_release(&storage_sem);	//�����ź��������
	
	//HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
}

void AD7606_INIT(void)
{
	AD7606_SetOS(AD_OS_NO);	 /* �޹����� */
	AD7606_SetInputRange(0); /* 0��ʾ��������Ϊ����5V, 1��ʾ����10V */
	AD7606_Reset();			 /* ��λ */
}

/*
*********************************************************************************************************
* �� �� ��: AD7606_SetOS
* ����˵�� : ����AD7606�����˲�����Ҳ�����ù��������ʡ�
*			  ͨ������ AD7606_OS0��OS1��OS2���ߵĵ�ƽ���״̬�������������ʡ�
*			  ����ADת��֮��AD7606�ڲ��Զ�ʵ��ʣ�������Ĳɼ���Ȼ����ƽ��ֵ�����
*
*			  ����������Խ�ߣ�ת��ʱ��Խ����
* 0���޹�����ʱ��ADת��ʱ�� = 3.45us - 4.15us
* 1��2��������ʱ       = 7.87us - 9.1us
* 2��4��������ʱ       = 16.05us - 18.8us
* 3��8��������ʱ       = 33us - 39us
* 4��16��������ʱ      = 66us - 78us
* 5��32��������ʱ      = 133us - 158us
* 6��64��������ʱ      = 257us - 315us
*
* ��    ��: _ucOS: ����������, 0 - 6
* �� �� ֵ : ��
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
* �� �� �� : AD7606_SetInputRange
* ����˵��  : ����AD7606ģ���ź��������̡�
* ��    ��: _ucRange: 0 ��ʾ����5V   1��ʾ����10V
* �� �� ֵ : ��
*********************************************************************************************************
*/
void AD7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		RANGE_0(); /* ����Ϊ����5V */
	}
	else
	{
		RANGE_1(); /* ����Ϊ����10V */
	}
}

/*
*********************************************************************************************************
* �� �� �� : AD7606_Reset
* ����˵��  : Ӳ����λAD7606����λ֮��ָ�����������״̬��
* ��    ��: ��
* �� �� ֵ : ��
*********************************************************************************************************
*/
void AD7606_Reset(void)
{
	RESET_0(); /* �˳���λ״̬ */

	RESET_1(); /* ���븴λ״̬ */
	RESET_1(); /* �������ӳ١� RESET��λ�ߵ�ƽ��������С50ns�� */
	RESET_1();
	RESET_1();

	RESET_0(); /* �˳���λ״̬ */
}

void AD7606_Start(void)
{
	Tim2_pwm_enable();
}

void AD7606_Stop(void)
{
	Tim2_pwm_disable();
}
