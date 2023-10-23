#ifndef __EFI_MY_PROTOCOL_H__
#define __EFI_MY_PROTOCOL_H__

EFI_GUID EFI_MY_PROTOCOL_GUID = { 0x99C55252, 0x54D1, 0x4596, { 0xA5, 0x6A, 0x7E, 0x52, 0xFB, 0xE4, 0x7A, 0xBE }};
//EFI_GUID gEfiMyProtocolGuid = { 0x99C55252, 0x54D1, 0x4596, { 0xA5, 0x6A, 0x7E, 0x52, 0xFB, 0xE4, 0x7A, 0xBE }};

typedef struct _MY_PROTOCOL MY_PROTOCOL;

typedef EFI_STATUS(EFIAPI *MY_FUNCTION)(
    IN MY_PROTOCOL *This,
    IN UINT32 Number);

struct _MY_PROTOCOL {
    UINT64 Revision;
    CHAR16 *Name;
    MY_FUNCTION My_Function;
};


extern EFI_GUID  gEfiMyProtocolGuid;

#endif