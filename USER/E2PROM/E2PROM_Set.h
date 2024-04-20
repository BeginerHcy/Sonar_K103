#ifndef __E2PROM_SET_H
#define __E2PROM_SET_H
#include "myiic.h"   


#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	    8191
#define AT24C128	16383
#define AT24C256	32767  
//开发板使用的是24c02，所以定义EE_TYPE为AT24C02
#define EE_TYPE AT24C256
					  
u8 E2PROM_ReadOneByte(u16 ReadAddr);							//指定地址读取一个字节
void E2PROM_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//指定地址写入一个字节
void E2PROM_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 E2PROM_ReadLenByte(u16 ReadAddr,u8 Len);					//指定地址开始读取指定长度数据

u8 E2PROM_Write_U16(u16 WriteAddr,u16 *pBuffer,u16 NumToWrite);	//从指定地址开始写入指定长度16位数组

u8 E2PROM_Read_U16(u16 ReadAddr,u16 *pBuffer,u16 NumToRead);   	//从指定地址开始读出指定长度的16位数组

u8 E2PROM_Check(void);  //检查器件
void E2PROM_Init(void); //初始化IIC

void E2PROM_Operate(void);

extern uint16_t E2PROM_Oprate_Flag;

#endif
















