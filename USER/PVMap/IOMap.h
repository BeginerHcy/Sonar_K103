#include "HwCfg.h"

#define	InputNum 16
#define OutputNum 16
#define UrtBfLen  200

#define time2ms   2
#define time4ms   4
#define time10ms  10
#define time100ms 100
#define time200ms 200
#define time500ms 500
#define timeLong  1500

#define sLedFlash 0
#define fLedFlash 1
#define DBLedFlash 2

typedef enum{PhoneMode=0,AMSMode,AGVMode,DoorMode,CfgMode,ImuMode,ComTestMode}BoxMode;
typedef enum{phStanby=0,phInCall,phInOrder,phFinish,phInError}PhoneState;
typedef enum{B433Stanby=0,B433Intial,B433IntialOK,B433BroadCast,B433FeedBackPC}Box433State;
typedef enum{MasterNode=0,SlaveNode}Box433Node;
typedef enum{DrStanby=0,DrOpening,DrOpened,DrClosing,DrClosed,DrInError}DoorState;
////////////////////////////////////
typedef struct Box_Monotor_type
{
	u8 NodeNum;
	u8 Channel;
	u8 UrtBuf[UrtBfLen];
	u8 Button[4];
	u8 Led[4];
	u8 DoorLmt[4];
	u8 DoorCmd[4];
}Box_Monotor_type;

typedef struct Box_Cfg_type
{
	BoxMode Mode;
	bool CfgOk;
}Box_Cfg_type;

typedef struct AGVBox_type
{
	Box_Monotor_type Monitor;
	Box_Cfg_type     Config;
}AGVBox_type;
/////////////////////////////////////


typedef struct UrtBuf_type
{
	u8	rBuffer[UrtBfLen];
	u8  pRfil;
	u8  pRder;
	
	u8	sBuffer[UrtBfLen];
	u8  sLen;
	u8  flagS;
}UrtBuf_type;

typedef	struct AGVIO_type
{
		
	bool	DI[InputNum];
	bool	DO[OutputNum];
	
	UrtBuf_type	Uart1Dat;
	UrtBuf_type	Uart2Dat;
	uint32_t timeStamp;
	bool flagLed[4];
}AGVIO_type;
///////////////////////////////////////
typedef struct BoxPhone_state_type
{
	PhoneState PhoneState;///记录当前状态
}BoxPhone_state_type;

typedef struct BoxPhone_cmd_type
{
	bool Call;
	bool OrderCancel;
	bool Finish;
	bool OrderRec;
}BoxPhone_cmd_type;
typedef	struct BoxPhone_type
{
	BoxPhone_cmd_type Command;
	BoxPhone_state_type State;
}BoxPhone_type;
///////////////////////////////////////Test Type
typedef struct BoxTest433_state_type
{
	Box433Node Node;///记录当前状态
	Box433State State;
	u8 RequestNum;
	u32 selfHeartBeat;//Com HeartBeat
	u32 HeartBeatSlave[20];//the heartBeat of the slaver
	u8 ActHeartBeatSlaveM1[20];//the heartBeat of the slaver
	u8 ActHeartBeatSlaveM2[20];//the heartBeat of the slaver
	u8 ActHeartBeatSlaveM3[20];//the heartBeat of the slaver
	u8 ActHeartBeatSlaveM4[20];//the heartBeat of the slaver
}BoxTest433_state_type;

typedef struct BoxTest433_cmd_type
{
	bool IntialRequest;
	bool IntialConfirm;
	bool Finish;
	bool OrderRec;
}BoxTest433_cmd_type;
typedef	struct BoxTest433_type
{
	BoxTest433_cmd_type Command;
	BoxTest433_state_type State;
}BoxTest433_type;
///////////////////////////////////////

typedef	struct BoxDoor_cmd_type
{
	bool openDoor;
	bool closeDoor;
	bool stopDoor;
}BoxDoor_cmd_type;
typedef	struct BoxDoor_status_type
{
	u8   ledState;
	bool Opened;
	bool closed;
	bool stop;
	bool Error;
}BoxDoor_status_type;
typedef	struct BoxDoor_Monitor_type
{
	BoxDoor_status_type Status;
	DoorState State;
}BoxDoor_Monitor_type;
typedef	struct BoxDoor_type
{
	BoxDoor_cmd_type Command;
	BoxDoor_Monitor_type Monitor;
}BoxDoor_type;


///////////////////////////////////////
typedef struct WorkSpaceCfg_type
{
	u8 AMSStation;
}WorkSpaceCfg_type;
#ifndef IMU_RAWData_type
	typedef struct IMU_RAWData_type
	{
		int16_t FLASH_CNT;
		double XGYRO_OUT;
		double YGYRO_OUT;
		double ZGYRO_OUT;
		double XACCL_OUT;
		double YACCL_OUT;
		double ZACCL_OUT;
		double TEMP_OUT;
		double XGYRO_OFF;
		double YGYRO_OFF;
		double ZGYRO_OFF;
		double XACCL_OFF;
		double YACCL_OFF;
		double ZACCL_OFF;
		int16_t GPIO_CTRL;
		int16_t MSC_CTRL;
		int16_t SMPL_PRD;
		int16_t SENS_AVG;
		int16_t DIAG_STAT;
		int16_t ALM_MAG1;
		int16_t ALM_MAG2;
		int16_t ALM_SMPL1;
		int16_t ALM_SMPL2;
		int16_t ALM_CTRL;
		int16_t LOT_ID1;
		int16_t LOT_ID2;
		int16_t PROD_ID;
		int16_t SERIAL_NUM;
	}IMU_RAWData_type;

	typedef struct IMU_type
	{
		IMU_RAWData_type RawData;
	}IMU_type;
#endif
	typedef struct
{
	int16_t SensorValue[15];
	int16_t SenState;//bit0:most left sensor ; bit14:most Right sensor
	int16_t compValue;//to desire the bet state of the 'senState Above'
	int16_t MagTapeWidth;//the width of the magTape,mostly is 30 50 or 5x5mm,also you can set out by youselft,or detect by itseft(not design yet)
	int8_t Sensity;//0-low sensity  1-high sensity
	bool cmdSendCanData;
	bool cmdSendRS232Data;
	bool cmdSendRS485Data;
	bool cmdSaveParameter;
	bool cmdIntialParameter;
	bool cmdPCConnect;
	bool ReqConnectAllow;
	bool ReqSetOKAllow;
	uint8_t BlockData[8];
	int8_t WriteBlockData[8];
	int16_t Xsection2;
	int16_t Xsection3;
	int16_t TempCount[5];
	int16_t ResTimeStamp[5];
	int16_t StatusID;
	int16_t StatusRBID;
	int16_t StatusTag;
	bool cmdReadBlocks;//???
	bool cmdWriteBlocks;//???
	bool cmdAnticollision;//???
	bool cmdReadUid;//???
	bool cmdTrigSend;
	bool flagDataReceived;//????????
	bool flagDataError;
	bool flagNoTagDetect;
	bool flagTagAlive;
	int8_t BufDataRev[40];
	
} RFID_Type; 
////////////////////////////////
typedef struct SonarComVal_type
{	
	int16_t ComDisACHA;
	int16_t ComDisBCHA;
	int16_t ComDisCCHA;	
	
	int16_t ComDisACHB;
	int16_t ComDisBCHB;
	int16_t ComDisCCHB;	
}SonarComVal_type;
typedef struct SonarComInterval_type
{	
	int16_t pAreaToCom;
	bool AreaDetectSwitch;
	int16_t OutComState;//0: No Error    1: A     2: B    3: C
}SonarComInterval_type;
////////////////////////////////
typedef struct SonarDualData_type
{
	////
	SonarComVal_type Area[20];
	////
	int16_t DistanCHA;
	int16_t DistanCHB;
	////
	bool bDI1;
	bool bDI2;
	bool bDI3;
	bool bDI4;
	////
	bool bDO1;
	bool bDO2;
	bool bDO3;
	bool bDO4;
	////
	SonarComInterval_type Internal;
	////
	bool AliveCHA;
	bool AliveCHB;
	////
	int8_t StatusStep;
}SonarDualData_type;


typedef struct SysParameter_type
{
	uint8_t	 BufferHead;
	uint8_t  DetectPolar;//0-N  1-S
	uint8_t  DataComInterface;//0-CAN,1-RS485,2-RS232
	uint8_t  RS485Node;//
	uint16_t CanNode;//
	uint8_t  RS485Bauderate;//0-9600 1-19200 2-38400 3-115200
	uint8_t	 RS232Bauderate;//0-9600 1-19200 2-38400 3-115200
	uint8_t	 CANBusBauderate;//0-125k 1-250k 2-500k 3-1000k 
	uint8_t  DataAnsMethod;//0-automate 1-passive 2-ByTrig
	uint8_t  RequestInterval;//0-4ms 1-10ms  2-20ms 3-40ms 4-100ms
	uint8_t  MagSensity;//0-80 1-150 2-200 3-250 4-300;
	uint8_t  MagTapWide;//0-30 1- 50;
	uint8_t  MountDir;//0-default 1- back;
	SonarComVal_type Area[20];
}SysParameter_type;
////////////////
////////////////////////////////
extern IMU_type ADs16445;
extern SysParameter_type gSystemPara;
///////////////////////////////////////
extern AGVIO_type BoxIO;
extern AGVBox_type AGVBox;
extern BoxPhone_type BoxPhone;
extern BoxDoor_type BoxDoor;
extern WorkSpaceCfg_type WkSpcCfg;
extern SonarDualData_type SonarDual;
extern RFID_Type Readers;
///////////////////////////////////////
void   PVMap(AGVIO_type * pBoxIO);
void   Uart_Send_Buffer(uint8_t *buffer,uint8_t bufLen);
void   FillUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx);
void 	 MBLArry(u8 *buffer,uint8_t bufLen);
typedef void(*data_parse_fun)(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);//定义回调函数格式

typedef void(*cyc_task_fun)();////
void   PhoneCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);//
void   AMSCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void   AGVCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void   DoorCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void   xChnCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void   IMUCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void   ComTestCap(UrtBuf_type * pSrcBuf,AGVBox_type * pBoxMsg);
void 	 SendUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx);
void 	cyc2ms(void);
void 	cyc4ms(void);
void 	cyc10ms(void);
void 	cyc100ms(void);
void 	cyc200ms(void);
void 	cyc500ms(void);
void cycLongTsk(void);



