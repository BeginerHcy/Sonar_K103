#ifndef __STMFLASH_H__
#define __STMFLASH_H__



#include "stm32f10x.h"



/************************************ STM32 内部 FLASH 配置 *********************************/
#define macSTM32_FLASH_SIZE 128 	 		      //所选STM32的FLASH容量大小(单位为K)
#define macSTM32_FLASH_WREN 1              //使能 FLASH 写入(0，不是能;1，使能)
#define FLASH_SAVE_ADDR  0X0801F000		     //0x820000 0x20000=128Kb, 0x20000-0x1F000 = 0x1000 /1024 = 4Kbyte


/************************************ STM32 内部 FLASH 函数声明 *********************************/
uint16_t STMFLASH_ReadHalfWord ( uint32_t faddr );		  //读出半字  
void     STMFLASH_WriteLenByte ( uint32_t WriteAddr, uint32_t DataToWrite, uint16_t Len );	      //指定地址开始写入指定长度的数据
uint32_t STMFLASH_ReadLenByte  ( uint32_t ReadAddr, uint16_t Len );					                    	//指定地址开始读取指定长度数据
void     STMFLASH_Write        ( uint32_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );		//从指定地址开始写入指定长度的数据
void     STMFLASH_Read         ( uint32_t ReadAddr, uint16_t * pBuffer, uint16_t NumToRead );   	//从指定地址开始读出指定长度的数据
void     Test_Write            ( uint32_t WriteAddr, uint16_t WriteData );                        //测试写入		

#endif

















