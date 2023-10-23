#ifndef _MY_PROTOCOL_DXE_H_
#define _MY_PROTOCOL_DXE_H_


#include <Library/UefiBootServicesTableLib.h>   //for gBS
#include <Library/UefiLib.h>    //for "Print" Function
#include "../../Protocol/MyProtocol.h"

#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <PiDxe.h>

#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HobLib.h>


MY_PROTOCOL MyProtocol;

EFI_STATUS
EFIAPI
MyProtocolFunction(IN MY_PROTOCOL *This,
                   IN UINT32 Number);



#endif