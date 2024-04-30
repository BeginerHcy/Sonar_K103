#include "HwCfg.h"
#include "delay.h"
#include "IOMap.h"
#include "stm_flash.h"
#include "BoxApp.h"
#include "stm32f10x_dma.h"

uint32_t ComBauderate[4] = {9600, 19200, 38400, 115200};
uint8_t Parameter[300] = {0};
uint8_t ReadTemp[300] = {0};
SysParameter_type gSystemPara;
void CfgRS232(uint32_t UartX, uint8_t uartPar, uint32_t GPTx, uint16_t GPTX_Pin, uint32_t GPRx, uint16_t GPRX_Pin);
void CfgRS232_2(uint32_t UartX, uint8_t uartPar, uint32_t GPTx, uint16_t GPTX_Pin, uint32_t GPRx, uint16_t GPRX_Pin);

void SystemConfig()
{
	// switchoff the JTAG,JTAG will reused some other IO
	JTAGOFF();
	//////////////
	SystemInit();
	// config system timer 72M
	SysTick_Init();
	// config SysTick timer
	HwCfgInit();
	// config IO depending on BoardDesign
	TimCfg(1000, TIMx3); // ����TIM3��1ms���ж�
};
void TimCfg(uint32_t timeUs, uint32_t BASEType)
{
	u16 Frequency = 72; // MHz
	u16 arr = timeUs - 1;
	u16 psc = Frequency - 1;
	NVIC_InitTypeDef TimNVIC;
	TIM_TypeDef *TIMxS;
	TIMxS = (TIM_TypeDef *)BASEType;
	///////////////
	APBCLKCfg(BASEType, ENABLE);
	TIMxS->ARR = arr;	   // �趨�������Զ���װֵ
	TIMxS->PSC = psc;	   // Ԥ��Ƶ��
	TIMxS->DIER |= 1 << 0; // ���������ж�
	TIMxS->CR1 |= 0x01;	   // ʹ�ܶ�ʱ��3
	///////////////////�����ж�
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TimNVIC.NVIC_IRQChannel = MapIRQn(BASEType);
	TimNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	TimNVIC.NVIC_IRQChannelSubPriority = 3;
	TimNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&TimNVIC);
}
void JTAGOFF()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}
void HwCfgInit()
{
	/////////////
	// ��s1����Ϊ����
	CfgPINIn(DI4);
	CfgPINIn(DI3);
	CfgPINIn(DI2);
	CfgPINIn(DI1);
	CfgPINIn(DI8);
	CfgPINIn(DI7);
	CfgPINIn(DI5);
	CfgPINIn(DI5);

	/////////////
	// ��DO1����Ϊ���1
	CfgPINOut(DO1);
	CfgPINOut(DO2);
	CfgPINOut(DO3);
	CfgPINOut(DO4);
	CfgPINOut(DO5);
	CfgPINOut(DO6);

	CfgPINOut(LED1);
	CfgPINOut(LED2);
	CfgPINOut(LED3);
	CfgPINOut(LED4);
	CfgPINOut(LED5);

	/////////////Obtain the buffParameter/////////
	STMFLASH_Read(FLASH_SAVE_ADDR, (uint16_t *)ReadTemp, 300);
	/////////////
	if (0xAB == ReadTemp[0] && 0xCD == ReadTemp[299])
	{
		memcpy(&gSystemPara, &ReadTemp, sizeof(gSystemPara));
	}
	else
	{
		////////////////////////
		gSystemPara.BufferHead = 0xAB;
		gSystemPara.CANBusBauderate = 1; // 250k
		gSystemPara.CanNode = 0xD6;
		gSystemPara.cmdSaveParameter = 0;
		gSystemPara.DataComInterface = 0;

		gSystemPara.RS232Bauderate = 0;
		gSystemPara.RS485Bauderate = 0;
		gSystemPara.RS485Node = 0xE8;

		/// default parameters
		memcpy(&Parameter, &gSystemPara, sizeof(gSystemPara));
		Parameter[299] = 0xCD;
		STMFLASH_Write(FLASH_SAVE_ADDR, (uint16_t *)Parameter, 300);
	}
	//////////////////
	// RS232
	CfgRS232(Uartx3, gSystemPara.RS232Bauderate, Uart3TX, Uart3RX);
	// RS485
	CfgPINOut(Uart2RDE);
	CfgRS232_2(Uartx2, gSystemPara.RS485Bauderate, Uart2TX, Uart2RX);
};

void CfgRS232(uint32_t UartX, uint8_t uartPar, uint32_t GPTx, uint16_t GPTX_Pin, uint32_t GPRx, uint16_t GPRX_Pin)
{
	// 115200,8,N,1 "char uartPar[]"��������
	GPIO_InitTypeDef GPIO_InitStructureTx;
	GPIO_InitTypeDef GPIO_InitStructureRx;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	APBCLKCfg(UartX, ENABLE); // ʹ��PC�˿�ʱ��
	APBCLKCfg(GPTx, ENABLE);  // ʹ��PC�˿�ʱ��
	APBCLKCfg(GPRx, ENABLE);  // ʹ��PC�˿�ʱ��
	GPIO_InitStructureTx.GPIO_Pin = GPTX_Pin;
	GPIO_InitStructureTx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructureTx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPTx, &GPIO_InitStructureTx);

	GPIO_InitStructureRx.GPIO_Pin = GPRX_Pin;
	GPIO_InitStructureRx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructureRx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPRx, &GPIO_InitStructureRx);

	USART_InitStructure.USART_BaudRate = ComBauderate[uartPar];						// ���������ã�115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// ����λ�����ã�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ֹͣλ���ã�1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// �Ƿ���żУ�飺��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Ӳ��������ģʽ���ã�û��ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �����뷢�Ͷ�ʹ��

	USART_Init((USART_TypeDef *)UartX, &USART_InitStructure);
	USART_Cmd((USART_TypeDef *)UartX, ENABLE);
	USART_ITConfig((USART_TypeDef *)UartX, USART_IT_RXNE, ENABLE); // ��ʼ��USART1
																   //  USART1ʹ��

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = MapIRQn(UartX);
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

void CfgRS232_2(uint32_t UartX, uint8_t uartPar, uint32_t GPTx, uint16_t GPTX_Pin, uint32_t GPRx, uint16_t GPRX_Pin)
{
	DMA_InitTypeDef DMA_InitStructure;
	// 115200,8,N,1 "char uartPar[]"��������
	GPIO_InitTypeDef GPIO_InitStructureTx;
	GPIO_InitTypeDef GPIO_InitStructureRx;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	APBCLKCfg(UartX, ENABLE); // ʹ��PC�˿�ʱ��
	APBCLKCfg(GPTx, ENABLE);  // ʹ��PC�˿�ʱ��
	APBCLKCfg(GPRx, ENABLE);  // ʹ��PC�˿�ʱ��
	GPIO_InitStructureTx.GPIO_Pin = GPTX_Pin;
	GPIO_InitStructureTx.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructureTx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPTx, &GPIO_InitStructureTx);

	GPIO_InitStructureRx.GPIO_Pin = GPRX_Pin;
	GPIO_InitStructureRx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructureRx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPRx, &GPIO_InitStructureRx);

	USART_InitStructure.USART_BaudRate = ComBauderate[uartPar];						// ���������ã�115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// ����λ�����ã�8λ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// ֹͣλ���ã�1λ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// �Ƿ���żУ�飺��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // Ӳ��������ģʽ���ã�û��ʹ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �����뷢�Ͷ�ʹ��

	USART_Init((USART_TypeDef *)UartX, &USART_InitStructure);
	USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
	USART_Cmd((USART_TypeDef *)UartX, ENABLE);

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel6);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&USART2->DR);
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)BoxIO.Uart2Dat.rBuffer;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 100;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	// DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);
	DMA_Cmd(DMA1_Channel6, ENABLE);

	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStruct.NVIC_IRQChannel = MapIRQn(UartX);
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
}

enum IRQn MapIRQn(uint32_t BASEType)
{
	switch (BASEType)
	{
	case Uartx2:
		return USART2_IRQn;
		break;
	case Uartx3:
		return USART3_IRQn;
		break;
	case TIMx3:
		return TIM3_IRQn;
		break;
	default:
		return 0;
		break;
	}
}
uint32_t GPIO2APB2CLK(uint32_t GPIOx)
{
	switch (GPIOx)
	{
	case PxA:
		return RCC_APB2Periph_GPIOA;
		break;

	case PxB:
		return RCC_APB2Periph_GPIOB;
		break;

	case PxC:
		return RCC_APB2Periph_GPIOC;
		break;

	case PxD:
		return RCC_APB2Periph_GPIOD;
		break;
	case Uartx2:
		return RCC_APB1Periph_USART2;
		break;
	case Uartx3:
		return RCC_APB1Periph_USART3;
		break;
	case TIMx3:
		return RCC_APB1Periph_TIM3;
		break;
	}
}
void APBCLKCfg(uint32_t GPIOx, FunctionalState NewState)
{
	if (GPIOx < APB2PERIPH_BASE)
		RCC_APB1PeriphClockCmd(GPIO2APB2CLK(GPIOx), NewState);
	else if (GPIOx >= APB2PERIPH_BASE)
		RCC_APB2PeriphClockCmd(GPIO2APB2CLK(GPIOx), NewState);
}
void CfgPINOut(uint32_t GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	APBCLKCfg(GPIOx, ENABLE); // ʹ��PC�˿�ʱ��
							  //////////////���ó�����ģ�Ƶ����50M//////////////
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	////////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init((GPIO_TypeDef *)GPIOx, &GPIO_InitStructure);
	GPIO_ResetBits((GPIO_TypeDef *)GPIOx, GPIO_Pin);
};

void CfgPINIn(uint32_t GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	APBCLKCfg(GPIOx, ENABLE); // ʹ��PC�˿�ʱ��
							  //////////////���ó�����ģ�Ƶ����50M/////////////
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	///////////////////////////////////////////////////
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_Init((GPIO_TypeDef *)GPIOx, &GPIO_InitStructure);
};

void delayxx(u32 xx)
{
	__IO u32 loop = xx;
	__IO int lo = 0;
	while (loop--)
	{
		for (lo = 0; lo < 100; lo++)
			;
	}
}
