#ifndef __BOX_APP_H__
#define __BOX_APP_H__
	#include "BoxFun.h"
	#include "IOMap.h"
	///////
	void _BoxTaskInit_(void);
	void _BoxTaskCyc_(void);
	///////
	void NullApp(void);
	void BMSApp(void);
	void CallerApp(void);
	void ChargeStaionApp(void);
	void SonarApp(void);
	///////////////////////////////////////

	bool HandShakeCap(UrtBuf_type * pSrcBuf);
	bool PCChangeParCap(UrtBuf_type * pSrcBuf);
	bool ChangeFilterCap(UrtBuf_type * pSrcBuf);
	bool PCChangeIDCap(UrtBuf_type * pSrcBuf);
	bool PCCalSensor(UrtBuf_type * pSrcBuf);

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
		bool cmdSendRS232Data;
		bool cmdPCConnect;
		bool ReqConnectAllow;
		bool ReqSetOKAllow;
		////
		int8_t StatusStep;
		uint8_t SetID;
		uint8_t FltPar;
		uint8_t ActID;
	}SonarDualData_type;

	////////////////////////////////	
	typedef struct SysParameter_type
	{
		uint8_t	 BufferHead;
		uint8_t  RS485Node;
		uint16_t CanNode;
		uint8_t  RS485Bauderate;//0-9600 1-19200 2-38400 3-115200
		uint8_t	 RS232Bauderate;//0-9600 1-19200 2-38400 3-115200
		uint8_t	 TTLBauderate;//0-9600 1-19200 2-38400 3-115200
		uint8_t	 CANBusBauderate;//0-125k 1-250k 2-500k 3-1000k 
		uint8_t  AppMode;
		uint8_t  AOMode;// 000:Disable 001:0~5V 002:0~10V 003:+-5V 004:+-10V 005:NULL 006:4~20mA 007:0~20mA 008:0~24mA
		bool cmdSaveParameter;
		uint8_t  DataComInterface;
		////////
		SonarComVal_type Area[20];
		uint8_t  RangeMode[4];
		uint8_t  SonarNodeCHNA;
		uint8_t  SonarNodeCHNB;
		
		/////BoardType 0 :Null  1: A-Type
		uint8_t BoardType;

	}SysParameter_type;
	////////////////
	////////////////////////////////
	extern SysParameter_type gSystemPara;
	extern SonarDualData_type SonarDual;
	#define sLedFlash 0
	#define fLedFlash 1
	#define DBLedFlash 2
#endif