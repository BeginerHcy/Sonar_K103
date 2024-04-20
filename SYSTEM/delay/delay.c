/**************************************
 * �ļ���  ��SysTick.c
 * ����    ��SysTick ϵͳ�δ�ʱ��10us�жϺ�����,�ж�ʱ����������ã�
 *           ���õ��� 1us 10us 1ms �жϡ�         
 * ʵ��ƽ̨��MINI STM32������ ����STM32F103C8T6
 * Ӳ�����ӣ���
 * ��汾  ��ST3.0.0  

*********************************************************/

#include "delay.h"

static __IO u32 TimingDelay;


static u8  fac_us=0;//us��ʱ������			   
static u16 fac_ms=0;//ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

/*��ʼ��  SysTick*/
void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	if (SysTick_Config(SystemFrequency / 1000))
  { 
    /* Capture error */
    while (1);
  }
	
	
}


/*us��ʱ����,10usΪһ����λ */
void Delay_us(__IO u32 nTime)
{ 
  TimingDelay = nTime;
  while(TimingDelay-- != 0);
}


/* ��ȡ���ĳ����� SysTick �жϺ��� SysTick_Handler()����	 */  
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}
void delay_init(u8 SYSCLK)
{
 	SysTick->CTRL&=~(1<<2);	//SYSTICKʹ���ⲿʱ��Դ	 
	fac_us=SYSCLK/8;		//�����Ƿ�ʹ��ucos,fac_us����Ҫʹ��	    
	fac_ms=(u16)fac_us*1000;//��ucos��,����ÿ��ms��Ҫ��systickʱ����   
}		
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;		//LOAD��ֵ	 
	if(nus==0)return;						//nus=0,ֱ���˳�
	ticks=nus*fac_us; 					//��Ҫ�Ľ�����
	tcnt=0;
	told=SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}
	};							    
} 
void delay_ms(u16 nms)
{	
	delay_us((u32)(nms*1000));	//��ͨ��ʽ��ʱ 
}