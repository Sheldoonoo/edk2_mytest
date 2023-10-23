#include "MyProtocolInsDxe.h"


EFI_STATUS
EFIAPI
MyProtocolFunction(IN MY_PROTOCOL *This,
                   IN UINT32 Number)
{
    Print(L"Hello %s--%d!\n", This->Name, Number);
    return EFI_SUCCESS;
}


MY_PROTOCOL gMyProtocol ;


EFI_STATUS
EFIAPI
MyProtocolInsEntryPoint(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{

    EFI_STATUS Status;

    gMyProtocol.Revision = 0X000001;
    gMyProtocol.Name = L"My Protocol";
    gMyProtocol.My_Function = MyProtocolFunction;

    Status = gBS->InstallMultipleProtocolInterfaces(
        &ImageHandle,
        &gEfiMyProtocolGuid,
        &gMyProtocol,
        NULL);

    return Status;
}