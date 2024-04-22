#include "BoxApp.h"
#include "delay.h"
#include "stm_flash.h"
#include "IOMap.h"
uint16_t NodenDis[10][3],pNowNode=0,NodeSum=2,TimeRecInterval=0;
uint8_t Parameter1[300] = {0};
uint8_t ReadTemp1[300] = {0};
uint32_t oldTimeStampForAlive,TimeAlive,timesExit[4],oldTimeStampForCnt=0,oldTimeStampFor4msCnt=0,oldTimeStampFor1sCnt;
u32 OldtimeStampCnt2,OldStamp[10],oldTimeStampForAlive,TimeAlive;
SonarDualData_type SonarDual={0};
bool firstBoot;
int16_t Temp[29];
uint8_t cntFlash;
///////////////////////////////////////////////////////////////////////////////

void cyc2ms(){}
void cyc4ms(){}
void cyc10ms(){}
void RecDistance(uint8_t Node){}

void cyc200ms(){}

void cyc100ms()
{
	cntFlash++;
	BoxIO.flagLed[DBLedFlash] = cntFlash==1||cntFlash==3;//������˸	
	if(cntFlash>=10)cntFlash=0;
	
}
void cyc500ms()
{
	BoxIO.flagLed[fLedFlash] = !BoxIO.flagLed[fLedFlash];//������˸
}
	
void cycLongTsk()
{
	BoxIO.flagLed[sLedFlash] = !BoxIO.flagLed[sLedFlash];//������˸
}


void _BoxTaskInit_()
{
	SystemConfig();						//����ϵͳ
	////////////////////////////////
	while(BoxIO.TimeStamp < 1000)
	{
		PVMap(&BoxIO);				 	//IOӳ���,�������,	
	}		
};
void _BoxTaskCyc_()
{
	while(TRUE)
	{
		SonarApp();
		PVMap(&BoxIO);				 //IOӳ���,�������,
	}
};
void SonarApp(){
	
	u8 pArea=0,i=0;
	uint8_t Parameter1[800] = {0};
	//////////////////
	SonarDual.bDI1 = !BoxIO.DI[0];
	SonarDual.bDI2 = !BoxIO.DI[1];
	SonarDual.bDI3 = !BoxIO.DI[2];
	SonarDual.bDI4 = !BoxIO.DI[3];
	
	BoxIO.LED[0] = BoxIO.flagLed[0];
	//BoxIO.LED[1] = BoxIO.flagLed[1];
	//BoxIO.LED[2] = BoxIO.flagLed[2];
	
	//////////////////
	if(BoxIO.TimeStamp - 1 > oldTimeStampForCnt || oldTimeStampForCnt > BoxIO.TimeStamp){//every 2 ms exeed;
		NodenDis[pNowNode][2]++;
		oldTimeStampForCnt = BoxIO.TimeStamp;
	}
	if(BoxIO.TimeStamp - 1000 > oldTimeStampFor1sCnt || oldTimeStampFor1sCnt > BoxIO.TimeStamp){//every 1 s exeed;
		if(SonarDual.cmdPCConnect){
			SonarDual.cmdSendRS232Data = 1;
		}
		oldTimeStampFor1sCnt = BoxIO.TimeStamp;
	}
	if(BoxIO.TimeStamp - 3 > oldTimeStampFor4msCnt || oldTimeStampFor4msCnt > BoxIO.TimeStamp){//every 4 ms exeed;
		//////////////////////////2 3 2////////////////////////////
		if(SonarDual.ReqSetOKAllow){
			SonarDual.ReqSetOKAllow = 0;
			BoxIO.Uart3Dat.sBuffer[0] = 0xAA;
			BoxIO.Uart3Dat.sBuffer[1] = 0x52;
			BoxIO.Uart3Dat.sBuffer[2] = 0x04;
			BoxIO.Uart3Dat.sBuffer[3] = 0x01;
			BoxIO.Uart3Dat.sBuffer[4] = 0x00;
			BoxIO.Uart3Dat.sBuffer[5] = 0x01;
			BoxIO.Uart3Dat.sBuffer[6] = 0xAC;
			BoxIO.Uart3Dat.sLen = 7;
			SonarDual.cmdSendRS232Data = 0;
			SendUrtBuf(&BoxIO.Uart3Dat,Uartx3);
		}
		else if(SonarDual.ReqConnectAllow){
			SonarDual.ReqConnectAllow = 0;
			BoxIO.Uart3Dat.sBuffer[0] = 0xAA;
			BoxIO.Uart3Dat.sBuffer[1] = 0x55;
			BoxIO.Uart3Dat.sBuffer[2] = 0x11;
			BoxIO.Uart3Dat.sBuffer[3] = 0x01;
			BoxIO.Uart3Dat.sBuffer[4] = 0x00;
			BoxIO.Uart3Dat.sBuffer[5] = gSystemPara.SonarNodeCHNA;
			BoxIO.Uart3Dat.sBuffer[6] = gSystemPara.SonarNodeCHNB;
			BoxIO.Uart3Dat.sBuffer[7] = gSystemPara.DataComInterface;
			BoxIO.Uart3Dat.sBuffer[8] = gSystemPara.RS485Node;
			BoxIO.Uart3Dat.sBuffer[9] = gSystemPara.CanNode>>8;
			BoxIO.Uart3Dat.sBuffer[10] = gSystemPara.CanNode;
			BoxIO.Uart3Dat.sBuffer[11] = gSystemPara.RS485Bauderate;
			BoxIO.Uart3Dat.sBuffer[12] = gSystemPara.RS232Bauderate;
			BoxIO.Uart3Dat.sBuffer[13] = gSystemPara.CANBusBauderate;

			///////////////////////////
			for(i=0;i<8;i++){
				BoxIO.Uart3Dat.sBuffer[12*i+0+19] = gSystemPara.Area[i].ComDisACHA>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+1+19] = gSystemPara.Area[i].ComDisACHA;
				BoxIO.Uart3Dat.sBuffer[12*i+2+19] = gSystemPara.Area[i].ComDisBCHA>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+3+19] = gSystemPara.Area[i].ComDisBCHA;
				BoxIO.Uart3Dat.sBuffer[12*i+4+19] = gSystemPara.Area[i].ComDisCCHA>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+5+19] = gSystemPara.Area[i].ComDisCCHA;	
				BoxIO.Uart3Dat.sBuffer[12*i+6+19] = gSystemPara.Area[i].ComDisACHB>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+7+19] = gSystemPara.Area[i].ComDisACHB;
				BoxIO.Uart3Dat.sBuffer[12*i+8+19] = gSystemPara.Area[i].ComDisBCHB>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+9+19] = gSystemPara.Area[i].ComDisBCHB;
				BoxIO.Uart3Dat.sBuffer[12*i+10+19] = gSystemPara.Area[i].ComDisCCHB>>8;
				BoxIO.Uart3Dat.sBuffer[12*i+11+19] = gSystemPara.Area[i].ComDisCCHB;	
			}
			///////////////////////////
			BoxIO.Uart3Dat.sBuffer[115] = 0xAC;
			BoxIO.Uart3Dat.sLen = 116;
			SonarDual.cmdSendRS232Data = 0;
			SendUrtBuf(&BoxIO.Uart3Dat,Uartx3);
		}
		
		else if(SonarDual.cmdSendRS232Data){
			SonarDual.cmdSendRS232Data = 0;		
			///////////////////////////////////////
			BoxIO.Uart3Dat.sBuffer[0] = 0xAA;
			BoxIO.Uart3Dat.sBuffer[1] = 0x53;
			BoxIO.Uart3Dat.sBuffer[2] = 0x29;
			BoxIO.Uart3Dat.sBuffer[3] = 0x01;
			BoxIO.Uart3Dat.sBuffer[4] = 0x00;
			///////////////////Payloads	///////////		
			BoxIO.Uart3Dat.sBuffer[5] 	= SonarDual.Internal.AreaDetectSwitch;
			BoxIO.Uart3Dat.sBuffer[6] 	= SonarDual.Internal.OutComState;
			BoxIO.Uart3Dat.sBuffer[7] 	= SonarDual.DistanCHA>>8;
			BoxIO.Uart3Dat.sBuffer[8] 	= SonarDual.DistanCHA;
			BoxIO.Uart3Dat.sBuffer[9] 	= SonarDual.DistanCHB>>8;
			BoxIO.Uart3Dat.sBuffer[10] 	= SonarDual.DistanCHB;
			BoxIO.Uart3Dat.sBuffer[11] 	= SonarDual.Internal.pAreaToCom;
			BoxIO.Uart3Dat.sBuffer[12] 	= SonarDual.ActID;
			BoxIO.Uart3Dat.sBuffer[13] 	= SonarDual.AliveCHA;
			BoxIO.Uart3Dat.sBuffer[14] 	= SonarDual.AliveCHB;
			BoxIO.Uart3Dat.sBuffer[15] 	= 0xAE;
			BoxIO.Uart3Dat.sLen 				= 16;	
			SendUrtBuf(&BoxIO.Uart3Dat,Uartx3);
		}
		oldTimeStampFor4msCnt = BoxIO.TimeStamp;
	}
	if(BoxIO.TimeStamp<oldTimeStampForCnt)
		oldTimeStampForCnt = BoxIO.TimeStamp;
	if(BoxIO.TimeStamp<oldTimeStampFor4msCnt)
		oldTimeStampFor4msCnt = BoxIO.TimeStamp;
	if(BoxIO.TimeStamp<oldTimeStampFor1sCnt)
		oldTimeStampFor1sCnt = BoxIO.TimeStamp;
	
	NodenDis[0][0] 	= gSystemPara.SonarNodeCHNA ;
	NodenDis[1][0] 	= gSystemPara.SonarNodeCHNB;
	
	BoxIO.Uart2Dat.sBuffer[0] = NodenDis[pNowNode][0];
	BoxIO.Uart2Dat.sBuffer[1] = 0x02;
	BoxIO.Uart2Dat.sBuffer[2] = 0xB0;
	BoxIO.Uart2Dat.sLen 			= 3;
	SonarDual.StatusStep = 10;
	SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
	delay_ms(1);
	SonarDual.StatusStep = 20;
		///////////////////////////////////////
	while(BoxIO.Uart2Dat.pRfil - BoxIO.Uart2Dat.pRder < 2 && NodenDis[pNowNode][2]/2<40){
		if(BoxIO.TimeStamp>oldTimeStampForAlive)
				TimeAlive+=(BoxIO.TimeStamp-oldTimeStampForAlive);
		oldTimeStampForAlive = BoxIO.TimeStamp;
		if(TimeAlive>100){
			break;
		}
	};
	if(BoxIO.Uart2Dat.pRfil>2){
		if((BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-1]==0x71||
			BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-1]==0x00) &&( 
			BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-3]==0xE2 || 
			BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-3]==0xE0)){
				SonarDual.ActID = BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-2];
			}
	}
	SonarDual.StatusStep = 30;
	if(BoxIO.Uart2Dat.pRfil - BoxIO.Uart2Dat.pRder > 1)
	{
		NodenDis[pNowNode][1] = BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-2]*256 + BoxIO.Uart2Dat.rBuffer[BoxIO.Uart2Dat.pRfil-1];
		BoxIO.Uart2Dat.pRder = BoxIO.Uart2Dat.pRfil;
		TimeRecInterval = NodenDis[pNowNode][2]/2;
		if(pNowNode==0)
			SonarDual.AliveCHA = 1;
		if(pNowNode==1)
			SonarDual.AliveCHB = 1;
	}
	else{
		BoxIO.Uart2Dat.pRder = BoxIO.Uart2Dat.pRfil;
		timesExit[pNowNode]++;
		if(timesExit[0]>10){
			SonarDual.AliveCHA = 0;
		}
		if(timesExit[1]>10){
			SonarDual.AliveCHB = 0;
		}
	}
	SonarDual.StatusStep 	= 40;
	TimeAlive 						= 0;
	NodenDis[pNowNode][2] = 0;
	///////////////////////////////////////
	SonarDual.DistanCHA = NodenDis[0][1];//
	SonarDual.DistanCHB = NodenDis[1][1];//
	///////////////////////////////////////	
	///////////////////////////////////////
	if(!SonarDual.bDI1 && !SonarDual.bDI2 && !SonarDual.bDI3 && !SonarDual.bDI4)
		SonarDual.Internal.AreaDetectSwitch = 0;
	else
		SonarDual.Internal.AreaDetectSwitch = 1;
	///////////////////////////////////////
	SonarDual.Internal.pAreaToCom				= SonarDual.bDI1*1 + SonarDual.bDI2*2 + SonarDual.bDI3*4;
	pArea = SonarDual.Internal.pAreaToCom;
	///////////////////////////////////////
	if(SonarDual.Internal.pAreaToCom==0){
		SonarDual.Internal.OutComState=0;
		SonarDual.Internal.AreaDetectSwitch = 0;
	}
	else
	{
		if(SonarDual.DistanCHA<gSystemPara.Area[pArea-1].ComDisCCHA || SonarDual.DistanCHB<gSystemPara.Area[pArea-1].ComDisCCHB)
			SonarDual.Internal.OutComState = 3;
		else if(SonarDual.DistanCHA<gSystemPara.Area[pArea-1].ComDisBCHA || SonarDual.DistanCHB<gSystemPara.Area[pArea-1].ComDisBCHB || !SonarDual.AliveCHA || !SonarDual.AliveCHB)
			SonarDual.Internal.OutComState = 2;
		else if(SonarDual.DistanCHA<gSystemPara.Area[pArea-1].ComDisACHA || SonarDual.DistanCHB<gSystemPara.Area[pArea-1].ComDisACHB)
			SonarDual.Internal.OutComState = 1;
		else
			SonarDual.Internal.OutComState = 0;
	}
	///////////////////////////////////////
	switch(SonarDual.Internal.OutComState){
		
		case 0://No Detect
			SonarDual.bDO1 = 0;
			SonarDual.bDO2 = 0;
			SonarDual.bDO3 = 0;
		  BoxIO.LED[1] = 0;
			break;
		
		case 1://Detect A
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 0;
			SonarDual.bDO3 = 0;
			BoxIO.LED[1] = BoxIO.flagLed[sLedFlash];
			break;
		
		case 2://Detect B
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 1;
			SonarDual.bDO3 = 0;
			BoxIO.LED[1] = BoxIO.flagLed[fLedFlash];
			break;
		
		case 3://Detect C		
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 1;
			SonarDual.bDO3 = 1;
			BoxIO.LED[1] = BoxIO.flagLed[DBLedFlash];
			break;	
	}
	
	BoxIO.DO[0] = !SonarDual.bDO1;
	BoxIO.DO[1] = !SonarDual.bDO2;
	BoxIO.DO[2] = !SonarDual.bDO3;
	
	BoxIO.LED[2] = !SonarDual.bDO1;
	BoxIO.LED[3] = !SonarDual.bDO2;
	BoxIO.LED[4] = !SonarDual.bDO3;
	
	///////////////////////////////////////
	pNowNode++;
	if(pNowNode>=NodeSum)pNowNode=0;

	////////////Data flow
	if(HandShakeCap(&BoxIO.Uart3Dat)){//RS232
		SonarDual.cmdPCConnect = 1;
		SonarDual.ReqConnectAllow = 1;
	}
	if(PCChangeParCap(&BoxIO.Uart3Dat)){//RS232
		gSystemPara.cmdSaveParameter = 1;
	}
	if(PCCalSensor(&BoxIO.Uart3Dat)){//RS232
		BoxIO.Uart2Dat.sBuffer[0] = SonarDual.ActID;
		BoxIO.Uart2Dat.sBuffer[1] = 0x02;
		BoxIO.Uart2Dat.sBuffer[2] = 0x9A;
		BoxIO.Uart2Dat.sLen 			= 3;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
		delay_ms(10);
		BoxIO.Uart2Dat.sBuffer[0] = SonarDual.ActID;
		BoxIO.Uart2Dat.sBuffer[1] = 0x02;
		BoxIO.Uart2Dat.sBuffer[2] = 0x92;
		BoxIO.Uart2Dat.sLen 			= 3;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
		delay_ms(10);
		BoxIO.Uart2Dat.sBuffer[0] = SonarDual.ActID;
		BoxIO.Uart2Dat.sBuffer[1] = 0x02;
		BoxIO.Uart2Dat.sBuffer[2] = 0x9E;
		BoxIO.Uart2Dat.sLen 			= 3;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
		delay_ms(10);
		BoxIO.Uart2Dat.sBuffer[0] = SonarDual.ActID;
		BoxIO.Uart2Dat.sBuffer[1] = 0x02;
		BoxIO.Uart2Dat.sBuffer[2] = SonarDual.SetID;
		BoxIO.Uart2Dat.sLen 			= 3;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
		delay_ms(10);
		SonarDual.ReqSetOKAllow = 1;
	}
	if(ChangeFilterCap(&BoxIO.Uart3Dat)){//RS232
		BoxIO.Uart2Dat.sBuffer[0] = SonarDual.ActID;
		BoxIO.Uart2Dat.sBuffer[1] = 0x02;
		BoxIO.Uart2Dat.sBuffer[2] = SonarDual.FltPar;
		BoxIO.Uart2Dat.sLen 			= 3;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
		SonarDual.ReqSetOKAllow = 1;
	}
	//////////////////////////////////////////////////
	
	if(gSystemPara.cmdSaveParameter)
	{
		static uint16_t sizePar = sizeof(gSystemPara);
		gSystemPara.cmdSaveParameter = 0;
		SonarDual.ReqSetOKAllow = 1;
		memcpy(&Parameter1,&gSystemPara,sizeof(gSystemPara));
		Parameter1[299] = 0xCD;
		STMFLASH_Write(FLASH_SAVE_ADDR,( uint16_t * )Parameter1,300);
	}	
};

bool PCChangeParCap(UrtBuf_type * pSrcBuf){
	uint8_t filled = pSrcBuf->pRfil,iDat;
	uint8_t degred = pSrcBuf->pRder;

	uint8_t chnOKBfLen = 116;
	
	bool result=0;
	////////////////
	result = 0;
	////////////////
	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(filled >= degred+ chnOKBfLen){
				/////////////////////////////////////
				if( pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x54&&
					pSrcBuf->rBuffer[degred+2]==0x11&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+115]==0xAC){
						
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					gSystemPara.SonarNodeCHNA = pSrcBuf->rBuffer[degred+5];
					gSystemPara.SonarNodeCHNB = pSrcBuf->rBuffer[degred+6];
					gSystemPara.DataComInterface = pSrcBuf->rBuffer[degred+7];
					gSystemPara.RS485Node = pSrcBuf->rBuffer[degred+8];
					gSystemPara.CanNode = pSrcBuf->rBuffer[degred+9]*256 + pSrcBuf->rBuffer[degred+10];
					gSystemPara.RS485Bauderate = pSrcBuf->rBuffer[degred+11];
					gSystemPara.RS232Bauderate = pSrcBuf->rBuffer[degred+12];
					gSystemPara.CANBusBauderate = pSrcBuf->rBuffer[degred+13];
					for(iDat=0;iDat<8;iDat++)
					{
						gSystemPara.Area[iDat].ComDisACHA = pSrcBuf->rBuffer[degred+ 19 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+20 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisBCHA = pSrcBuf->rBuffer[degred+ 21 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+22 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisCCHA = pSrcBuf->rBuffer[degred+ 23 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+24 + 0 + iDat*12];
						
						gSystemPara.Area[iDat].ComDisACHB = pSrcBuf->rBuffer[degred+ 25 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+26 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisBCHB = pSrcBuf->rBuffer[degred+ 27 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+28 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisCCHB = pSrcBuf->rBuffer[degred+ 29 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+30 + 0 + iDat*12];
					}				
					result = 1;
				}
			}
		}
	}
	return result;
}

bool ChangeFilterCap(UrtBuf_type * pSrcBuf){
	
	uint8_t filled = pSrcBuf->pRfil,SensorIndex;
	uint8_t degred = pSrcBuf->pRder;
	uint8_t iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	uint8_t chnOKBfLen = 14;
	uint16_t cycResult;
	bool result=0;
	///
	result = 0;
	///
	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(filled >= degred+ chnOKBfLen){
				if( pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x56&&
					pSrcBuf->rBuffer[degred+2]==0x0B&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+13]==0xAD)
				{
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					SonarDual.FltPar = pSrcBuf->rBuffer[degred+5];
					result = 1;
				}
			}
		}
	}
	return result;
}

bool HandShakeCap(UrtBuf_type * pSrcBuf){
	uint8_t filled = pSrcBuf->pRfil;
	uint8_t degred = pSrcBuf->pRder;
	
	uint8_t chnOKBfLen = 7;
	
	bool result=0;
	///
	result = 0;
	///
	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(filled >= degred+ chnOKBfLen){
				/////////////////////////////////////
				if(pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x55&&
					pSrcBuf->rBuffer[degred+2]==0x04&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+5]==0x01&&
					pSrcBuf->rBuffer[degred+6]==0xAB){
						
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					result = 1;
				}
			}
		}
	}
	return result;
}
bool PCCalSensor(UrtBuf_type * pSrcBuf){
	uint8_t filled = pSrcBuf->pRfil,SensorIndex;
	uint8_t degred = pSrcBuf->pRder;
	uint8_t iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	uint8_t chnOKBfLen = 14;
	uint16_t cycResult;	
	bool result=0;
	///
	result = 0;
	///

	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(filled >= degred+ chnOKBfLen){
				if( pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x51&&
					pSrcBuf->rBuffer[degred+2]==0x0B&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+13]==0xAC)
				{
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					SonarDual.SetID = pSrcBuf->rBuffer[degred+5];
					result = 1;
				}
			}
		}
	}
	return result;
}