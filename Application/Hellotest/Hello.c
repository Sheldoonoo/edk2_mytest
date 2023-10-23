#include <Uefi.h>
#include <Library/UefiLib.h>                  //include "print" function
#include <Library/UefiBootServicesTableLib.h> //include *gBS

EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{

    // SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Hello World\n");
    Print(L"Hello World\n");

    // LocateHandleProtocol
    EFI_STATUS STATUS = EFI_SUCCESS;
    UINTN NoHandles = 0;
    EFI_HANDLE *Buffer = NULL;

    STATUS = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL,
        &NoHandles,
        &Buffer);
    if (EFI_ERROR(STATUS))
    {
        Print(L"Failed to LocateHandleBuffer\n");
        return STATUS;
    }
    else
    {
        Print(L"Succeed to LocateHandleBuffer\n");
        Print(L"Handles Numer = %d\n", NoHandles);
    }

    // OpenProtocol
    EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop = NULL;
    STATUS = gBS->OpenProtocol(
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&Gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(STATUS))
    {
        Print(L"Failed to Open Protocol\n");
        return STATUS;
    }
    else
    {
        Print(L"Succeed to Open Protocol\n");
    }

    // list resolution
    UINTN SizeOfInfo = 0;
    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = NULL;

    for (UINTN i = 0; i < Gop->Mode->MaxMode; i++)
    {
        STATUS = Gop->QueryMode(
            Gop,
            i,
            &SizeOfInfo,
            &Info);
        Print(L"Mode:%d,Ver:%d,Format:%d,Scanline:%d,Resolution:%d*%d\n",
              i,
              Info->Version,
              Info->PixelFormat,
              Info->PixelsPerScanLine,
              Info->HorizontalResolution,
              Info->VerticalResolution);
    }

    // // Set Resolution
    // STATUS = Gop->SetMode(Gop, 22);

    // // Set Red Block
    // EFI_GRAPHICS_OUTPUT_BLT_PIXEL Red = {0, 0, 255, 0}; // B G R R
    // Gop->Blt(Gop,
    //          &Red,
    //          EfiBltVideoFill,
    //          0, 0,
    //          0, 0,
    //          100, 100,
    //          0);

    // // Copy Red Block
    // Gop->Blt(Gop,
    //          &Red,
    //          EfiBltVideoToVideo,
    //          0, 0,
    //          0, 500,
    //          100, 100,
    //          0);

    return EFI_SUCCESS;
}