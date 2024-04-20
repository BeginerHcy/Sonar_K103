#include "BoxFun.h"

AGVBox_type AGVBox;
data_parse_fun CaptureFun1[]={PhoneCap,AMSCap,AGVCap,DoorCap,xChnCap,IMUCap,ComTestCap};
void AGVCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
	
};

void AMSCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
	
};
void ComTestCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
;
};
void IMUCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{


};
void PhoneCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
}
void DoorCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
};
void xChnCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg)
{
};
///////////
void GetNode(AGVIO_type * pBoxIO, AGVBox_type * pBoxMsg)
{
;
};

///////////
void GetChannel(AGVIO_type * pBoxIO, AGVBox_type * pBoxMsg)
{
};

void LinkIO(AGVIO_type * pBoxIO, AGVBox_type * pBoxMsg)
{
	
}
void ConfigChannel(AGVIO_type * pBoxIO, AGVBox_type * pBoxMsg)
{
}

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
