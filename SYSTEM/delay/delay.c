/**************************************
 * 文件名  ：SysTick.c
 * 描述    ：SysTick 系统滴答时钟10us中断函数库,中断时间可自由配置，
 *           常用的有 1us 10us 1ms 中断。         
 * 实验平台：MINI STM32开发板 基于STM32F103C8T6
 * 硬件连接：无
 * 库版本  ：ST3.0.0  

*********************************************************/

#include "delay.h"

static __IO u32 TimingDelay;


static u8  fac_us=0;//us延时倍乘数			   
static u16 fac_ms=0;//ms延时倍乘数,在ucos下,代表每个节拍的ms数

/*初始化  SysTick*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms中断一次
	 * SystemFrequency / 100000	 10us中断一次
	 * SystemFrequency / 1000000 1us中断一次
	 */
	if (SysTick_Config(SystemFrequency / 1000))
  { 
    /* Capture error */
    while (1);
  }
	
	
}


/*us延时程序,10us为一个单位 */
void Delay_us(__IO u32 nTime)
{ 
  TimingDelay = nTime;
  while(TimingDelay-- != 0);
}


/* 获取节拍程序，在 SysTick 中断函数 SysTick_Handler()调用	 */  
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
void delay_init(u8 SYSCLK)
{
 	SysTick->CTRL&=~(1<<2);	//SYSTICK使用外部时钟源	 
	fac_us=SYSCLK/8;		//不论是否使用ucos,fac_us都需要使用	    
	fac_ms=(u16)fac_us*1000;//非ucos下,代表每个ms需要的systick时钟数   
}		
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;		//LOAD的值	 
	if(nus==0)return;						//nus=0,直接退出
	ticks=nus*fac_us; 					//需要的节拍数
	tcnt=0;
	told=SysTick->VAL;        	//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//时间超过/等于要延迟的时间,则退出.
		}
	};							    
} 
void delay_ms(u16 nms)
{	
	delay_us((u32)(nms*1000));	//普通方式延时 
}