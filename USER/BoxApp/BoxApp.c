#include "BoxApp.h"
#include "delay.h"
#include "stm_flash.h"
IMU_type ADs16445;
BoxPhone_type BoxPhone;
WorkSpaceCfg_type WkSpcCfg;
BoxTest433_type BoxTest433;
BoxDoor_type BoxDoor;
SonarDualData_type SonarDual;
uint16_t NodenDis[10][3],pNowNode=0,NodeSum=2,TimeRecInterval=0;
agv_mode_App AppTask[]={phoneApp,AMSApp,AGVApp,DoorApp,CfgApp,IMUApp,ComTest};
u16 IMUTemperature;
uint8_t Parameter1[200] = {0};
uint8_t ReadTemp1[200] = {0};
u32 OldtimeStampCnt2,OldStamp[10],oldTimeStampForAlive,TimeAlive,timesExit[2];
bool firstBoot;
int16_t Temp[29];
uint8_t cntFlash;
RFID_Type Readers;
data_parse_fun CaptureFun[]={PhoneCap,AMSCap,AGVCap,DoorCap,xChnCap,IMUCap,ComTestCap};
///////////////////////////////////////////////////////////////////////////////
u8 BuiltStr(UrtBuf_type * UartDat,AGVBox_type * pBoxMsg,str2Com comType)
{
	u8 TempBuf[]={0xA2,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x0D,0xDA};
	u8 iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	u8 lenBuf=0,iNode=0,inodenode=0;
	u8 TempBufcnf[]={0xA7,0x55,0x01,0x00,0x01,0xFF,0x01,0x02,0x03,0x04};
	
	u8 TempBufPC[]={0xA7,0x55,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	
	switch(comType)
	{
		case PhCalStr:
			TempBuf[iSrcSta] 		= pBoxMsg->Monitor.NodeNum;
			TempBuf[iDstSta] 		= WkSpcCfg.AMSStation;
			TempBuf[iLenCode] 	= 1;
			TempBuf[iFunCode] 	= phCalCde;
			lenBuf							= 10;
		  TempBuf[iFunCode+1] = 0;  
			TempBuf[iFunCode+2] = CRC_Verify(&(TempBuf[iSrcSta]),lenBuf-5);//CRC,lenBuf是总长度，头尾共3个字节+2Byte结束符
			TempBuf[iFunCode+3] = 0x0D;
			TempBuf[iFunCode+4] = 0xDA;
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBuf),lenBuf);
			break;
		
		case DoorMsg:
			TempBuf[iSrcSta] 		= pBoxMsg->Monitor.NodeNum;
			TempBuf[iDstSta] 		= WkSpcCfg.AMSStation;
			TempBuf[iLenCode] 	= 2;
			TempBuf[iFunCode] 	= doorMsgCde;
			lenBuf							= 10;
			if(pBoxMsg->Monitor.DoorLmt[UpLimit])
			{
				TempBuf[iFunCode+1] = 1;
			}
			else
			{
				TempBuf[iFunCode+1] = 0;
			}
			TempBuf[iFunCode+2] = CRC_Verify(&(TempBuf[iSrcSta]),lenBuf-5);//CRC,lenBuf是总长度，头尾共3个字节+2Byte结束符
			TempBuf[iFunCode+3] = 0x0D;
			TempBuf[iFunCode+4] = 0xDA;
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBuf),lenBuf);		
			break;

		case CnfirmFinish:
			TempBuf[iSrcSta] 		= pBoxMsg->Monitor.NodeNum;
			TempBuf[iDstSta] 		= WkSpcCfg.AMSStation;
			TempBuf[iLenCode] 	= 2;
			TempBuf[iFunCode] 	= CnfrmCde;
			lenBuf							= 11;
			TempBuf[iFunCode+1] = 0;
			TempBuf[iFunCode+2] = phRunCde;
			TempBuf[iFunCode+3] = CRC_Verify(&(TempBuf[iSrcSta]),lenBuf-5);//CRC,lenBuf是总长度，头尾共3个字节+2Byte结束符
			TempBuf[iFunCode+4] = 0x0D;
			TempBuf[iFunCode+5] = 0xDA;
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBuf),lenBuf);
			break;	
		
		case MasterConfirm433:
			TempBufcnf[iSrcSta] 		= 1;
			TempBufcnf[iDstSta] 		= BoxTest433.State.RequestNum;
			TempBufcnf[iLenCode] 	= 1;
			TempBufcnf[iFunCode] 	= B433IntiCde;
			lenBuf							= 7;//6+n
		  TempBufcnf[iFunCode+1] = 1;  
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBufcnf),lenBuf);			
			break;
		
		case SlaveB433HeartBeat:
			TempBufcnf[iSrcSta] 		= pBoxMsg->Monitor.Channel;
			TempBufcnf[iDstSta] 		= 1;
			TempBufcnf[iLenCode] 		= 4;
			TempBufcnf[iFunCode] 		= B433HeartCde;
			lenBuf									= 10;//6+n
		  TempBufcnf[iFunCode+1] 	= BoxTest433.State.selfHeartBeat>>24;  
			TempBufcnf[iFunCode+2] 	= BoxTest433.State.selfHeartBeat>>16; 
			TempBufcnf[iFunCode+3] 	= BoxTest433.State.selfHeartBeat>>8; 
			TempBufcnf[iFunCode+4] 	= BoxTest433.State.selfHeartBeat>>0; 
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBufcnf),lenBuf);			
			break;
		case B433InitReq:
			TempBufcnf[iSrcSta] 		= pBoxMsg->Monitor.Channel;
			TempBufcnf[iDstSta] 		= 1;
			TempBufcnf[iLenCode] 		= 1;
			TempBufcnf[iFunCode] 		= B433IntiCde;
			lenBuf									= 7;//6+n
		  TempBufcnf[iFunCode+1] 	= 1;  
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBufcnf),lenBuf);			
			break;

		case B433FbkPC:
			iNode=0;
			inodenode=0;
			TempBufPC[iNode+2] = BoxTest433.State.ActHeartBeatSlaveM1[inodenode];
			TempBufPC[iNode+3] = BoxTest433.State.ActHeartBeatSlaveM2[inodenode];
			TempBufPC[iNode+4] = BoxTest433.State.ActHeartBeatSlaveM3[inodenode];
			TempBufPC[iNode+5] = BoxTest433.State.ActHeartBeatSlaveM4[inodenode];
			TempBufPC[iNode+6] = BoxTest433.State.HeartBeatSlave[inodenode]>>24;
			TempBufPC[iNode+7] = BoxTest433.State.HeartBeatSlave[inodenode]>>16;
			TempBufPC[iNode+8] = BoxTest433.State.HeartBeatSlave[inodenode]>>8;
			TempBufPC[iNode+9] = BoxTest433.State.HeartBeatSlave[inodenode]>>0;
			
			iNode=8;
			inodenode=1;
			TempBufPC[iNode+2] = BoxTest433.State.ActHeartBeatSlaveM1[inodenode];
			TempBufPC[iNode+3] = BoxTest433.State.ActHeartBeatSlaveM2[inodenode];
			TempBufPC[iNode+4] = BoxTest433.State.ActHeartBeatSlaveM3[inodenode];
			TempBufPC[iNode+5] = BoxTest433.State.ActHeartBeatSlaveM4[inodenode];
			TempBufPC[iNode+6] = BoxTest433.State.HeartBeatSlave[inodenode]>>24;
			TempBufPC[iNode+7] = BoxTest433.State.HeartBeatSlave[inodenode]>>16;
			TempBufPC[iNode+8] = BoxTest433.State.HeartBeatSlave[inodenode]>>8;
			TempBufPC[iNode+9] = BoxTest433.State.HeartBeatSlave[inodenode]>>0;		
			
			iNode=16;
			inodenode=2;
			TempBufPC[iNode+2] = BoxTest433.State.ActHeartBeatSlaveM1[inodenode];
			TempBufPC[iNode+3] = BoxTest433.State.ActHeartBeatSlaveM2[inodenode];
			TempBufPC[iNode+4] = BoxTest433.State.ActHeartBeatSlaveM3[inodenode];
			TempBufPC[iNode+5] = BoxTest433.State.ActHeartBeatSlaveM4[inodenode];
			TempBufPC[iNode+6] = BoxTest433.State.HeartBeatSlave[inodenode]>>24;
			TempBufPC[iNode+7] = BoxTest433.State.HeartBeatSlave[inodenode]>>16;
			TempBufPC[iNode+8] = BoxTest433.State.HeartBeatSlave[inodenode]>>8;
			TempBufPC[iNode+9] = BoxTest433.State.HeartBeatSlave[inodenode]>>0;	
			
			iNode=24;
			inodenode=3;
			TempBufPC[iNode+2] = BoxTest433.State.ActHeartBeatSlaveM1[inodenode];
			TempBufPC[iNode+3] = BoxTest433.State.ActHeartBeatSlaveM2[inodenode];
			TempBufPC[iNode+4] = BoxTest433.State.ActHeartBeatSlaveM3[inodenode];
			TempBufPC[iNode+5] = BoxTest433.State.ActHeartBeatSlaveM4[inodenode];
			TempBufPC[iNode+6] = BoxTest433.State.HeartBeatSlave[inodenode]>>24;
			TempBufPC[iNode+7] = BoxTest433.State.HeartBeatSlave[inodenode]>>16;
			TempBufPC[iNode+8] = BoxTest433.State.HeartBeatSlave[inodenode]>>8;
			TempBufPC[iNode+9] = BoxTest433.State.HeartBeatSlave[inodenode]>>0;	
		
			lenBuf = 34;
			UartDat->sLen = lenBuf;
			memcpy(UartDat->sBuffer,&(TempBufPC),lenBuf);	
			
			break;
		
		default:
			break;
	};
	UartDat->flagS = TRUE;
	return lenBuf;
};

void cyc2ms(){NodenDis[pNowNode][2]++;}
void cyc4ms()
{
	u8 i;
	//////////////////////////2 3 2////////////////////////////
	if(Readers.ReqSetOKAllow){
		Readers.ReqSetOKAllow = 0;
		BoxIO.Uart2Dat.sBuffer[0] = 0xAA;
		BoxIO.Uart2Dat.sBuffer[1] = 0x52;
		BoxIO.Uart2Dat.sBuffer[2] = 0x04;
		BoxIO.Uart2Dat.sBuffer[3] = 0x01;
		BoxIO.Uart2Dat.sBuffer[4] = 0x00;
		BoxIO.Uart2Dat.sBuffer[5] = 0x01;
		BoxIO.Uart2Dat.sBuffer[6] = 0xAC;
		BoxIO.Uart2Dat.sLen = 7;
		Readers.cmdSendRS232Data = 0;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);

	}
	else if(Readers.ReqConnectAllow){
		Readers.ReqConnectAllow = 0;
		BoxIO.Uart2Dat.sBuffer[0] = 0xAA;
		BoxIO.Uart2Dat.sBuffer[1] = 0x55;
		BoxIO.Uart2Dat.sBuffer[2] = 0x11;
		BoxIO.Uart2Dat.sBuffer[3] = 0x01;
		BoxIO.Uart2Dat.sBuffer[4] = 0x00;
		BoxIO.Uart2Dat.sBuffer[5] = gSystemPara.BufferHead;
		BoxIO.Uart2Dat.sBuffer[6] = gSystemPara.DetectPolar;
		BoxIO.Uart2Dat.sBuffer[7] = gSystemPara.DataComInterface;
		BoxIO.Uart2Dat.sBuffer[8] = gSystemPara.RS485Node;
		BoxIO.Uart2Dat.sBuffer[9] = gSystemPara.CanNode>>8;
		BoxIO.Uart2Dat.sBuffer[10] = gSystemPara.CanNode;
		BoxIO.Uart2Dat.sBuffer[11] = gSystemPara.RS485Bauderate;
		BoxIO.Uart2Dat.sBuffer[12] = gSystemPara.RS232Bauderate;
		BoxIO.Uart2Dat.sBuffer[13] = gSystemPara.CANBusBauderate;
		BoxIO.Uart2Dat.sBuffer[14] = gSystemPara.DataAnsMethod;
		BoxIO.Uart2Dat.sBuffer[15] = gSystemPara.RequestInterval;
		BoxIO.Uart2Dat.sBuffer[16] = gSystemPara.MagSensity;
		BoxIO.Uart2Dat.sBuffer[17] = gSystemPara.MagTapWide;
		BoxIO.Uart2Dat.sBuffer[18] = gSystemPara.MountDir;
		///////////////////////////
		for(i=0;i<8;i++){
			BoxIO.Uart2Dat.sBuffer[12*i+0+19] = gSystemPara.Area[i].ComDisACHA>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+1+19] = gSystemPara.Area[i].ComDisACHA;
			BoxIO.Uart2Dat.sBuffer[12*i+2+19] = gSystemPara.Area[i].ComDisBCHA>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+3+19] = gSystemPara.Area[i].ComDisBCHA;
			BoxIO.Uart2Dat.sBuffer[12*i+4+19] = gSystemPara.Area[i].ComDisCCHA>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+5+19] = gSystemPara.Area[i].ComDisCCHA;	
			BoxIO.Uart2Dat.sBuffer[12*i+6+19] = gSystemPara.Area[i].ComDisACHB>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+7+19] = gSystemPara.Area[i].ComDisACHB;
			BoxIO.Uart2Dat.sBuffer[12*i+8+19] = gSystemPara.Area[i].ComDisBCHB>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+9+19] = gSystemPara.Area[i].ComDisBCHB;
			BoxIO.Uart2Dat.sBuffer[12*i+10+19] = gSystemPara.Area[i].ComDisCCHB>>8;
			BoxIO.Uart2Dat.sBuffer[12*i+11+19] = gSystemPara.Area[i].ComDisCCHB;	
		}
		///////////////////////////
		BoxIO.Uart2Dat.sBuffer[115] = 0xAC;
		BoxIO.Uart2Dat.sLen = 116;
		Readers.cmdSendRS232Data = 0;
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
	}
	
	else if(Readers.cmdSendRS232Data){
		Readers.cmdSendRS232Data = 0;		
		/////////////
		BoxIO.Uart2Dat.sBuffer[0] = 0xAA;
		BoxIO.Uart2Dat.sBuffer[1] = 0x53;
		BoxIO.Uart2Dat.sBuffer[2] = 0x29;
		BoxIO.Uart2Dat.sBuffer[3] = 0x01;
		BoxIO.Uart2Dat.sBuffer[4] = 0x00;
		///////////////////Payloads			
		BoxIO.Uart2Dat.sBuffer[5] 	= SonarDual.Internal.AreaDetectSwitch;
		BoxIO.Uart2Dat.sBuffer[6] 	= SonarDual.Internal.OutComState;
		BoxIO.Uart2Dat.sBuffer[7] 	= SonarDual.DistanCHA>>8;
		BoxIO.Uart2Dat.sBuffer[8] 	= SonarDual.DistanCHA;
		BoxIO.Uart2Dat.sBuffer[9] 	= SonarDual.DistanCHB>>8;
		BoxIO.Uart2Dat.sBuffer[10] 	= SonarDual.DistanCHB;
		BoxIO.Uart2Dat.sBuffer[11] 	= SonarDual.Internal.pAreaToCom;
		BoxIO.Uart2Dat.sBuffer[12] 	= SonarDual.StatusStep;
		BoxIO.Uart2Dat.sBuffer[13] 	= SonarDual.AliveCHA;
		BoxIO.Uart2Dat.sBuffer[14] 	= SonarDual.AliveCHB;
		BoxIO.Uart2Dat.sBuffer[15] 	= 0xAE;
		BoxIO.Uart2Dat.sLen 				= 16;	
		SendUrtBuf(&BoxIO.Uart2Dat,Uartx2);
	}
}
void cyc10ms()
{


}
void RecDistance(uint8_t Node){
	
	u8 pArea=0,i;
	BoxIO.Uart1Dat.sBuffer[0] = NodenDis[Node][0];
	BoxIO.Uart1Dat.sBuffer[1] = 0x02;
	BoxIO.Uart1Dat.sBuffer[2] = 0xBC;
	BoxIO.Uart1Dat.sLen 			= 3;
	SonarDual.StatusStep = 10;
	SendUrtBuf(&BoxIO.Uart1Dat,Uartx1);
	delay_ms(1);
	SonarDual.StatusStep = 20;
	///////////////////////////////////////
	while(BoxIO.Uart1Dat.pRfil - BoxIO.Uart1Dat.pRder < 2 && NodenDis[Node][2]/2<40){
		if(BoxIO.timeStamp>oldTimeStampForAlive)
				TimeAlive+=(BoxIO.timeStamp-oldTimeStampForAlive);
		///////////////////////////////////////////////////////
		oldTimeStampForAlive = BoxIO.timeStamp;
		if(TimeAlive>100){
			break;
		}
	};
	SonarDual.StatusStep = 30;
	if(BoxIO.Uart1Dat.pRfil - BoxIO.Uart1Dat.pRder > 1)
	{
		NodenDis[Node][1] = BoxIO.Uart1Dat.rBuffer[BoxIO.Uart1Dat.pRfil-2]*256 + BoxIO.Uart1Dat.rBuffer[BoxIO.Uart1Dat.pRfil-1];
		BoxIO.Uart1Dat.pRder = BoxIO.Uart1Dat.pRfil;
		TimeRecInterval = NodenDis[Node][2]/2;
		if(Node==0)
			SonarDual.AliveCHA = 1;
		if(Node==1)
			SonarDual.AliveCHB = 1;
	}
	else{
		BoxIO.Uart1Dat.pRder = BoxIO.Uart1Dat.pRfil;
		timesExit[Node]++;
		if(timesExit[0]>10){
			SonarDual.AliveCHA = 0;
		}
		if(timesExit[1]>10){
			SonarDual.AliveCHB = 0;
		}
	}
	SonarDual.StatusStep = 40;
	TimeAlive 				= 0;
	NodenDis[Node][2] = 0;
	///////////////////////////////////////
	SonarDual.DistanCHA = NodenDis[0][1];//
	SonarDual.DistanCHB = NodenDis[1][1];//
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
			break;
		
		case 1://Detect A
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 0;
			SonarDual.bDO3 = 0;
			break;
		
		case 2://Detect B
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 1;
			SonarDual.bDO3 = 0;
			break;
		
		case 3://Detect C		
			SonarDual.bDO1 = 1;
			SonarDual.bDO2 = 1;
			SonarDual.bDO3 = 1;
			break;	
	}
}

void cyc200ms()
{
}

void cyc100ms()
{
	cntFlash++;
	BoxIO.flagLed[DBLedFlash] = cntFlash==1||cntFlash==3;//快速闪烁	
	if(cntFlash>=10)cntFlash=0;
	
	//////
	if(Readers.cmdPCConnect){
		Readers.cmdSendRS232Data = 1;
	}
}
void cyc500ms()
{
	BoxIO.flagLed[fLedFlash] = !BoxIO.flagLed[fLedFlash];//快速闪烁
}
	
void cycLongTsk()
{
	BoxIO.flagLed[sLedFlash] = !BoxIO.flagLed[sLedFlash];//慢速闪烁
}

u8 BuiltVar(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg,str2Com comType)
{
	u8 funcode=0,srcSta=0,dstSta=0,datPar[20],datLen;
	u8 iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;;
		
	switch(comType)
	{
		case PhCalStr:
			if(pBoxMsg->Monitor.UrtBuf[0]==0xA2 && pBoxMsg->Monitor.UrtBuf[1]==0x55)
			{
				funcode = pBoxMsg->Monitor.UrtBuf[iFunCode];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				dstSta  = pBoxMsg->Monitor.UrtBuf[iDstSta];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				datLen  = pBoxMsg->Monitor.UrtBuf[iLenCode];
				memcpy(&(datPar),&(pBoxMsg->Monitor.UrtBuf[6]),datLen);
			}
			if(srcSta==WkSpcCfg.AMSStation && dstSta==pBoxMsg->Monitor.NodeNum)
			{
				if(funcode==CnfrmCde && datPar[1]==phCalCde)
				{
					if(datPar[0]==0)BoxPhone.Command.OrderRec    = 1;	
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
				if(funcode==phRunCde && datPar[1]==phCalCde)
				{
					if(datPar[0]==0)BoxPhone.Command.Finish 		 = 1;
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
			}
			break;
			
		case DoorCmd:
			if(pBoxMsg->Monitor.UrtBuf[0]==0xA2 && pBoxMsg->Monitor.UrtBuf[1]==0x55)
			{
				funcode = pBoxMsg->Monitor.UrtBuf[iFunCode];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				dstSta  = pBoxMsg->Monitor.UrtBuf[iDstSta];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				datLen  = pBoxMsg->Monitor.UrtBuf[iLenCode];
				memcpy(&(datPar),&(pBoxMsg->Monitor.UrtBuf[6]),datLen);
			}
			if(srcSta==WkSpcCfg.AMSStation && dstSta==pBoxMsg->Monitor.NodeNum)
			{
				if(funcode==doorCtlCde)
				{
					if(datPar[0]==1)BoxDoor.Command.openDoor     = 1;
					if(datPar[0]==0)BoxDoor.Command.closeDoor  	 = 1;
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
			}			
			break;
			
		case B433Test:
			if(pBoxMsg->Monitor.UrtBuf[0]==0xA7 && pBoxMsg->Monitor.UrtBuf[1]==0x55)
			{
				funcode = pBoxMsg->Monitor.UrtBuf[iFunCode];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				dstSta  = pBoxMsg->Monitor.UrtBuf[iDstSta];
				srcSta  = pBoxMsg->Monitor.UrtBuf[iSrcSta];
				datLen  = pBoxMsg->Monitor.UrtBuf[iLenCode];
				memcpy(&(datPar),&(pBoxMsg->Monitor.UrtBuf[6]),datLen);
			}
			if(srcSta!=1 && dstSta==1 && BoxTest433.State.Node == MasterNode)//Slave Request
			{
				if(funcode==B433IntiCde)//Intial Request
				{
					BoxTest433.State.RequestNum = srcSta;
					BoxTest433.Command.IntialRequest = 1;
					BoxTest433.State.HeartBeatSlave[srcSta] = 0;
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
				else if(funcode==B433HeartCde){
					BoxTest433.State.HeartBeatSlave[srcSta]++;
					BoxTest433.State.ActHeartBeatSlaveM1[srcSta] = datPar[0];
					BoxTest433.State.ActHeartBeatSlaveM2[srcSta] = datPar[1];
					BoxTest433.State.ActHeartBeatSlaveM3[srcSta] = datPar[2];
					BoxTest433.State.ActHeartBeatSlaveM4[srcSta] = datPar[3];					
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
			}
			else if(srcSta==1 && dstSta==pBoxMsg->Monitor.Channel && BoxTest433.State.Node == SlaveNode)
			{
				if(funcode==B433IntiCde)//Intial Request
				{
					BoxTest433.Command.IntialConfirm = 1;
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);
				}
			}
			else
			{
					memset(&(pBoxMsg->Monitor.UrtBuf),0,UrtBfLen);			
			}
			break;
		
		default:
			break;
	}
	return 1;
};
void IMUApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{

};
void CfgApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{
	
};
void ComTest(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg){
	
	if(1==AGVBox.Monitor.Channel)BoxTest433.State.Node = MasterNode;
	else BoxTest433.State.Node = SlaveNode;
	
	switch(BoxTest433.State.Node)
	{
		case MasterNode:
			BuiltVar(pBoxIO,pBoxMsg,B433Test);
			if(BoxTest433.Command.IntialRequest==1)
			{
				BoxTest433.Command.IntialRequest = 0;
				BuiltStr(&BoxIO.Uart2Dat,&AGVBox,MasterConfirm433);//send confirm
			}
			BuiltStr(&BoxIO.Uart1Dat,&AGVBox,B433FbkPC);//send confirm			
			break;
		
		
		case SlaveNode:
			switch(BoxTest433.State.State)
			{
				case B433Stanby:
					BoxTest433.State.State = B433Intial;
					BoxTest433.Command.IntialRequest = 1;
					break;
				
				case B433Intial:		
					BuiltStr(&BoxIO.Uart2Dat,&AGVBox,B433InitReq);//send request
					BoxTest433.State.selfHeartBeat = 1;
					BoxTest433.State.State = B433IntialOK;
					break;
				
				case B433IntialOK:
					BuiltVar(pBoxIO,pBoxMsg,B433Test);
					if(BoxTest433.Command.IntialConfirm==1 )
					{
						BoxTest433.Command.IntialRequest = 0;
						BoxTest433.Command.IntialConfirm = 0;
						BoxTest433.State.State = B433BroadCast;
					}
					else
					{
						BuiltStr(&BoxIO.Uart2Dat,&AGVBox,B433InitReq);//send request
					}
					break;
				case B433BroadCast:
					BuiltStr(&BoxIO.Uart2Dat,&AGVBox,SlaveB433HeartBeat);
					break;
				case B433FeedBackPC:
					break;
			}
			
			break;
	}
	
};
void phoneApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{
	static uint32_t bntTimeStamp,bntResetTimeStamp,tempTimeInterval,TestDelay;
	uint32_t bntDectDelay=300,bntResetDelay=3000,bntHigPrioDelay=300000;
	
	BoxIO.DO[1]=BoxIO.DO[2]=BoxIO.DO[3]=
	BoxIO.DO[4] = !BoxIO.DI[3];
	
	if(pBoxMsg->Monitor.Button[BntCalAGV])
	{
		tempTimeInterval = pBoxIO->timeStamp - bntTimeStamp;
		if(tempTimeInterval>bntResetDelay)
		{
			BoxPhone.Command.OrderCancel = 1;			
			tempTimeInterval = 0;
		}
	}
	else
	{
		if(tempTimeInterval>bntDectDelay && tempTimeInterval<bntResetDelay)
		{
			BoxPhone.Command.Call = 1;	
			tempTimeInterval = 0;
		}
		else if(tempTimeInterval>bntResetDelay)
		{
			BoxPhone.Command.OrderCancel = 1;			
			tempTimeInterval = 0;
		}
		else
		{
			tempTimeInterval = 0;
			bntTimeStamp = pBoxIO->timeStamp;
		}
	}
	
	if(BoxPhone.Command.Call)
	{
		BoxPhone.State.PhoneState=phInCall;
	};			
	switch(BoxPhone.State.PhoneState)
	{
		case phStanby:
			if((TestDelay+bntHigPrioDelay)<pBoxIO->timeStamp)
			{
				TestDelay	= pBoxIO->timeStamp;
				BoxPhone.Command.Call = TRUE;
			}		
			BuiltVar(pBoxIO,pBoxMsg,PhCalStr);			
			if(BoxPhone.Command.Finish)
			{
				BuiltStr(&pBoxIO->Uart1Dat,pBoxMsg,CnfirmFinish);
			}
			BoxPhone.Command.Finish 			= FALSE;
			BoxPhone.Command.OrderCancel  = FALSE;
			BoxPhone.Command.OrderRec  		= FALSE;			
			pBoxMsg->Monitor.Led[LedBoxState] = TRUE;//点着LED灯
			break;
			
		case phInCall:
			BuiltStr(&pBoxIO->Uart1Dat,pBoxMsg,PhCalStr);
			BuiltVar(pBoxIO,pBoxMsg,PhCalStr);
			if(BoxPhone.Command.OrderCancel)
			{
				BoxPhone.State.PhoneState 		= phFinish;
				BoxPhone.Command.Call		  		= FALSE;
				BoxPhone.Command.OrderCancel  = FALSE;
			}
			if(BoxPhone.Command.OrderRec)
			{
				BoxPhone.State.PhoneState 		= phInOrder;
				BoxPhone.Command.Call		  		= FALSE;
				BoxPhone.Command.OrderRec     = FALSE;
			}
			pBoxMsg->Monitor.Led[LedBoxState] = pBoxIO->flagLed[sLedFlash];
			break;
			
		case phInOrder:
			BuiltVar(pBoxIO,pBoxMsg,PhCalStr);
			if(BoxPhone.Command.OrderCancel || BoxPhone.Command.Finish)
			{
				BoxPhone.State.PhoneState 		= phFinish;
				BoxPhone.Command.Finish		  	= FALSE;
				BoxPhone.Command.OrderCancel  = FALSE;
				BoxPhone.Command.OrderRec     = FALSE;
				BuiltStr(&pBoxIO->Uart1Dat,pBoxMsg,CnfirmFinish);
			}
			pBoxMsg->Monitor.Led[LedBoxState] = pBoxIO->flagLed[fLedFlash];
			break;
		
		case phFinish:
			BoxPhone.State.PhoneState 		  = phStanby;
			TestDelay												= pBoxIO->timeStamp;
			break;
		
		case phInError:
			BoxPhone.State.PhoneState 		  = phStanby;
			break;
		
		default:
			break;
	
	};
};
void AMSApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{
	
};
void AGVApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{
	
};
void DoorApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg)
{
	static uint32_t reTimeStamp;
	uint32_t clsDelay = 10000;
	pBoxMsg->Monitor.DoorLmt[DownLimit] = BoxDoor.Monitor.Status.closed;
	//BoxDoor.Monitor.Status.closed = pBoxMsg->Monitor.DoorLmt[DownLimit];
	BoxDoor.Monitor.Status.Opened = pBoxMsg->Monitor.DoorLmt[UpLimit];
	
	switch(BoxDoor.Monitor.State)
	{	
		
		case DrStanby:
			BuiltVar(pBoxIO,pBoxMsg,DoorCmd);
			if(BoxDoor.Command.openDoor == 1)
			{
				BoxDoor.Monitor.State 						= DrOpening;
				pBoxMsg->Monitor.DoorCmd[cmdOpen] = 1;
				BoxDoor.Monitor.Status.closed 		= 0;
				BoxDoor.Command.openDoor          = FALSE;
			}
			if(BoxDoor.Command.closeDoor == 1)
			{
				BoxDoor.Monitor.State = DrClosing;
				reTimeStamp = pBoxIO->timeStamp;
				pBoxMsg->Monitor.DoorCmd[cmdOpen] = 0;
				BoxDoor.Command.closeDoor					= FALSE;
			}
			break;
		
		case DrOpening:
			if(BoxDoor.Monitor.Status.Opened == 1)
			{
				BoxDoor.Monitor.State = DrOpened;
			}
			BuiltStr(&pBoxIO->Uart1Dat,pBoxMsg,DoorMsg);
			pBoxMsg->Monitor.Led[LedBoxState] = pBoxIO->flagLed[fLedFlash];
			break;
		
		case DrOpened:
			BoxDoor.Monitor.State     = DrStanby;
			BoxDoor.Command.openDoor  = FALSE;
			pBoxMsg->Monitor.Led[LedBoxState] = pBoxIO->flagLed[fLedFlash];
			break;
				
		case DrClosing:
				
			if(pBoxIO->timeStamp-reTimeStamp>clsDelay)
			{
				BoxDoor.Monitor.Status.closed = 1;
			}
			if(BoxDoor.Monitor.Status.closed == 1)
			{
				BoxDoor.Monitor.State = DrClosed;
			}
			BuiltVar(pBoxIO,pBoxMsg,DoorCmd);
			if(BoxDoor.Command.closeDoor)
			{
				BoxDoor.Command.closeDoor = FALSE;
				BuiltStr(&pBoxIO->Uart1Dat,pBoxMsg,DoorMsg);
			}
			break;
		
		case DrClosed:
			BoxDoor.Monitor.State     = DrStanby;
			BoxDoor.Command.closeDoor	= FALSE;
			
			break;
		
		case DrInError:
			break;
						
		default:
			break;
	}
};

void _BoxTaskInit_()
{
	SystemConfig();						//配置系统
	
	////////////////////////////////
	while(BoxIO.timeStamp < 1000)
	{
		SetDO(DoBuzzer,1);
		LinkIO(&BoxIO,&AGVBox);	//
		PVMap(&BoxIO);				 	//IO映射层,输入输出,	
	}		
	NodenDis[0][0] 	= gSystemPara.CanNode;
	NodenDis[1][0] 	= gSystemPara.RS485Node;
	SetDO(DoBuzzer,0);
};
void _BoxTaskCyc_()
{
	while(TRUE)
	{
		LinkIO(&BoxIO,&AGVBox);//
		PVMap(&BoxIO);				 //IO映射层,输入输出,
		RecDistance(pNowNode);
		pNowNode++;
		if(pNowNode>=NodeSum)pNowNode=0;
		DataFlow();
	}
};
void DataFlow(void){
	//////////////////////////////////////////////////
	////////COM Interface capture master request//////
	//////////////////////////////////////////////////
	

	if(2==gSystemPara.DataComInterface){//RS232
		if(ComRequestCap(&BoxIO.Uart2Dat))
			Readers.cmdSendRS232Data = 1;
	}
	if(HandShakeCap(&BoxIO.Uart2Dat)){//RS232
		Readers.cmdPCConnect = 1;
		Readers.ReqConnectAllow = 1;
	}
	if(PCChangeParCap(&BoxIO.Uart2Dat)){//RS232
		Readers.cmdSaveParameter = 1;
	}
	//////////////////////////////////////////////////
	//////////////////////////////////////////////////
	///////////////Parameter save or initial//////////
	
	////////FOR TEST////////
	//gSystemPara.DataComInterface = 2;
	//gSystemPara.RequestInterval = 4;
	////////////////////////
	
	if(Readers.cmdSaveParameter)
	{
		Readers.cmdSaveParameter = 0;
		Readers.ReqSetOKAllow = 1;
		memcpy(&Parameter1,&gSystemPara,sizeof(gSystemPara));
		Parameter1[199] = 0xCD;
		STMFLASH_Write(FLASH_SAVE_ADDR,( uint16_t * )Parameter1,200);
	}
	if(Readers.cmdIntialParameter)
	{
		Readers.cmdIntialParameter = 0;
		////////////////////////
		gSystemPara.BufferHead = 0xAB;
		gSystemPara.CANBusBauderate = 1;//250k
		gSystemPara.CanNode = 0x100;
		gSystemPara.DataAnsMethod = 0;
		gSystemPara.DataComInterface = 0;
		gSystemPara.DetectPolar = 0;
		gSystemPara.MagSensity = 1;
		gSystemPara.RequestInterval = 2;
		gSystemPara.RS232Bauderate = 0;
		gSystemPara.RS485Bauderate = 0;
		gSystemPara.RS485Node = 10;
		gSystemPara.MagTapWide = 0;
		///default parameters
		memcpy(&Parameter1,&gSystemPara,sizeof(gSystemPara));
		Parameter1[199] = 0xCD;
		STMFLASH_Write(FLASH_SAVE_ADDR,( uint16_t * )Parameter1,200);
	}	
	//////////////////////////////////////////////////
}

bool PCChangeIDCap(UrtBuf_type * pSrcBuf){
	uint8_t filled = pSrcBuf->pRfil;
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
			if( pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x51&&
					pSrcBuf->rBuffer[degred+2]==0x0B&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+13]==0xAC)
				{
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					result = 1;
				}
		}
	}
	return result;
}
bool PCChangeParCap(UrtBuf_type * pSrcBuf){
	uint8_t filled = pSrcBuf->pRfil,iDat;
	uint8_t degred = pSrcBuf->pRder;
	uint8_t iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	uint8_t chnOKBfLen = 116;
	uint16_t cycResult;
	bool result=0;
	///
	result = 0;
	///
	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if( pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x54&&
					pSrcBuf->rBuffer[degred+2]==0x11&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+115]==0xAC)
				{
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					gSystemPara.DetectPolar = pSrcBuf->rBuffer[degred+6];
					gSystemPara.DataComInterface = pSrcBuf->rBuffer[degred+7];
					gSystemPara.RS485Node = pSrcBuf->rBuffer[degred+8];
					gSystemPara.CanNode = pSrcBuf->rBuffer[degred+9]*256 + pSrcBuf->rBuffer[degred+10];
					gSystemPara.RS485Bauderate = pSrcBuf->rBuffer[degred+11];
					gSystemPara.RS232Bauderate = pSrcBuf->rBuffer[degred+12];
					gSystemPara.CANBusBauderate = pSrcBuf->rBuffer[degred+13];
					gSystemPara.DataAnsMethod = pSrcBuf->rBuffer[degred+14];
					gSystemPara.RequestInterval = pSrcBuf->rBuffer[degred+15];
					gSystemPara.MagSensity = pSrcBuf->rBuffer[degred+16];
					gSystemPara.MagTapWide = pSrcBuf->rBuffer[degred+17];
					gSystemPara.MountDir = pSrcBuf->rBuffer[degred+18];
					/////////////////////////////////////////
					for(iDat=0;iDat<8;iDat++)
					{
						gSystemPara.Area[iDat].ComDisACHA = pSrcBuf->rBuffer[degred+ 19 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+20 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisBCHA = pSrcBuf->rBuffer[degred+ 21 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+22 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisCCHA = pSrcBuf->rBuffer[degred+ 23 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+24 + 0 + iDat*12];
						
						gSystemPara.Area[iDat].ComDisACHB = pSrcBuf->rBuffer[degred+ 25 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+26 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisBCHB = pSrcBuf->rBuffer[degred+ 27 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+28 + 0 + iDat*12];
						gSystemPara.Area[iDat].ComDisCCHB = pSrcBuf->rBuffer[degred+ 29 + 0 + iDat*12]*256 + pSrcBuf->rBuffer[degred+30 + 0 + iDat*12];
					}
					
					/////////////////////////////////////////
					result = 1;
				}
		}
	}
	return result;
}
bool HandShakeCap(UrtBuf_type * pSrcBuf)
{
	uint8_t filled = pSrcBuf->pRfil;
	uint8_t degred = pSrcBuf->pRder;
	uint8_t iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	uint8_t chnOKBfLen = 7;
	uint16_t cycResult;
	bool result=0;
	///
	result = 0;
	///
	if(filled >= degred+ chnOKBfLen)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(pSrcBuf->rBuffer[degred]==0xAA && 
					pSrcBuf->rBuffer[degred+1]==0x55&&
					pSrcBuf->rBuffer[degred+2]==0x04&&
					pSrcBuf->rBuffer[degred+3]==0x00&&
					pSrcBuf->rBuffer[degred+4]==0x00&&
					pSrcBuf->rBuffer[degred+5]==0x01&&
					pSrcBuf->rBuffer[degred+6]==0xAB)
				{
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					result = 1;
				}
		}
	}
	return result;
}
bool ComRequestCap(UrtBuf_type * pSrcBuf)
{
	uint8_t filled = pSrcBuf->pRfil;
	uint8_t degred = pSrcBuf->pRder;
	uint8_t iFunCode=5,iLenCode=4,iDstSta=3,iSrcSta=2;
	uint8_t chnOKBfLen = 7;
	uint16_t cycResult;
	bool result=0;
	///
	result = 0;
	///
	if(filled >= chnOKBfLen-1)//1. Len enough.
	{
		for(;degred<=filled-1;degred++)
		{
			if(pSrcBuf->rBuffer[degred]==0x52 && 
					pSrcBuf->rBuffer[degred+1]==0x4D &&
					pSrcBuf->rBuffer[degred+2]==0x67 &&
					pSrcBuf->rBuffer[degred+3]==0x73 &&
					pSrcBuf->rBuffer[degred+4]==0x77 &&
					pSrcBuf->rBuffer[degred+5]==0x5E &&
					pSrcBuf->rBuffer[degred+6]==0x69)
			{
				if(filled >= degred+ chnOKBfLen){
					cycResult = CRC_Verify(&(pSrcBuf->rBuffer[degred]),5);//
					pSrcBuf->pRder = degred+chnOKBfLen-1;
					result = 1;
				}
			}
		}
	}
	return result;
};