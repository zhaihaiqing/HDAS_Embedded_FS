/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define EMPTY 	0xFFFF
#define TIMEOUT 0xFFFF

#define UART8_RBUF_SIZE   128		//要求：2 的整次幂
typedef struct uart8_rbuf_st
{
	unsigned int 		in;							//输入
	unsigned int 		out;							//输出
	unsigned char 	buf [UART8_RBUF_SIZE];		//缓冲区空间
}UART8_RBUF_ST;


#define UART_GetChar(x)	(uart##x##_rbuf.in == uart##x##_rbuf.out) ? EMPTY : uart##x##_rbuf.buf[(uart##x##_rbuf.out++) & (UART##x##_RBUF_SIZE - 1)]
#define LOG_UART_GetChar()    	UART_GetChar(8)
extern UART8_RBUF_ST	uart8_rbuf;


#define	UART_RxChar(x)	UART##x##_RBUF_ST *p = &uart##x##_rbuf;\
												d = *(--huart->pRxBuffPtr);\
												if((p->in - p->out)<UART##x##_RBUF_SIZE)\
												{\
														p->buf [p->in & (UART##x##_RBUF_SIZE-1)] = d;	\
														p->in++;\
												}\
												HAL_UART_Receive_IT(&huart##x,&uart##x##_temp,1);

												
												
												
#define  debug_log

#ifdef debug_log

   #define  log_info(...) printf(__VA_ARGS__)

#else
   #define  log_info(...)
#endif
												
												
												
												

extern UART_HandleTypeDef huart8;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART8_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
