
/**************************************

*********************************************************/

#include "stm32f10x.h"
#include "delay.h"
#include "HwCfg.h"
#include "BoxApp.h" 

int main(void)
{
	
	_BoxTaskInit_();

	_BoxTaskCyc_();			
	
}