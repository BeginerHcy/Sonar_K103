#ifndef __SYSTICK_H
#define __SYSTICK_H

#include "stm32f10x.h"

void SysTick_Init(void);
void Delay_us(__IO u32 nTime);

void TimingDelay_Decrement(void);

void delay_us(u32 nus);
void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
#endif /* __SYSTICK_H */
