#include <Uefi.h>
#include <Library/UefiLib.h>                  //include "print" function
#include <Library/UefiBootServicesTableLib.h> //include *gBS
#include "../../Protocol/MyProtocol.h"


EFI_STATUS
EFIAPI
Error_Return_Fun(EFI_STATUS Status)
{
    if (EFI_ERROR(Status))
    {
        Print(L"Failed to operate protocol: %r\n", Status);
        return Status;
    }
    return Status;
}


EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_HANDLE *Buffer =NULL;
    UINTN NoHandles = 0;
    MY_PROTOCOL *Protocol = NULL;
    EFI_OPEN_PROTOCOL_INFORMATION_ENTRY *EntryBuffer=NULL;
    UINTN EntryCount = 0;

    // Install Protocol
    Print(L"Succeed to InstallProtoco -- GUID: %x-%x-%x-%x%x%x%x%x%x%x%x\n",
          gEfiMyProtocolGuid.Data1,
          gEfiMyProtocolGuid.Data2,
          gEfiMyProtocolGuid.Data3,
          gEfiMyProtocolGuid.Data4[0],gEfiMyProtocolGuid.Data4[1],
          gEfiMyProtocolGuid.Data4[2],gEfiMyProtocolGuid.Data4[3],
          gEfiMyProtocolGuid.Data4[4],gEfiMyProtocolGuid.Data4[5],
          gEfiMyProtocolGuid.Data4[6],gEfiMyProtocolGuid.Data4[7]);
    
    
    // LocateHandleProtocol
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiMyProtocolGuid,
        NULL,
        &NoHandles,
        &Buffer);

    Error_Return_Fun(Status);
    Print(L"Succeed to LocateHandleBuffer -- Handles Numer = %d\n", NoHandles);
    

    // Locate Protocol
    Status = gBS->LocateProtocol(
        &gEfiMyProtocolGuid,
        NULL,
        (VOID **)&Protocol);

    Error_Return_Fun(Status);
    Print(L"Succeed to LocateProtoco -- Name: %s\n", Protocol->Name);


    // HandleProtocol
    Status = gBS->HandleProtocol(
        Buffer[0],
        &gEfiMyProtocolGuid,
        (VOID **)&Protocol);

    Error_Return_Fun(Status);
    Print(L"Succeed to HandleProtocol -- Name: %s\n", Protocol->Name);

    
    // OpenProtocol
    Status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiMyProtocolGuid,
        (VOID **)&Protocol,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL);

    Error_Return_Fun(Status);
    Print(L"Succeed to OpenProtocol -- Name: %s\n", Protocol->Name);
    Print(L"Protocol Function: ");
    Status = Protocol->My_Function(Protocol,8);


    // OpenProtocol Information
    Status = gBS->OpenProtocolInformation(
        Buffer[0],
        &gEfiMyProtocolGuid,
        (EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **)&EntryBuffer,
        &EntryCount
    );

    Error_Return_Fun(Status);
    Print(L"Succeed to OpenProtocol Information -- Opencount: %d %d\n", EntryCount,EntryBuffer->OpenCount);


    // CloseProtocol
    Status = gBS->CloseProtocol(
        Buffer[0],
        &gEfiMyProtocolGuid,
        ImageHandle,
        NULL
    );

    Error_Return_Fun(Status);
    Print(L"Succeed to CloseProtocol \n");


    // OpenProtocol Information 
    Status = gBS->OpenProtocolInformation(
        Buffer[0],
        &gEfiMyProtocolGuid,
        (EFI_OPEN_PROTOCOL_INFORMATION_ENTRY **)&EntryBuffer,
        &EntryCount);

    Error_Return_Fun(Status);
    Print(L"Succeed to OpenProtocol Information -- Opencount: %d %d\n", EntryCount,EntryBuffer->OpenCount);

    if (Buffer != NULL)
    {
        gBS->FreePool(Buffer);
    }

    return EFI_SUCCESS;
}