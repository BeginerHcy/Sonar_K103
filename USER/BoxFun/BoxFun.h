#include "DataGet.h"
#include "IOMap.h"

#define BntCalAGV 0
#define BntReleasAGV 1

#define LedBoxState 0

#define UpLimit 0
#define DownLimit 1

#define cmdOpen 0
#define cmdClose 1

#define DIBntCalAGV 9
#define DIReleasAGV 8

#define DIDoorUpLmt 9
#define DIDoorDwnLmt 8

#define DOLedState 0

#define DOOpenDoor 0
#define DOCloseDoor 1


u16 CRC_Verify(u8 *CRC_Buf,u8 Buf_Length);