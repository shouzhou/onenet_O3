#ifndef USART1_H_H
#define USART1_H_H
#include "stm32f10x.h"
/*
 *  @brief USART1初始化函数
 */
void USART1_Init(void);
void USART1_Write(USART_TypeDef* USARTx, uint8_t *Data, uint32_t len);

#endif

