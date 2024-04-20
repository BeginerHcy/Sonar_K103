/*************************************
 * 文件名  ：key.c
 * 描述    ：按键应用函数库        
 * 实验平台：MINI STM32开发板 基于STM32F103C8T6
 * 硬件连接： PA0 - key1          
 * 库版本  ：ST3.0.0 

**********************************************************************************/
#include "stm32f10x_exti.h"

#include "key.h" 



 /*不精确的延时 */
 void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

 /*	 配置按键用到的I/O口 */
void Key_GPIO_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); 	//开启按键端口PC的时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 				//端口配置为上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);								//初始化端口
}

 /* 检测是否有按键按下 */
u8 Key_Scan(GPIO_TypeDef* GPIOx,u16 GPIO_Pin)
{			
		/*检测是否有``按键按下 */
   	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON ) 
	  {	   
	 	 /*延时消抖*/
	  	Delay(50000);		
	   		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
					{	 
						/*等待按键释放 */
						while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON); 
						Delay(5000);
						if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_OFF);
			 			return 	KEY_ON;	 
					}
			else
					return KEY_OFF;
		}
	else
		return KEY_OFF;
}
void Key_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置P[A|B|C|D|E]10为中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


}

void EXTI_PC0_Config(void)
{
		EXTI_InitTypeDef EXTI_InitStructure;

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); 
		EXTI_InitStructure.EXTI_Line = EXTI_Line0;
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;		
		EXTI_Init(&EXTI_InitStructure);
	
}

void Key_Config(void)
{
	Key_GPIO_Config();
	
	Key_NVIC_Config();
	
	EXTI_PC0_Config();
}


