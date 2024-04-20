#include "Maths.h"

BOOL XORD(unsigned char* pBuffer, unsigned short BufferSize)
{
	/*TODO: Add your code here*/
	return 1;
};

BOOL Rotat2D(struct Pos2D* pPos2D,double deltaX, double Yaw)
{
	pPos2D->yaw += Yaw;
	pPos2D->x 	= pPos2D->x + deltaX * cos(pPos2D->yaw);
	pPos2D->y 	= pPos2D->y + deltaX * sin(pPos2D->yaw);
	return 1;
};

double STDeviate(unsigned long pDataArray, unsigned char DataLen)
{
	int16_t data[1000],aveData,iData,iData2;
	int32_t DataSum=0,DataSum2=0;
	double  Result;
	
	mymemcpy(&(data[0]),(void*)pDataArray,DataLen*sizeof(int32_t));
	for (iData = 0; iData < DataLen; iData++)
	{
		DataSum = data[iData]+ DataSum;
	}
	aveData = DataSum/DataLen;
	for (iData2 = 0; iData2 < DataLen; iData2++)
	{
		DataSum2 = (data[iData2]-aveData)*(data[iData2]-aveData)+ DataSum2;
	}
	return aveData;
};
BOOL Transtlate2D(struct Pos2D* pPos2D, double X, double Y)
{
	pPos2D->x += X;
	pPos2D->y += Y;	
	return 1;
}
double MIN(double a,double b)
{
	double tempa=a;
	double tempb=b;
	if(tempa>tempb) return tempb;
	else return tempa;
}

void ArryMLO(double* buf,u32 bufByte)
{
	//u32 bufByte = sizeof(buf)/sizeof(buf[0]);
	u32 iBuf=0;
	while(iBuf+1<bufByte)
	{
		iBuf++;
		buf[bufByte-iBuf] = buf[bufByte-iBuf-1];
	}
};
double MoveAvgFilter(MoveStrutType * pFunData)
{
	double SumBuffer=0;
	u32 indexCal=0;
	double temN=pFunData->Ntime;//Byte
	u32 BufSizeByte=sizeof(pFunData->buffer)/sizeof(double);
	u32 BufLenMin=sizeof(pFunData->buffer)-sizeof(double);
	///////
	ArryMLO(&pFunData->buffer[0],BufSizeByte);
	///////
	pFunData->buffer[0] = pFunData->x;
	//////
	if(temN>=BufSizeByte) temN=BufSizeByte;
	
	if(temN==0) temN=1;
	//////
	pFunData->Nfilled++;
	//////
	if(pFunData->Nfilled>=temN)pFunData->Nfilled=temN;
	SumBuffer=0;
	/////
	for(indexCal=0;indexCal<pFunData->Nfilled;indexCal++)
	{
		 SumBuffer = SumBuffer + pFunData->buffer[indexCal];
	}
	/////
	pFunData->y =  SumBuffer/pFunData->Nfilled;
	return pFunData->y;
	
};
u8 XorCheckSum(u8* buf,u8 bufLen)
{
  unsigned int packet_size=bufLen;
  u8 cs=0;
	unsigned int i;
  for (i = 2; i < packet_size; i++)
  {
    cs ^= buf[i];
  }
  return cs;
}