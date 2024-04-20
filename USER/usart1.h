#ifndef __USART1_H
#define	__USART1_H

#define ControlCenter_Address 0X52
#define Tellephone1_Address 0X4D

#define YES	1
#define NO	0


#include "stm32f10x.h"
#include <stdio.h>

void USART1_Config(void);
void UART1Test(void);

void Rx_Data_Init(void);


int fputc(int ch, FILE *f);


void Uart_Send_Buffer(uint8_t *buffer,uint8_t bufLen);

u8 Send_DATA_Cheak(void);

extern uint8_t RX_Data[10];

extern uint8_t SendBuf [10] ;

extern uint8_t RX_Data[10];

extern int All_AGV_Number ;
extern uint8_t AGV_Address_Ori ;
extern uint8_t AGV_Address ;




extern void Delay_us(__IO u32 nTime);


#endif /* __USART1_H */
