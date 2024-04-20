/*******************************************************************************

���ܣ�E2PROMģ��

����ʱ�䣺2016/03/21

���ܣ�

�޸���ʷ��

********************************************************************************/

#include "E2PROM_Set.h" 
#include "delay.h" 	
#include "Station_Cmd.h"

uint16_t E2PROM_Oprate_Flag = 0;


/*********************************************************************************

˵����E2PROM��������������E2PROM������־�Կ������Ĳ������е��籣���Լ���ȡ��

**********************************************************************************/
void E2PROM_Operate(void)
{
	switch(E2PROM_Oprate_Flag)
	{
		case 1:  
			
			//if(E2PROM_Write_U16(0,Road1_Station_Num,50)) E2PROM_Oprate_Flag=0 ;
		
		break;
		
		case 2:  
			
			//if(E2PROM_Read_U16(0,Road1_Station_Num,50)) E2PROM_Oprate_Flag=0;
		
		break;

		
		default:
		
		break;
	}
}



//��ʼ��IIC�ӿ�
void E2PROM_Init(void)
{
	IIC_Init();
}



/*********************************************************************************

˵������E2PROMָ����ַ����һ������
ReadAddr:��ʼ�����ĵ�ַ  
����ֵ  :����������

**********************************************************************************/
u8 E2PROM_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}



/*********************************************************************************

˵������E2PROMָ����ַд��һ������
WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
DataToWrite:Ҫд�������

**********************************************************************************/
void E2PROM_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}




/*********************************************************************************

˵������E2PROM�����ָ����ַ��ʼд�볤��ΪLen������
�ú�������д��16bit����32bit������.
WriteAddr  :��ʼд��ĵ�ַ  
DataToWrite:���������׵�ַ
Len        :Ҫд�����ݵĳ���2,4

**********************************************************************************/
void E2PROM_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		E2PROM_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}




/*********************************************************************************

˵������E2PROM�����ָ����ַ��ʼ��������ΪLen������
�ú������ڶ���16bit����32bit������.
ReadAddr   :��ʼ�����ĵ�ַ 
����ֵ     :����
Len        :Ҫ�������ݵĳ���2,4

**********************************************************************************/
u32 E2PROM_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=E2PROM_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}



/*********************************************************************************

˵�������E2PROM�Ƿ�����
��������24XX�����һ����ַ(255)���洢��־��.
���������24Cϵ��,�����ַҪ�޸�
����1:���ʧ��
����0:���ɹ�

**********************************************************************************/
u8 E2PROM_Check(void)
{
	u8 temp;
	temp=E2PROM_ReadOneByte(255);//����ÿ�ο�����дE2PROM			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		E2PROM_WriteOneByte(255,0X55);
	    temp=E2PROM_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}



/*********************************************************************************

˵������E2PROM�����ָ����ַ��ʼ����ָ��������16λ����
ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
pBuffer  :���������׵�ַ
NumToRead:Ҫ�������ݵĸ���

**********************************************************************************/
u8 E2PROM_Read_U16(u16 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=E2PROM_ReadLenByte(ReadAddr,2);	
		ReadAddr += 2;
		NumToRead-=1;  //E2PROMÿ����ַ1���ֽڣ������Ƕ�16λ���ݽ��в�����
	}
	return 1;
}  

/*********************************************************************************

˵������E2PROM�����ָ����ַ��ʼд��ָ����������
WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
pBuffer   :���������׵�ַ
NumToWrite:Ҫд�����ݵĸ���
Len:�������ݳ��ȣ�16λ

**********************************************************************************/
u8 E2PROM_Write_U16(u16 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		E2PROM_WriteLenByte(WriteAddr,*pBuffer,2);
		WriteAddr+=2;  //E2PROMÿ����ַ1���ֽڣ������Ƕ�16λ���ݽ��в�����
		pBuffer++;
	}
	return 1;
}




