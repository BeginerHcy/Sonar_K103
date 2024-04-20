#include "BoxFun.h"



u16 CRC_Verify(u8 *CRC_Buf,u8 Buf_Length)
{
	u8 i,j;
	u16 wCrc = 0xffff;
	u16 wPolyNum = 0xA001;	
	for(i = 0;i < Buf_Length;i++)
	{
		wCrc ^= CRC_Buf[i];
		
		for(j=0;j<8;j++)
		{
			if(wCrc & 0x0001)
			{
				wCrc = (wCrc >> 1)^wPolyNum;
			}
			else
			{
				wCrc = wCrc>>1;
			}
		}
	}
	return wCrc;
}
