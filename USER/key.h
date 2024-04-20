#ifndef __KEY_H
#define	__KEY_H

#include "stm32f10x.h"

 /*******
 *按键按下标置
 KEY_ON 0
 KEY_OFF 1
 ********/
#define KEY_ON	0
#define KEY_OFF	1

void Key_GPIO_Config(void);
void Key_NVIC_Config(void);
void EXTI_PC0_Config(void);

void Key_Config(void);

u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

extern void Delay_us(__IO u32 nTime);


//u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin);

#endif /* __LED_H */

