#ifndef __IO_MAP_H__
#define __IO_MAP_H__
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
	////////////////////////////////////
	typedef struct UrtBuf_type
	{
		u8	rBuffer[UrtBfLen];
		u8  pRfil;
		u8  pRder;
		
		u8	sBuffer[UrtBfLen];
		u8  sLen;
		u8  flagS;
	}UrtBuf_type;
	////////////////////////////////
	typedef	struct AGVIO_type
	{
			
		bool	DI[InputNum];
		bool	DO[OutputNum];
		bool 	LED[5];
		UrtBuf_type	Uart1Dat;
		UrtBuf_type	Uart2Dat;
		UrtBuf_type	Uart3Dat;
		uint32_t TimeStamp;
		bool flagLed[4];
	}AGVIO_type;
	
	///////////////////////////////////////
	extern AGVIO_type BoxIO;
	///////////////////////////////////////
	void   	PVMap(AGVIO_type * pBoxIO);
	void   	Uart_Send_Buffer(uint8_t *buffer,uint8_t bufLen);
	void   	FillUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx);
	void 	MBLArry(u8 *buffer,uint8_t bufLen);
	////////////////////////////////
	typedef void(*cyc_task_fun)();////
	void 	SendUrtBuf(UrtBuf_type * UartDat,uint32_t USARTx);
	void 	cyc2ms(void);
	void 	cyc4ms(void);
	void 	cyc10ms(void);
	void 	cyc100ms(void);
	void 	cyc200ms(void);
	void 	cyc500ms(void);
	void  	cycLongTsk(void);

#endif

