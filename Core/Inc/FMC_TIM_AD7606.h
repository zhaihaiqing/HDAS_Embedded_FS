/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FMC_TIM_AD7606_H
#define __FMC_TIM_AD7606_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* USER CODE BEGIN EC */
#define RANGE_GPIO GPIOJ
#define RANGE_PIN GPIO_PIN_4

#define OS0_GPIO GPIOJ
#define OS0_PIN GPIO_PIN_3

#define OS1_GPIO GPIOJ
#define OS1_PIN GPIO_PIN_2

#define OS2_GPIO GPIOB
#define OS2_PIN GPIO_PIN_2

#define ADRST_GPIO GPIOC
#define ADRST_PIN GPIO_PIN_5

/* 设置过采样的IO, 在扩展的74HC574上 */
#define OS0_1() OS0_GPIO->BSRR = OS0_PIN
#define OS0_0() OS0_GPIO->BSRR = ((uint32_t)OS0_PIN << 16U)
#define OS1_1() OS1_GPIO->BSRR = OS1_PIN
#define OS1_0() OS1_GPIO->BSRR = ((uint32_t)OS1_PIN << 16U)
#define OS2_1() OS2_GPIO->BSRR = OS2_PIN
#define OS2_0() OS2_GPIO->BSRR = ((uint32_t)OS2_PIN << 16U)

/* 设置输入量程的GPIO, 在扩展的74HC574上 */
#define RANGE_1() RANGE_GPIO->BSRR = RANGE_PIN
#define RANGE_0() RANGE_GPIO->BSRR = ((uint32_t)RANGE_PIN << 16U)

/* AD7606复位口线, 在扩展的74HC574上 */
#define RESET_1() ADRST_GPIO->BSRR = ADRST_PIN
#define RESET_0() ADRST_GPIO->BSRR = ((uint32_t)ADRST_PIN << 16U)
/* 过采样倍率 */
typedef enum
{
	AD_OS_NO = 0,
	AD_OS_X2 = 1,
	AD_OS_X4 = 2,
	AD_OS_X8 = 3,
	AD_OS_X16 = 4,
	AD_OS_X32 = 5,
	AD_OS_X64 = 6
}AD7606_OS_E;
/* AD7606 FSMC总线地址，只能读，无需写 */
#define AD7606_BASE 0x60003000

	/* 8路同步采集，每次采集16字节数据，防止DMA突发方式1KB边界问题，即每次采集不要有跨边界的情况 */
//#define AD7606_BUFSIZE 1600
//#define AD7606_BUFSIZE DATA_NUM*8		//DMA单次最大传输16*2KB数据，共计DATA_NUM*2组数据


//extern __align(16) int16_t g_sAd7606Buf[AD7606_BUFSIZE];
//extern int16_t g_sAd7606Buf[AD7606_BUFSIZE];
void AD7606_SetOS(uint8_t _ucOS);
void AD7606_SetInputRange(uint8_t _ucRange);
void AD7606_Reset(void);
void AD7606_DmaCplCb(DMA_HandleTypeDef *hdma);
void AD7606_DmaHalfCplCb(DMA_HandleTypeDef *hdma);
void AD7606_INIT(void);
void AD7606_Start(void);
void AD7606_Stop(void);
	


#endif 

