#include "PCIProtocoltest.h"


// From ShellPkg/Library/UefiShellDebug1CommandsLib/Pci.c
/**
  Generates printable Unicode strings that represent PCI device class,
  subclass and programmed I/F based on a value passed to the function.

  @param[in] ClassCode      Value representing the PCI "Class Code" register read from a
                 PCI device. The encodings are:
                     bits 23:16 - Base Class Code
                     bits 15:8  - Sub-Class Code
                     bits  7:0  - Programming Interface
  @param[in, out] ClassStrings   Pointer of PCI_CLASS_STRINGS structure, which contains
                 printable class strings corresponding to ClassCode. The
                 caller must not modify the strings that are pointed by
                 the fields in ClassStrings.
**/
VOID PciGetClassStrings(
    IN UINT32 ClassCode,
    IN OUT PCI_CLASS_STRINGS *ClassStrings)
{
    INTN Index;
    UINT8 Code;
    PCI_CLASS_ENTRY *CurrentClass;

    //
    // Assume no strings found
    //
    ClassStrings->BaseClass = L"UNDEFINED";
    ClassStrings->SubClass = L"UNDEFINED";
    ClassStrings->PIFClass = L"UNDEFINED";

    CurrentClass = gClassStringList;
    Code = (UINT8)(ClassCode >> 16);
    Index = 0;

    //
    // Go through all entries of the base class, until the entry with a matching
    // base class code is found. If reaches an entry with a null description
    // text, the last entry is met, which means no text for the base class was
    // found, so no more action is needed.
    //
    while (Code != CurrentClass[Index].Code)
    {
        if (NULL == CurrentClass[Index].DescText)
        {
            return;
        }

        Index++;
    }

    //
    // A base class was found. Assign description, and check if this class has
    // sub-class defined. If sub-class defined, no more action is needed,
    // otherwise, continue to find description for the sub-class code.
    //
    ClassStrings->BaseClass = CurrentClass[Index].DescText;
    if (NULL == CurrentClass[Index].LowerLevelClass)
    {
        return;
    }

    //
    // find Subclass entry
    //
    CurrentClass = CurrentClass[Index].LowerLevelClass;
    Code = (UINT8)(ClassCode >> 8);
    Index = 0;

    //
    // Go through all entries of the sub-class, until the entry with a matching
    // sub-class code is found. If reaches an entry with a null description
    // text, the last entry is met, which means no text for the sub-class was
    // found, so no more action is needed.
    //
    while (Code != CurrentClass[Index].Code)
    {
        if (NULL == CurrentClass[Index].DescText)
        {
            return;
        }

        Index++;
    }

    //
    // A class was found for the sub-class code. Assign description, and check if
    // this sub-class has programming interface defined. If no, no more action is
    // needed, otherwise, continue to find description for the programming
    // interface.
    //
    ClassStrings->SubClass = CurrentClass[Index].DescText;
    if (NULL == CurrentClass[Index].LowerLevelClass)
    {
        return;
    }

    //
    // Find programming interface entry
    //
    CurrentClass = CurrentClass[Index].LowerLevelClass;
    Code = (UINT8)ClassCode;
    Index = 0;

    //
    // Go through all entries of the I/F entries, until the entry with a
    // matching I/F code is found. If reaches an entry with a null description
    // text, the last entry is met, which means no text was found, so no more
    // action is needed.
    //
    while (Code != CurrentClass[Index].Code)
    {
        if (NULL == CurrentClass[Index].DescText)
        {
            return;
        }

        Index++;
    }

    //
    // A class was found for the I/F code. Assign description, done!
    //
    ClassStrings->PIFClass = CurrentClass[Index].DescText;
    return;
}


// From MdeModulePkg/Bus/Pci/PciBusDxe/PciEnumeratorSupport.c
/**
  This routine is used to check whether the pci device is present.

  @param PciRootBridgeIo   Pointer to instance of EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL.
  @param Pci               Output buffer for PCI device configuration space.
  @param Bus               PCI bus NO.
  @param Device            PCI device NO.
  @param Func              PCI Func NO.

  @retval EFI_NOT_FOUND    PCI device not present.
  @retval EFI_SUCCESS      PCI device is found.

**/
EFI_STATUS
PciDevicePresent(
    IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridgeIo,
    OUT PCI_TYPE00 *Pci,
    IN UINT8 Bus,
    IN UINT8 Device,
    IN UINT8 Func)
{
    UINT64 Address;
    EFI_STATUS Status;

    //
    // Create PCI address map in terms of Bus, Device and Func
    //
    Address = EFI_PCI_ADDRESS(Bus, Device, Func, 0);

    //
    // Read the Vendor ID register
    //
    Status = PciRootBridgeIo->Pci.Read(
        PciRootBridgeIo,
        EfiPciWidthUint32,
        Address,
        1,
        Pci);

    if (!EFI_ERROR(Status) && ((Pci->Hdr).VendorId != 0xffff))
    {
        //
        // Read the entire config header for the device
        //
        Status = PciRootBridgeIo->Pci.Read(
            PciRootBridgeIo,
            EfiPciWidthUint32,
            Address,
            sizeof(PCI_TYPE00) / sizeof(UINT32),
            Pci);

        return EFI_SUCCESS;
    }

    return EFI_NOT_FOUND;
}

CHAR16 *
CapIdtoStr(IN UINT8 cap_id)
{
    switch (cap_id)
    {
    case 0x01:
        return L"CAP_ID_PMI";
        break;
    case 0x02:
        return L"CAP_ID_AGP";
        break;
    case 0x03:
        return L"CAP_ID_VPD";
        break;
    case 0x04:
        return L"CAP_ID_SLOTID";
        break;
    case 0x05:
        return L"CAP_ID_MSI";
        break;
    case 0x06:
        return L"CAP_ID_HOTPLUG";
        break;
    case 0x0C:
        return L"CAP_ID_SHPC";
        break;

    default:
        return L"unknown";
        break;
    }
    return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_PCI_IO_PROTOCOL *PciIo = NULL;

    UINTN NoHandles = 0;
    EFI_HANDLE *HandleBuffer = NULL;
    UINT16 i = 0;

    UINT16 vendorId, deviceId;
    UINT8 *CapPointer;
    UINT8 capId;

    CHAR16 *String = L"Unknow";

    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *BridgeIo = NULL;
    UINT8 Bus, Device, Func;
    PCI_TYPE00 *Pci00;
    UINT32 ClassCode;
    PCI_CLASS_STRINGS ClassStrings;

    Status = gBS->AllocatePool(EfiRuntimeServicesData, sizeof(UINT8), (VOID **)&CapPointer); // CapPointer != NULL

    // way 1
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiPciIoProtocolGuid,
        NULL,
        &NoHandles,
        &HandleBuffer);
    Print(L"PciIoProtocol NoHandles = %d\n", NoHandles);

    for (i = 0; i < NoHandles; i++)
    {
        vendorId = 0x00;
        deviceId = 0x00,
        capId = 0x00;
        *CapPointer = 0x00;
        String = L"Unknow";

        Status = gBS->HandleProtocol(
            HandleBuffer[i],
            &gEfiPciIoProtocolGuid,
            (VOID **)&PciIo);

        if (!EFI_ERROR(Status))
        {
            // READ CONFIG SPACE
            Status = PciIo->Pci.Read(
                PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &vendorId);
            if (EFI_ERROR(Status))
            {
                Print(L"Failed to read Vendor ID %r \n", Status);
                return Status;
            }
            Status = PciIo->Pci.Read(
                PciIo, EfiPciIoWidthUint16, PCI_DEVICE_ID_OFFSET, 1, &deviceId);
            if (EFI_ERROR(Status))
            {
                Print(L"Failed to read Device ID %r \n", Status);
                return Status;
            }
            Status = PciIo->Pci.Read(
                PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, CapPointer); // CapPointer != NULL
            if (EFI_ERROR(Status))
            {
                Print(L"Failed to read Capbility Pointer %r \n", Status);
                return Status;
            }

            if (*CapPointer != 0x00)
            {
                Status = PciIo->Pci.Read(
                    PciIo, EfiPciIoWidthUint8, (*CapPointer), 1, &capId);
                // Print(L"Capbility ptr = 0x%x, ID = 0x%X \n", *CapPointer, capId);
                String = CapIdtoStr(capId);
                // Print(L"%s\n",String);
            }

            Print(L"Device%d: Vendor ID = 0x%X, Device ID = 0x%X, Cap ID = 0x%02X, Cap Name = %s\n",
                  i, vendorId, deviceId, capId, String);
        }
    }

    if (HandleBuffer != NULL)
    {
        gBS->FreePool(HandleBuffer);
    }

    // way 2
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiPciRootBridgeIoProtocolGuid,
        NULL,
        &NoHandles,
        &HandleBuffer);
    Print(L"\nPciRootBridgeIoProtocol NoHandles = %d\n\n", NoHandles);

    for (i = 0; i < NoHandles; i++)
    {
        Status = gBS->HandleProtocol(
            HandleBuffer[i],
            &gEfiPciRootBridgeIoProtocolGuid,
            (VOID **)&BridgeIo);
    }

    Status = gBS->AllocatePool(EfiRuntimeServicesData, sizeof(PCI_TYPE00), (VOID **)&Pci00);
    // for (Bus = 0; Bus <= PCI_MAX_BUS; Bus++)
    // {
    Bus = 0;
    for (Device = 0; Device <= PCI_MAX_DEVICE; Device++)
    {
        for (Func = 0; Func <= PCI_MAX_FUNC; Func++)
        {
            Status = PciDevicePresent(
                BridgeIo,
                Pci00,
                Bus,
                Device,
                Func);
            if (!EFI_ERROR(Status))
            {
                ClassCode = 0;
                ClassCode |= (UINT32)Pci00->Hdr.ClassCode[0];
                ClassCode |= (UINT32)(Pci00->Hdr.ClassCode[1] << 8);
                ClassCode |= (UINT32)(Pci00->Hdr.ClassCode[2] << 16);
                // Get name from class code
                PciGetClassStrings(ClassCode, &ClassStrings);

                Print(L"BDF:%02x-%02x-%02x VID: 0x%X, DID: 0x%X, CapPtr: 0x%02x\n",
                      Bus, Device, Func, Pci00->Hdr.VendorId, Pci00->Hdr.DeviceId,
                      Pci00->Device.CapabilityPtr);
                Print(L"%s  %s  %s\n\n",
                      ClassStrings.BaseClass, ClassStrings.SubClass, ClassStrings.PIFClass);
            }
        }
    }
    // }
    if (Pci00 != NULL)
    {
        gBS->FreePool(Pci00);
    }

    return Status;
}