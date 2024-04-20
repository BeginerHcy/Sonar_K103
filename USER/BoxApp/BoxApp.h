#include "BoxFun.h"
///////
#define phCalCde 0x10
#define phRunCde 0x12
#define CnfrmCde 0x14
#define OrderCde 0x16
#define AgvCtrCde 0x18
#define doorCtlCde 0x1A
#define doorMsgCde 0x1C
#define AgvMsgCde 0x1E
#define B433IntiCde 0xFF
#define B433HeartCde 0x0A

///////
typedef enum {PhCalStr,PhRun,CnfirmFinish,Order,AGVcmd,DoorCmd,DoorMsg,AGVMsg,B433InitReq,B433Test,MasterConfirm433,SlaveB433HeartBeat,B433FbkPC}str2Com;
///////
void _BoxTaskInit_(void);
void _BoxTaskCyc_(void);
///////
typedef void(*agv_mode_App)(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void phoneApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void ComTest(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void AMSApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void AGVApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void DoorApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void CfgApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
void IMUApp(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg);
///////
u8 BuiltStr(UrtBuf_type * UartDat,AGVBox_type * pBoxMsg,str2Com comType);//生成数据包并发送
u8 BuiltVar(AGVIO_type * pBoxIO,AGVBox_type * pBoxMsg,str2Com comType);//获取数据包并发送
///////
void DataFlow(void);
bool ComRequestCap(UrtBuf_type * pSrcBuf);
bool HandShakeCap(UrtBuf_type * pSrcBuf);
bool PCChangeParCap(UrtBuf_type * pSrcBuf);
bool PCChangeIDCap(UrtBuf_type * pSrcBuf);