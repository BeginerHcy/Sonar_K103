#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "string.h"
#include "stm32f10x_spi.h"

/////define SPI Interface
#define      macSPIx                                     SPI1
#define      macSPI_APBxClock_FUN                        RCC_APB2PeriphClockCmd
#define      macSPI_CLK                                  RCC_APB2Periph_SPI1

#define      macSPI_CS_APBxClock_FUN                     RCC_APB2PeriphClockCmd
#define      macSPI_CS_CLK                               RCC_APB2Periph_GPIOC    
#define      macSPI_CS_PORT                              GPIOC
#define      macSPI_CS_PIN                               GPIO_Pin_4

#define      macSPI_SCK_APBxClock_FUN                    RCC_APB2PeriphClockCmd
#define      macSPI_SCK_CLK                              RCC_APB2Periph_GPIOA   
#define      macSPI_SCK_PORT                             GPIOA   
#define      macSPI_SCK_PIN                              GPIO_Pin_5

#define      macSPI_MISO_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define      macSPI_MISO_CLK                             RCC_APB2Periph_GPIOA    
#define      macSPI_MISO_PORT                            GPIOA 
#define      macSPI_MISO_PIN                             GPIO_Pin_6

#define      macSPI_MOSI_APBxClock_FUN                   RCC_APB2PeriphClockCmd
#define      macSPI_MOSI_CLK                             RCC_APB2Periph_GPIOA    
#define      macSPI_MOSI_PORT                            GPIOA 
#define      macSPI_MOSI_PIN                             GPIO_Pin_7

/////define regeisters//////
////Redefine
#define PxA GPIOA_BASE
#define PxB GPIOB_BASE
#define PxC GPIOC_BASE
#define PxD GPIOD_BASE
#define Uartx1 USART1_BASE
#define Uartx2 USART2_BASE
#define Uartx3 USART3_BASE
#define TIMx3  TIM3_BASE///使用TIM3作为1ms中断源
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
#define DO6 PxB,PIN8
#define DO5 PxB,PIN9
#define DO4 PxA,PIN0
#define DO3 PxA,PIN1
#define DO2 PxA,PIN4
#define DO1 PxA,PIN5
#define DO_CANS PxB,PIN12
#define DO_YL1 PxB,PIN13
#define DO_YL2 PxB,PIN14

#define DI4 PxA,PIN6
#define DI3 PxA,PIN7
#define DI2 PxC,PIN4
#define DI1 PxC,PIN5
#define BT4 PxD,PIN2
#define BT3 PxB,PIN3
#define BT2 PxB,PIN4
#define BT1 PxB,PIN5

///////////////////
#define SS2 PxC,PIN0
///////////////////
#define Uart3TX PxB,PIN10
#define Uart3RX PxB,PIN11
///////////////////
#define Uart1TX PxA,PIN9
#define Uart1RX PxA,PIN10
///////////////////
#define Uart2TX PxA,PIN2
#define Uart2RX PxA,PIN3
#define Uart2RDE PxA,PIN1
///////////////////
#define E2romSCL PxB,PIN6
#define E2romSDA PxB,PIN7

//////SPI function
#define      macSPI_IMU_CS_ENABLE()                       GPIO_ResetBits( macSPI_CS_PORT, macSPI_CS_PIN )
#define      macSPI_IMU_CS_DISABLE()                      GPIO_SetBits( macSPI_CS_PORT, macSPI_CS_PIN )
///////////////////////
///////////////////////////////////////

////////////////
void SystemConfig();
uint32_t GPIO2APB2CLK(uint32_t  GPIOx);
enum IRQn MapIRQn(uint32_t BASEType);
void CfgPINOut(uint32_t GPIOx, uint16_t GPIO_Pin);
void CfgPINIn(uint32_t GPIOx, uint16_t GPIO_Pin);
void CfgRS232(uint32_t UartX,char uartPar[],uint32_t GPTx, uint16_t GPTX_Pin,uint32_t GPRx, uint16_t GPRX_Pin);
void HwCfgInit();
void TimCfg(uint32_t timeUs,uint32_t BASEType);
//SPI 相关
void SPI_IMU_Init(void);
u16 SPI_IMU_SendByte(u16 byte);

u16 WirteReg(u8 Addr, u8 data);
u16 ReadReg(u8 Addr);
void JTAGOFF();
u16 ReceiveByte(void);
void APBCLKCfg( uint32_t  GPIOx,FunctionalState NewState);
double TempScal(int16_t a);
void Temp2Raw(void);
double VelScal(int16_t a);
double AccScal(int16_t a);
double VelOFFScal(int16_t a);
double AccOFFScal(int16_t a);