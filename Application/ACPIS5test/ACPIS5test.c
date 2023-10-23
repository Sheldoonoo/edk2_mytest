#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h> //include *gBS
#include <IndustryStandard/Acpi.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>

#include <Library/UefiLib/UefiLibInternal.h>
#include <Library/IoLib.h>
#include <IndustryStandard/Acpi.h>
#include <Guid/Acpi.h>

/**
  This function scans ACPI table in XSDT/RSDT.

  @param Sdt                    ACPI XSDT/RSDT.
  @param TablePointerSize       Size of table pointer: 8(XSDT) or 4(RSDT).
  @param Signature              ACPI table signature.
  @param PreviousTable          Pointer to previous returned table to locate
                                next table, or NULL to locate first table.
  @param PreviousTableLocated   Pointer to the indicator about whether the
                                previous returned table could be located, or
                                NULL if PreviousTable is NULL.

  If PreviousTable is NULL and PreviousTableLocated is not NULL, then ASSERT().
  If PreviousTable is not NULL and PreviousTableLocated is NULL, then ASSERT().

  @return ACPI table or NULL if not found.

**/
EFI_ACPI_COMMON_HEADER *
ScanTableInSDT_cpy (
  IN  EFI_ACPI_DESCRIPTION_HEADER  *Sdt,
  IN  UINTN                        TablePointerSize,
  IN  UINT32                       Signature,
  IN  EFI_ACPI_COMMON_HEADER       *PreviousTable  OPTIONAL,
  OUT BOOLEAN                      *PreviousTableLocated OPTIONAL
  )
{
  UINTN                   Index;
  UINTN                   EntryCount;
  UINT64                  EntryPtr;
  UINTN                   BasePtr;
  EFI_ACPI_COMMON_HEADER  *Table;

  if (PreviousTableLocated != NULL) {
    ASSERT (PreviousTable != NULL);
    *PreviousTableLocated = FALSE;
  } else {
    ASSERT (PreviousTable == NULL);
  }

  if (Sdt == NULL) {
    return NULL;
  }

  EntryCount = (Sdt->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / TablePointerSize;

  BasePtr = (UINTN)(Sdt + 1);
  for (Index = 0; Index < EntryCount; Index++) {
    EntryPtr = 0;
    CopyMem (&EntryPtr, (VOID *)(BasePtr + Index * TablePointerSize), TablePointerSize);
    Table = (EFI_ACPI_COMMON_HEADER *)((UINTN)(EntryPtr));
    if ((Table != NULL) && (Table->Signature == Signature)) {
      if (PreviousTable != NULL) {
        if (Table == PreviousTable) {
          *PreviousTableLocated = TRUE;
        } else if (*PreviousTableLocated) {
          //
          // Return next table.
          //
          return Table;
        }
      } else {
        //
        // Return first table.
        //
        return Table;
      }
    }
  }

  return NULL;
}

EFI_STATUS EFIAPI UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{

    UINTN Index;
    UINT32 Pm1aCnt;
    UINTN regaddress;
    EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER *Rsdp;
    EFI_ACPI_DESCRIPTION_HEADER *Xsdt;
    EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *Fadt;

    for (Index = 0; Index < gST->NumberOfTableEntries; Index++)
    {
        if (CompareGuid(&gEfiAcpi20TableGuid, &(gST->ConfigurationTable[Index].VendorGuid)))
        {
            Rsdp = (EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)gST->ConfigurationTable[Index].VendorTable;
            break;
        }
    }

    // 判断是否为RSDP结构
    if (Rsdp->Signature == EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE &&
        Rsdp->XsdtAddress != 0)
    {

        Xsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->XsdtAddress;

        // 判断是否为XSDT结构
        if (Xsdt->Signature == EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE)
        {
            // locate FADT
            Fadt = (EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE *)ScanTableInSDT_cpy(
                Xsdt,
                sizeof(UINT64),
                EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
                NULL,
                NULL);
            Print(L"Fadt->Pm1aCntBlk address = 0x%x. \n", Fadt->Pm1aCntBlk);

            // 检查是否存在有效的系统控制地址
            if ((Fadt->Flags & EFI_ACPI_6_5_HW_REDUCED_ACPI) == 0 && Fadt->Pm1aCntBlk != 0)
            {
                // Pm1aCntBlk or XPm1aCntBlk
                regaddress = Fadt->Pm1aCntBlk;
                if(Fadt->XPm1aCntBlk.Address != 0) // XPm1aCntBlk none zero, ignore Pm1aCntBlk
                {
                    regaddress = Fadt->XPm1aCntBlk.Address;
                }
               
                // 关机 
                Pm1aCnt = IoRead16(regaddress);
                Print(L"Fadt->Pm1aCntBlk value = 0x%x\n", Pm1aCnt);
                
                Pm1aCnt |= 0x3c00; 
                IoWrite16(regaddress, Pm1aCnt);
                
                Pm1aCnt = IoRead16(regaddress);
                Print(L"Fadt->Pm1aCntBlk value = 0x%x\n", Pm1aCnt);
                
                Print(L"System shutdown successful.\n");
                return EFI_SUCCESS;
            }
            else
            {
                // 如果没有找到FADT，则返回失败
                Print(L"System shutdown failed! FADT not found.\n");
                return EFI_NOT_FOUND;
            }
        }
        
        // 如果没有找到XSDT，则返回失败
        Print(L"System shutdown failed! XSDT not found.\n");
        return EFI_NOT_FOUND;
    }

    // 如果没有找到RSDP，则返回失败
    Print(L"System shutdown failed! RSDP not found.\n");
    return EFI_NOT_FOUND;
}