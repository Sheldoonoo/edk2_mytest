#ifndef _SMBIOS_TEST_
#define _SMBIOS_TEST_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h> //include *gBS

#include <IndustryStandard/SmBios.h>
#include <Protocol/Smbios.h>

#include <Library/SmbiosLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Library/ShellLib.h>
#include <Library/ShellCommandLib.h>

typedef struct
{
    SMBIOS_STRUCTURE Hdr;
    SMBIOS_TABLE_STRING LastName;
    SMBIOS_TABLE_STRING FirstName;
    UINT8 Age;
    UINT8 Id;
    SMBIOS_TABLE_STRING Summary;
} SMBIOS_TABLE_TYPE188;

#endif