#ifndef __RS485_H
#define __RS485_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include "crc16.h"
#include "usart.h"

/* �����жϽ��յ����ݵĳ��ȱ��� */
extern uint8_t USART_RX_CNT;
/* �����жϽ��յ����ݱ���Ļ����� */
extern uint8_t USART2_RX_BUF[255];

void RS485_RW_Opr(uint8_t ucAddr,uint8_t ucCmd,uint16_t ucReg,uint16_t uiDate);
void UartDriver(void);
void UartRxMonitor(uint8_t ms);
void USART2_IRQHandler(void);




#endif

