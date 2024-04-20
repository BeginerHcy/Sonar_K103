/*******************************************************************************

功能：E2PROM模块

创建时间：2016/03/21

功能：

修改历史：

********************************************************************************/

#include "E2PROM_Set.h" 
#include "delay.h" 	
#include "Station_Cmd.h"

uint16_t E2PROM_Oprate_Flag = 0;


/*********************************************************************************

说明：E2PROM操作函数，根据E2PROM操作标志对控制器的参数进行掉电保存以及读取。

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



//初始化IIC接口
void E2PROM_Init(void)
{
	IIC_Init();
}



/*********************************************************************************

说明：在E2PROM指定地址读出一个数据
ReadAddr:开始读数的地址  
返回值  :读到的数据

**********************************************************************************/
u8 E2PROM_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//发送高地址	    
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //发送器件地址0XA0,写数据 	   
	IIC_Wait_Ack(); 
    IIC_Send_Byte(ReadAddr%256);   //发送低地址
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //进入接收模式			   
	IIC_Wait_Ack();	 
    temp=IIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}



/*********************************************************************************

说明：在E2PROM指定地址写入一个数据
WriteAddr  :写入数据的目的地址    
DataToWrite:要写入的数据

**********************************************************************************/
void E2PROM_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //发送写命令
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//发送高地址	  
	}else IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //发送器件地址0XA0,写数据 	 
	IIC_Wait_Ack();	   
    IIC_Send_Byte(WriteAddr%256);   //发送低地址
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //发送字节							   
	IIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}




/*********************************************************************************

说明：在E2PROM里面的指定地址开始写入长度为Len的数据
该函数用于写入16bit或者32bit的数据.
WriteAddr  :开始写入的地址  
DataToWrite:数据数组首地址
Len        :要写入数据的长度2,4

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

说明：在E2PROM里面的指定地址开始读出长度为Len的数据
该函数用于读出16bit或者32bit的数据.
ReadAddr   :开始读出的地址 
返回值     :数据
Len        :要读出数据的长度2,4

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

说明：检查E2PROM是否正常
这里用了24XX的最后一个地址(255)来存储标志字.
如果用其他24C系列,这个地址要修改
返回1:检测失败
返回0:检测成功

**********************************************************************************/
u8 E2PROM_Check(void)
{
	u8 temp;
	temp=E2PROM_ReadOneByte(255);//避免每次开机都写E2PROM			   
	if(temp==0X55)return 0;		   
	else//排除第一次初始化的情况
	{
		E2PROM_WriteOneByte(255,0X55);
	    temp=E2PROM_ReadOneByte(255);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}



/*********************************************************************************

说明：在E2PROM里面的指定地址开始读出指定个数的16位数组
ReadAddr :开始读出的地址 对24c02为0~255
pBuffer  :数据数组首地址
NumToRead:要读出数据的个数

**********************************************************************************/
u8 E2PROM_Read_U16(u16 ReadAddr,u16 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=E2PROM_ReadLenByte(ReadAddr,2);	
		ReadAddr += 2;
		NumToRead-=1;  //E2PROM每个地址1个字节，现在是对16位数据进行操作。
	}
	return 1;
}  

/*********************************************************************************

说明：在E2PROM里面的指定地址开始写入指定个数数组
WriteAddr :开始写入的地址 对24c02为0~255
pBuffer   :数据数组首地址
NumToWrite:要写入数据的个数
Len:数组数据长度，16位

**********************************************************************************/
u8 E2PROM_Write_U16(u16 WriteAddr,u16 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		E2PROM_WriteLenByte(WriteAddr,*pBuffer,2);
		WriteAddr+=2;  //E2PROM每个地址1个字节，现在是对16位数据进行操作。
		pBuffer++;
	}
	return 1;
}




