#include "IOMap.h"

AGVIO_type BoxIO;

cyc_task_fun cycTask[]={cyc2ms,cyc4ms,cyc10ms,cyc100ms,cyc200ms,cyc500ms,cycLongTsk};
typedef	enum {task2ms=0,task4ms,task10ms,task100ms,task200ms,task500ms,taskLong} cycTaskIndex;

void RS2322TTL();
void TTL2RS232();
void PVMap(AGVIO_type * pBoxIO)
{
		///////DI MAPING///////
		pBoxIO->DI[0]		= ReadIn(DI1);
		pBoxIO->DI[1]		= ReadIn(DI2);
		pBoxIO->DI[2]		= ReadIn(DI3);
		pBoxIO->DI[3]		= ReadIn(DI4);
	
		pBoxIO->DI[4]		= ReadIn(BT1);
		pBoxIO->DI[5]		= ReadIn(BT2);
		pBoxIO->DI[6]		= ReadIn(BT3);
		pBoxIO->DI[7]		= ReadIn(BT4);
	
		//////DO Maping/////////
		if(SonarDual.Internal.OutComState==1){
			SetDO(DoBuzzer,BoxIO.flagLed[sLedFlash]);
		}
		else if(SonarDual.Internal.OutComState==2){
			SetDO(DoBuzzer,BoxIO.flagLed[DBLedFlash]);
		}
		else if(SonarDual.Internal.OutComState==3){	
			SetDO(DoBuzzer,BoxIO.flagLed[fLedFlash]);
		}
		else{
			SetDO(DoBuzzer,0);
		}	
	
		SetDO(DO1,pBoxIO->DO[1]);
		SetDO(DO2,pBoxIO->DO[2]);
		SetDO(DO3,pBoxIO->DO[3]);
		SetDO(DO4,pBoxIO->DO[4]);
	
		SonarDual.bDI1 = pBoxIO->DI[0];
		SonarDual.bDI2 = pBoxIO->DI[1];
		SonarDual.bDI3 = pBoxIO->DI[2];
		SonarDual.bDI4 = pBoxIO->DI[3];
		
		pBoxIO->DO[1] = SonarDual.bDO1;
		pBoxIO->DO[2] = SonarDual.bDO2;
		pBoxIO->DO[3] = SonarDual.bDO3;
		pBoxIO->DO[4] = SonarDual.bDO4;
			
		SetDO(DO5,pBoxIO->DO[5]);
		SetDO(DO6,pBoxIO->DO[6]);	
		SetDO(DO_CANS,pBoxIO->DO[7]);	
		SetDO(DO_YL1,pBoxIO->DO[8]);
		SetDO(DO_YL2,pBoxIO->DO[9]);			
		///////////////////////		
}

void FillUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx)//
{
	USART_TypeDef * Uarts;
	Uarts = (USART_TypeDef*)USARTx;
	if(USART_GetITStatus(Uarts, USART_IT_RXNE) != RESET)
	{
		u8 ResData;
		u8 iFill;
		ResData = Uarts->DR;
		iFill=UartDat->pRfil;
		UartDat->rBuffer[iFill] = ResData;
		UartDat->pRfil++;
		
		if(UartDat->pRfil >= UrtBfLen)
		{
			if(UartDat->pRder>0)UartDat->pRder--;
			UartDat->pRfil = UrtBfLen-1;
			MBLArry(UartDat->rBuffer,UrtBfLen);
		}
	}
}


void MBLArry(u8 *buffer,uint8_t bufLen)
{
	u8 iArry;
	if(bufLen<1) bufLen=1;
	for(iArry=0;iArry<bufLen-1;iArry++)
	{
		buffer[iArry] = buffer[iArry+1];
	}
}


void USART1_IRQHandler(void)///RS232
{
	FillUrtBuf(&BoxIO.Uart1Dat,Uartx1);//////fill the Com Buffer//////
}

void USART2_IRQHandler(void)///TTL
{
	FillUrtBuf(&BoxIO.Uart2Dat,Uartx2);//////fill the Com Buffer//////	
}
void TIM3_IRQHandler(void)
{
	static uint32_t oldTimeStamp[10];					///5个不同时间周期的中断处理
	if(TIM3->SR&0X0001)												//溢出中断
	{
		BoxIO.timeStamp++;											//生成一个TimeStamp的时钟		
		/////////////////2MS执行一遍////////////////////
		oldTimeStamp[task2ms]++;
		if(oldTimeStamp[task2ms]>=time2ms)
		{
			cycTask[task2ms]();
			oldTimeStamp[task2ms] = 0;
		};
		/////////////////4MS执行一遍////////////////////
		oldTimeStamp[task4ms]++;
		if(oldTimeStamp[task4ms]>=time4ms)
		{
			cycTask[task4ms]();
			oldTimeStamp[task4ms] = 0;
		};
		/////////////////10MS执行一遍////////////////////
		oldTimeStamp[task10ms]++;
		if(oldTimeStamp[task10ms]>=time10ms)
		{
			cycTask[task10ms]();
			oldTimeStamp[task10ms] = 0;
		};
		/////////////////200MS执行一遍////////////////////
		oldTimeStamp[task200ms]++;
		if(oldTimeStamp[task200ms]>=time200ms)
		{
			cycTask[task200ms]();
			oldTimeStamp[task200ms] = 0;
		};
		/////////////////100MS执行一遍////////////////////
		oldTimeStamp[task100ms]++;
		if(oldTimeStamp[task100ms]>=time100ms)
		{
			cycTask[task100ms]();
			oldTimeStamp[task100ms] = 0;
		};
		/////////////////500ms执行一遍///////////////////
		oldTimeStamp[task500ms]++;
		if(oldTimeStamp[task500ms]>=time500ms)
		{
			cycTask[task500ms]();
			oldTimeStamp[task500ms] = 0;
		};
		oldTimeStamp[taskLong]++;
		////////////////长时间的时间，任意设定/////////
		if(oldTimeStamp[taskLong]>=timeLong)
		{
			cycTask[taskLong]();
			oldTimeStamp[taskLong] = 0;
		};
	}
	
	TIM3->SR&=~(1<<0);//清除中断标志位 	 
}

void SendUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx)
{
		unsigned char i;
		unsigned char bufLen = UartDat->sLen;
		USART_TypeDef * Uarts;
		Uarts = (USART_TypeDef*)USARTx;
		for(i=0;i<bufLen;i++)
		{			
			USART_SendData(Uarts, UartDat->sBuffer[i]);
			while (USART_GetFlagStatus(Uarts, USART_FLAG_TXE) == RESET);			
		}
		while(USART_GetFlagStatus(Uarts, USART_FLAG_TC)==RESET);
};

void RS2322TTL()
{
	u8 ResData;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		ResData = USART1->DR;
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
		USART_SendData(USART2, ResData);
//	while (USART_GetFlagStatus(USART2, USART_FLAG_TC));		

}
void TTL2RS232()
{
	u8 ResData;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
		ResData = USART2->DR;
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);		
		USART_SendData(USART1, ResData);
//	while (USART_GetFlagStatus(USART2, USART_FLAG_TC));		

}
