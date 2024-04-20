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
//������ʹ�õ���24c02�����Զ���EE_TYPEΪAT24C02
#define EE_TYPE AT24C256
					  
u8 E2PROM_ReadOneByte(u16 ReadAddr);							//ָ����ַ��ȡһ���ֽ�
void E2PROM_WriteOneByte(u16 WriteAddr,u8 DataToWrite);		//ָ����ַд��һ���ֽ�
void E2PROM_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 E2PROM_ReadLenByte(u16 ReadAddr,u8 Len);					//ָ����ַ��ʼ��ȡָ����������

u8 E2PROM_Write_U16(u16 WriteAddr,u16 *pBuffer,u16 NumToWrite);	//��ָ����ַ��ʼд��ָ������16λ����

u8 E2PROM_Read_U16(u16 ReadAddr,u16 *pBuffer,u16 NumToRead);   	//��ָ����ַ��ʼ����ָ�����ȵ�16λ����

u8 E2PROM_Check(void);  //�������
void E2PROM_Init(void); //��ʼ��IIC

void E2PROM_Operate(void);

extern uint16_t E2PROM_Oprate_Flag;

#endif
















