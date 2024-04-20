#include "HwCfg.h"
#include "delay.h"
#include "IOMap.h"
#include "stm_flash.h"
//AGVIO_type BoxIO;
uint32_t ComBauderate[4] = {9600,19200,38400,115200};
uint8_t Parameter[200] = {0};
uint8_t ReadTemp[200] = {0};
SysParameter_type gSystemPara;
void SystemConfig()
{
		//switchoff the JTAG,JTAG will reused some other IO
		JTAGOFF();
		//////////////
		SystemInit();	
		//config system timer 72M 	
		SysTick_Init();
    //config SysTick timer 	
		HwCfgInit();
		//config IO depending on BoardDesign
		TimCfg(1000,TIMx3);//利用TIM3做1ms的中断
		
};
void TimCfg(uint32_t timeUs ,uint32_t BASEType)
{
	u16 Frequency=72;//MHz
	u16 arr=timeUs-1;
	u16 psc=Frequency-1;
	NVIC_InitTypeDef TimNVIC;
	TIM_TypeDef * TIMxS;
	TIMxS = (TIM_TypeDef * )BASEType;
	///////////////
	APBCLKCfg(BASEType, ENABLE);
	TIMxS->ARR = arr;		 //设定计数器自动重装值 
	TIMxS->PSC =psc;  	   //预分频器	  
	TIMxS->DIER|=1<<0;   //允许更新中断	  
	TIMxS->CR1 |=0x01;    //使能定时器3
	///////////////////设置中断
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TimNVIC.NVIC_IRQChannel = MapIRQn(BASEType);
	TimNVIC.NVIC_IRQChannelPreemptionPriority = 1;
	TimNVIC.NVIC_IRQChannelSubPriority = 3;
	TimNVIC.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&TimNVIC);
}
void JTAGOFF()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}
void HwCfgInit()
{
		/////////////
		//将s1设置为输入
	  CfgPINIn(DI4);
		CfgPINIn(DI3);
	  CfgPINIn(DI2);
		CfgPINIn(DI1);
		CfgPINIn(BT4);
		CfgPINIn(BT3);
		CfgPINIn(BT2);
		CfgPINIn(BT1);
	
		/////////////
		//将DO1设置为输出1
		CfgPINOut(DoBuzzer);
		CfgPINOut(DO1);
		CfgPINOut(DO2);
		CfgPINOut(DO3);
		CfgPINOut(DO4);
		CfgPINOut(DO5);
		CfgPINOut(DO6);
		CfgPINOut(DO_CANS);
		CfgPINOut(DO_YL1);
		CfgPINOut(DO_YL2);
	
	
		/////////////Obtain the buffParameter/////////
	STMFLASH_Read(FLASH_SAVE_ADDR,( uint16_t * )ReadTemp,200);
	/////////////
	if(0xAB==ReadTemp[0]&&0xCD==ReadTemp[199]){
		memcpy(&gSystemPara,&ReadTemp,sizeof(gSystemPara));
	}
	else{
		////////////////////////
		gSystemPara.BufferHead = 0xAB;
		gSystemPara.CANBusBauderate = 1;//250k
		gSystemPara.CanNode = 0xD6;
		gSystemPara.DataAnsMethod = 0;
		gSystemPara.DataComInterface = 0;
		gSystemPara.DetectPolar = 0;
		gSystemPara.MagSensity = 1;
		gSystemPara.RequestInterval = 1;
		gSystemPara.RS232Bauderate = 0;
		gSystemPara.RS485Bauderate = 0;
		gSystemPara.RS485Node = 0xE8;
		gSystemPara.MagTapWide = 0;
		///default parameters
		memcpy(&Parameter,&gSystemPara,sizeof(gSystemPara));
		Parameter[199] = 0xCD;
		STMFLASH_Write(FLASH_SAVE_ADDR,( uint16_t * )Parameter,200);
	}		
	if(gSystemPara.MountDir!=0)
		gSystemPara.MountDir = 1;
	//////////////////
	/////////////
	CfgRS232(Uartx1,"115200,8,N,1",Uart1TX,Uart1RX);
	////配置232//
	CfgRS232(Uartx2,"115200,8,N,1",Uart2TX,Uart2RX);
	//SPI_IMU_Init();
	AGVBox.Config.Mode = ComTestMode;
};

void CfgRS232(uint32_t UartX,char uartPar[],uint32_t GPTx, uint16_t GPTX_Pin,uint32_t GPRx, uint16_t GPRX_Pin)
{
	//115200,8,N,1 "char uartPar[]"用来设置
	GPIO_InitTypeDef GPIO_InitStructureTx;
	GPIO_InitTypeDef GPIO_InitStructureRx;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	APBCLKCfg(UartX, ENABLE); 						// 使能PC端口时钟
	APBCLKCfg(GPTx, ENABLE); 						  // 使能PC端口时钟
	APBCLKCfg(GPRx, ENABLE); 						  // 使能PC端口时钟
  GPIO_InitStructureTx.GPIO_Pin = GPTX_Pin;
  GPIO_InitStructureTx.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructureTx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPTx, &GPIO_InitStructureTx); 
		
  GPIO_InitStructureRx.GPIO_Pin = GPRX_Pin;
  GPIO_InitStructureRx.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructureRx.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init((GPIO_TypeDef *)GPRx, &GPIO_InitStructureRx); 
	
	USART_InitStructure.USART_BaudRate = 9600;																		//波特率设置：115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;											//数据位数设置：8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 													//停止位设置：1位
	USART_InitStructure.USART_Parity = USART_Parity_No ;  													//是否奇偶校验：无
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;									//接收与发送都使能
	
	USART_Init((USART_TypeDef *)UartX, &USART_InitStructure);  
	USART_Cmd((USART_TypeDef *)UartX, ENABLE);		
	USART_ITConfig((USART_TypeDef *)UartX,USART_IT_RXNE,ENABLE);										//初始化USART1
																																									// USART1使能

	
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
		case Uartx1:
			return USART1_IRQn;
			break;
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
uint32_t GPIO2APB2CLK(uint32_t  GPIOx)
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
		
		case Uartx1:
			return RCC_APB2Periph_USART1;
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
void APBCLKCfg( uint32_t  GPIOx,FunctionalState NewState)
{
	if(GPIOx<APB2PERIPH_BASE)
		RCC_APB1PeriphClockCmd(GPIO2APB2CLK(GPIOx), NewState);
	else if(GPIOx>=APB2PERIPH_BASE)
		RCC_APB2PeriphClockCmd(GPIO2APB2CLK(GPIOx), NewState);	 
}
void CfgPINOut(uint32_t  GPIOx,uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	APBCLKCfg(GPIOx, ENABLE); 						// 使能PC端口时钟
	//////////////设置成输出的，频率是50M//////////////
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	////////////////////////////////////////////////////
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin;	
  GPIO_Init((GPIO_TypeDef *)GPIOx,&GPIO_InitStructure);
  GPIO_ResetBits((GPIO_TypeDef *)GPIOx,GPIO_Pin);
};


void CfgPINIn(uint32_t GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  APBCLKCfg(GPIOx, ENABLE); 						// 使能PC端口时钟
	//////////////设置成输入的，频率是50M/////////////
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;       
  //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	///////////////////////////////////////////////////
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin;	
  GPIO_Init((GPIO_TypeDef *)GPIOx,&GPIO_InitStructure);
};

void delayxx(u32 xx)
{
	__IO u32 loop = xx;
	__IO int lo = 0;
	while(loop--)
	{
		for(lo=0; lo<100; lo++);		
	}	
}
