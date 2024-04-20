#ifndef __LED_H
#define	__LED_H

#include "stm32f10x.h"

#include "delay.h"


/* the macro definition to trigger the led on or off 
 * 1 - off
 - 0 - on
 */
#define ON  1
#define OFF 0

#define LED1(a)	if (a)	\
					GPIO_SetBits(GPIOC,GPIO_Pin_13);\
					else		\
					GPIO_ResetBits(GPIOC,GPIO_Pin_13)

//#define LED2(a)	if (a)	\
//					GPIO_SetBits(GPIOB,GPIO_Pin_14);\
//					else		\
//					GPIO_ResetBits(GPIOB,GPIO_Pin_14)



void LED_GPIO_Config(void);
					
void LED_Flash(uint32_t time);


#endif /* __LED_H */
