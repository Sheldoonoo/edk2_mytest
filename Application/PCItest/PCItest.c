#include <Uefi.h>
#include <Library/UefiLib.h>                  //include "print" function
#include <Library/UefiBootServicesTableLib.h> //include *gBS
#include <Library/IoLib.h>

#define CMOS_INDEX 0x70
#define CMOS_DATA 0x71

#define PCI_CONF_ADDR 0x0CF8
#define PCI_CONF_DATA 0x0CFC

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{

    Print(L"<< < < << PCI test app >> > > >>\n\n");

    Print(L">>>>> I. Read IO Port\n");
    UINTN IO_Value = 0x00;
    IO_Value = IoRead8(0X80);
    Print(L"The value of IO Port 0x80 is 0x%x\n\n", IO_Value);

    Print(L">>>>> II. Write&Read Memory Address\n");
    volatile UINTN *MEM_ADDR = (VOID *)0X7E00001;
    Print(L"The value of Mem_Addr 0x%x is 0x%x\n", MEM_ADDR, *MEM_ADDR);

    *MEM_ADDR = 0X12;
    Print(L"The value of Mem_Addr 0x%x is 0x%x\n\n", MEM_ADDR, *MEM_ADDR);

    Print(L">>>>> III. Read CMOS\n");
    UINTN year = 0, month = 0, day = 0, cmos_value = 0;
    IoWrite8(CMOS_INDEX, 0x09);
    year = IoRead8(CMOS_DATA);
    IoWrite8(CMOS_INDEX, 0x08);
    month = IoRead8(CMOS_DATA);
    IoWrite8(CMOS_INDEX, 0x07);
    day = IoRead8(CMOS_DATA);
    Print(L"Today is %x/%x/%x\n", year, month, day);

    IoWrite8(CMOS_INDEX, 0x37);
    cmos_value = IoRead8(CMOS_DATA);
    Print(L"The data of CMOS Offest 0x37 is 0x%x\n\n", cmos_value);

    Print(L">>>>> IV. Read PCI\n");
    UINTN PCI_Value = 0x00;
    UINTN PCI_Reg = 0x00;
    // for (UINTN i=0x00;i<0x40;i+=0x04)
    // {
    //   //BDF 00:31.00
    //   PCI_Reg =  0x8000F800+i;
    //   IoWrite32(PCI_CONF_ADDR,PCI_Reg);
    //   PCI_Value = IoRead32(PCI_CONF_DATA);
    //   Print(L"The value of PCI 0x%x is 0x%x\n",PCI_Reg,PCI_Value);
    // }

    for (UINTN i = 0x00; i < 0x40; i += 0x04)
    {
        // BDF 0x00:0x01.0x00   UEFI shell: pci 00 01 00 -i
        PCI_Reg = 0x80000000 + (0X01 << 11) + i;
        IoWrite32(PCI_CONF_ADDR, PCI_Reg);
        PCI_Value = IoRead32(PCI_CONF_DATA);
        Print(L"The value of PCI 0x%x is 0x%x\n", PCI_Reg, PCI_Value);
    }

    UINT32 address;
    UINT32 baseAdress;
    address = 0x80000000 | ((0x00 & 0xFF) << 16) | ((0x00 & 0x1F) << 11) | ((0x00 & 0x7) << 8) | (0x60 & 0xfffffffc);
    IoWrite32(PCI_CONF_ADDR, address);                 // read host bridge 60h value
    baseAdress = IoRead32(PCI_CONF_DATA) & 0xFF000000; // Get High 8 Bits
    Print(L"The PCI MMIO baseAdress is 0x%x\n", baseAdress);

    volatile UINT32 *MEM_Base_ADDR = (VOID *)0xE0000000;
    UINT32 PCI_Base_Reg = 0xE0000000;
    for (UINTN i = 0x00; i < 0x40; i += 0x04)
    {
        // BDF 0x00:0x01.0x00   UEFI shell: pci 00 01 00 -i
        PCI_Base_Reg = 0xE0000000 + (0x01 << 15) + i;
        PCI_Value = MmioRead32(PCI_Base_Reg);
        Print(L"The value of PCI 0x%x is 0x%x\n", PCI_Base_Reg, PCI_Value);

        MEM_Base_ADDR = (VOID *)0xE0000000 + (0x01 << 15) + i;
        Print(L"The value of PCI 0x%0llx is 0x%x\n", MEM_Base_ADDR, *MEM_Base_ADDR);
    }

    return EFI_SUCCESS;
}