#include "IOMap.h"
#include "HwCfg.h"
#include "stm32f10x_dma.h"
AGVIO_type BoxIO;
#define RS485RE SetDO(Uart2RDE, 0)
#define RS485SE SetDO(Uart2RDE, 1)
cyc_task_fun cycTask[] = {cyc2ms, cyc4ms, cyc10ms, cyc100ms, cyc200ms, cyc500ms, cycLongTsk};
typedef enum
{
	task2ms = 0,
	task4ms,
	task10ms,
	task100ms,
	task200ms,
	task500ms,
	taskLong
} cycTaskIndex;

void PVMap(AGVIO_type *pBoxIO)
{
	///////DI MAPING///////
	pBoxIO->DI[0] = ReadIn(DI1);
	pBoxIO->DI[1] = ReadIn(DI2);
	pBoxIO->DI[2] = ReadIn(DI3);
	pBoxIO->DI[3] = ReadIn(DI4);

	pBoxIO->DI[4] = ReadIn(DI5);
	pBoxIO->DI[5] = ReadIn(DI6);
	pBoxIO->DI[6] = ReadIn(DI7);
	pBoxIO->DI[7] = ReadIn(DI8);

	SetDO(DO1, pBoxIO->DO[0]);
	SetDO(DO2, pBoxIO->DO[1]);
	SetDO(DO3, pBoxIO->DO[2]);
	SetDO(DO4, pBoxIO->DO[3]);

	SetDO(DO5, pBoxIO->DO[4]);
	SetDO(DO6, pBoxIO->DO[5]);
	SetDO(DO7, pBoxIO->DO[6]);
	SetDO(DO8, pBoxIO->DO[7]);
	///////////////////////
	SetDO(LED1, pBoxIO->LED[0]);
	SetDO(LED2, !pBoxIO->LED[1]);
	SetDO(LED3, pBoxIO->LED[2]);
	SetDO(LED4, pBoxIO->LED[3]);
	SetDO(LED5, pBoxIO->LED[4]);
}

void FillUrtBuf(UrtBuf_type *UartDat, uint32_t USARTx) //
{
	USART_TypeDef *Uarts;
	Uarts = (USART_TypeDef *)USARTx;
	if (USART_GetITStatus(Uarts, USART_IT_RXNE) != RESET)
	{
		u8 ResData;
		u8 iFill;
		ResData = Uarts->DR;
		iFill = UartDat->pRfil;
		UartDat->rBuffer[iFill] = ResData;
		UartDat->pRfil++;

		if (UartDat->pRfil >= UrtBfLen)
		{
			if (UartDat->pRder > 0)
				UartDat->pRder--;
			UartDat->pRfil = UrtBfLen - 1;
			MBLArry(UartDat->rBuffer, UrtBfLen);
		}
	}
}

void MBLArry(u8 *buffer, uint8_t bufLen)
{
	u8 iArry;
	if (bufLen < 1)
		bufLen = 1;
	for (iArry = 0; iArry < bufLen - 1; iArry++)
	{
		buffer[iArry] = buffer[iArry + 1];
	}
}

void USART3_IRQHandler(void) /// RS232
{
	FillUrtBuf(&BoxIO.Uart3Dat, Uartx3); //////fill the Com Buffer//////
}

void USART2_IRQHandler(void) /// RS485
{
	uint32_t temp = 0;
	uint16_t i = 0;
	if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		// USART_ClearFlag(USART1,USART_IT_IDLE);
		temp = USART2->SR;
		temp = USART2->DR; //?USART_IT_IDLE??
		DMA_Cmd(DMA1_Channel6, DISABLE);

		temp = 100 - DMA_GetCurrDataCounter(DMA1_Channel6);
		BoxIO.Uart2Dat.pRfil = temp;
		BoxIO.Uart2Dat.pRder = 0;
		DMA_SetCurrDataCounter(DMA1_Channel6, 100);
		DMA_Cmd(DMA1_Channel6, ENABLE);
	}
	// FillUrtBuf(&BoxIO.Uart2Dat,Uartx2);//////fill the Com Buffer//////
}
void TIM3_IRQHandler(void)
{
	static uint32_t oldTimeStamp[10]; /// 5����ͬʱ�����ڵ��жϴ���
	if (TIM3->SR & 0X0001)			  // ����ж�
	{
		BoxIO.TimeStamp++; // ����һ��TimeStamp��ʱ��
		/////////////////2MSִ��һ��////////////////////
		oldTimeStamp[task2ms]++;
		if (oldTimeStamp[task2ms] >= time2ms)
		{
			cycTask[task2ms]();
			oldTimeStamp[task2ms] = 0;
		};
		/////////////////4MSִ��һ��////////////////////
		oldTimeStamp[task4ms]++;
		if (oldTimeStamp[task4ms] >= time4ms)
		{
			cycTask[task4ms]();
			oldTimeStamp[task4ms] = 0;
		};
		/////////////////10MSִ��һ��////////////////////
		oldTimeStamp[task10ms]++;
		if (oldTimeStamp[task10ms] >= time10ms)
		{
			cycTask[task10ms]();
			oldTimeStamp[task10ms] = 0;
		};
		/////////////////200MSִ��һ��////////////////////
		oldTimeStamp[task200ms]++;
		if (oldTimeStamp[task200ms] >= time200ms)
		{
			cycTask[task200ms]();
			oldTimeStamp[task200ms] = 0;
		};
		/////////////////100MSִ��һ��////////////////////
		oldTimeStamp[task100ms]++;
		if (oldTimeStamp[task100ms] >= time100ms)
		{
			cycTask[task100ms]();
			oldTimeStamp[task100ms] = 0;
		};
		/////////////////500msִ��һ��///////////////////
		oldTimeStamp[task500ms]++;
		if (oldTimeStamp[task500ms] >= time500ms)
		{
			cycTask[task500ms]();
			oldTimeStamp[task500ms] = 0;
		};
		oldTimeStamp[taskLong]++;
		////////////////��ʱ���ʱ�䣬�����趨/////////
		if (oldTimeStamp[taskLong] >= timeLong)
		{
			cycTask[taskLong]();
			oldTimeStamp[taskLong] = 0;
		};
	}

	TIM3->SR &= ~(1 << 0); // ����жϱ�־λ
}

void SendUrtBuf(UrtBuf_type *UartDat, uint32_t USARTx)
{
	unsigned char i;
	unsigned char bufLen = UartDat->sLen;
	USART_TypeDef *Uarts;
	Uarts = (USART_TypeDef *)USARTx;
	switch (USARTx)
	{
	case Uartx2:
		RS485SE;
		break;
	default:;
		break;
	}
	for (i = 0; i < bufLen; i++)
	{
		USART_SendData(Uarts, UartDat->sBuffer[i]);
		while (USART_GetFlagStatus(Uarts, USART_FLAG_TXE) == RESET)
			;
	}
	while (USART_GetFlagStatus(Uarts, USART_FLAG_TC) == RESET)
		;
	switch (USARTx)
	{
	case Uartx2:
		RS485RE;
		break;
	default:;
		break;
	}
};
