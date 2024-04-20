#ifndef __HW_CFG_H__
#define __HW_CFG_H__

    #include "stm32f10x.h"
    #include "stm32f10x_gpio.h"
    #include "string.h"
    #include "stm32f10x_spi.h"


    /////define regeisters//////
    ////Redefine
    #define PxA GPIOA_BASE
    #define PxB GPIOB_BASE
    #define PxC GPIOC_BASE
    #define PxD GPIOD_BASE

    #define Uartx2 USART2_BASE
    #define Uartx3 USART3_BASE

    #define TIMx3  TIM3_BASE///ʹ��TIM3��Ϊ1ms�ж�Դ
    ///////////
    #define PIN0 	GPIO_Pin_0
    #define PIN1 	GPIO_Pin_1
    #define PIN2 	GPIO_Pin_2
    #define PIN3 	GPIO_Pin_3
    #define PIN4 	GPIO_Pin_4
    #define PIN5 	GPIO_Pin_5
    #define PIN6 	GPIO_Pin_6
    #define PIN7 	GPIO_Pin_7
    #define PIN8 	GPIO_Pin_8
    #define PIN9 	GPIO_Pin_9
    #define PIN10 GPIO_Pin_10
    #define PIN11 GPIO_Pin_11
    #define PIN12 GPIO_Pin_12
    #define PIN13 GPIO_Pin_13
    #define PIN14 GPIO_Pin_14
    #define PIN15 GPIO_Pin_15

    ///////////MAPING
    #define DoBuzzer PxC,PIN0
    #define DO8 PxB,PIN1
    #define DO7 PxB,PIN0
    #define DO6 PxC,PIN5
    #define DO5 PxC,PIN4
    #define DO4 PxA,PIN7
    #define DO3 PxA,PIN6
    #define DO2 PxA,PIN5
    #define DO1 PxA,PIN4

    #define DI8 PxC,PIN11
    #define DI7 PxC,PIN12
    #define DI6 PxD,PIN2
    #define DI5 PxB,PIN3
    #define DI4 PxB,PIN7
    #define DI3 PxB,PIN6
    #define DI2 PxB,PIN5
    #define DI1 PxB,PIN4
		
		
		#define LED1 PxA,PIN8
		#define LED2 PxC,PIN9
		#define LED3 PxC,PIN8
		#define LED4 PxC,PIN7
		#define LED5 PxC,PIN6


    ///////////////////
    #define Uart3TX PxB,PIN10
    #define Uart3RX PxB,PIN11
    ///////////////////
    #define Uart2TX PxA,PIN2
    #define Uart2RX PxA,PIN3
    #define Uart2RDE PxA,PIN1
    ///////////////////

    ///////////////////////////////////////

    ////////////////
    void SystemConfig();
    uint32_t GPIO2APB2CLK(uint32_t  GPIOx);
    enum IRQn MapIRQn(uint32_t BASEType);
    void CfgPINOut(uint32_t GPIOx, uint16_t GPIO_Pin);
    void CfgPINIn(uint32_t GPIOx, uint16_t GPIO_Pin);

    void HwCfgInit();
    void TimCfg(uint32_t timeUs,uint32_t BASEType);


    void JTAGOFF();
    void APBCLKCfg( uint32_t  GPIOx,FunctionalState NewState);
#endif