#include "Smbiostest.h"

STATIC CONST SHELL_PARAM_ITEM SMBIOSParamList[] = {
    {L"-add", TypeFlag},
    {L"-rm",  TypeValue},
    {L"-mod", TypeFlag},
    {NULL, TypeMax}
};


// 这里是TYPE188具体内容
SMBIOS_TABLE_TYPE188 gSmbiosType188Template = {
    {188, sizeof(SMBIOS_TABLE_TYPE188), 0}, // Hdr
    1,                                      // LastName
    2,                                      // FirstName
    25,                                     // Age
    33,                                     // Id
    3,                                      // Summary
};

CHAR8 *gSmbiosType188Strings[] = {
    "Wang",
    "XD",
    "This is a simple SMBIOS Type 188 test!",
    NULL};

SMBIOS_TEMPLATE_ENTRY MySmbiosTemplate =
    {(SMBIOS_STRUCTURE *)&gSmbiosType188Template, gSmbiosType188Strings};



/// @brief           Get Smbios Record By Type 
/// @param Type      Smbios Type
/// @param Record    the first handle of the specify Smbios Type
/// @return          EFI Status
EFI_STATUS GetSmbiosbyType(IN EFI_SMBIOS_TYPE *Type,
                           OUT EFI_SMBIOS_TABLE_HEADER **Record)
{
    EFI_STATUS Status;
    EFI_SMBIOS_HANDLE SearchSmbiosHandle;

    // get smbios. Find the first handle of the specify type
    SearchSmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = gSmbios->GetNext(gSmbios,
                              &SearchSmbiosHandle,
                              Type,
                              Record,
                              NULL);

    return Status;
}



// 主函数
EFI_STATUS
EFIAPI
UefiMain(
    IN EFI_HANDLE ImageHandle,
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status;

    UINTN ModStrNum = 1;
    UINTN ModStrNum3 = 3;
    CHAR8 *ModStr = "WANG";
    CHAR8 *ModStr3 = "SMBIOS Type 188 has been modified!";
    UINT8 Index = 0;
    CHAR8 *ReadStr = NULL;

    EFI_SMBIOS_HANDLE SearchSmbiosHandle;
    EFI_SMBIOS_TYPE SearchSmbiosType;
    EFI_SMBIOS_TABLE_HEADER *SearchRecord;

    LIST_ENTRY *Package;
    CHAR16 *ProblemParam = NULL;
    // CONST CHAR16 *ParamAdd;
    CONST CHAR16 *ParamRm;
    UINT16 intParamRm = 0;
    BOOLEAN RmFlag = FALSE;
    BOOLEAN AddFlag = FALSE;
    BOOLEAN ModFlag = FALSE;

    SearchSmbiosType = gSmbiosType188Template.Hdr.Type;

    //
    // initialize the shell lib (we must be in non-auto-init...)
    //
    Status = ShellInitialize();
    if (EFI_ERROR(Status))
    {
        Print(L"ShellInitialize Fail! Status :%r.\n", Status);
    }
    Status = CommandInit();
    if (EFI_ERROR(Status))
    {
        Print(L"CommandInit Fail! Status :%r.\n", Status);
    }
    //
    // parse the command line
    //
    Status = ShellCommandLineParse(SMBIOSParamList, &Package, &ProblemParam, TRUE);

    if (EFI_ERROR(Status))
    {
        Print(L"ShellCommandLineParse Fail! Status :%r.\n", Status);
    }
    else
    {
        if (ShellCommandLineGetFlag(Package, L"-rm"))
        {
            RmFlag = TRUE;
            ParamRm = ShellCommandLineGetValue(Package, L"-rm");
            if (ParamRm != NULL)
            {
                intParamRm = ShellStrToUintn(ParamRm);
            }
        }
        if (ShellCommandLineGetFlag(Package, L"-add"))
        {
            AddFlag = TRUE;
            // ParamAdd = ShellCommandLineGetValue(Package, L"-add");
            // if (ParamAdd != NULL)
            // {

            // }
        }
        if (ShellCommandLineGetFlag(Package, L"-mod"))
        {
            ModFlag = TRUE;
        }
        // free the command line package
        ShellCommandLineFreeVarList(Package);
    }
    Print(L"-----------------------------------\n");

    // Add SMBIOS Type
    if (AddFlag)
    {
        // Initialize SMBIOS tables from template
        Status = SmbiosLibCreateEntry(MySmbiosTemplate.Entry, MySmbiosTemplate.StringArray);
        Print(L" Add OEM SMBIOS type success!\n");

        // get smbios
        Status = GetSmbiosbyType(&SearchSmbiosType,
                                 &SearchRecord);

        SearchSmbiosType = SearchRecord->Type;
        SearchSmbiosHandle = SearchRecord->Handle;

        if (!EFI_ERROR(Status) && SearchSmbiosType == gSmbiosType188Template.Hdr.Type)
        {
            Print(L"Find OEM SMBIOS type success!\n");
            Print(L"SMBIOS TYPE  : %d\n", SearchSmbiosType);
            Print(L"SMBIOS Handle: %d\n", SearchSmbiosHandle);
            Print(L"Hdr Length   : %d\n", ((SMBIOS_TABLE_TYPE188 *)SearchRecord)->Hdr.Length);
            Print(L"Id           : %d\n", ((SMBIOS_TABLE_TYPE188 *)SearchRecord)->Id);
            Print(L"Age          : %d\n", ((SMBIOS_TABLE_TYPE188 *)SearchRecord)->Age);

            for (Index = 0; MySmbiosTemplate.StringArray[Index] != NULL; Index++)
            {
                ReadStr = SmbiosLibReadString(SearchRecord, (Index + 1)); // Index start from 1~N
                Print(L"ReadStr %d : %a\n", (Index + 1), ReadStr);
            }
        }
    }
    
    // Mod SMBIOS Type String
    if (ModFlag)
    {
        // get smbios
        Status = GetSmbiosbyType(&SearchSmbiosType,
                                 &SearchRecord);
        SearchSmbiosType = SearchRecord->Type;
        SearchSmbiosHandle = SearchRecord->Handle;

        // Type == 188
        if (SearchSmbiosType == gSmbiosType188Template.Hdr.Type)
        {
            // Modified String
            Status = gSmbios->UpdateString(gSmbios,
                                           &SearchSmbiosHandle,
                                           &ModStrNum,
                                           ModStr);

            Status = SmbiosLibUpdateString(SearchSmbiosHandle,
                                           ModStrNum3,
                                           ModStr3);

            // 修改string后 重新get smbios
            Status = GetSmbiosbyType(&SearchSmbiosType,
                                     &SearchRecord);

            ReadStr = SmbiosLibReadString(SearchRecord, ModStrNum); // Index start from 1~N
            Print(L"ModStr  %d : %a\n", ModStrNum, ReadStr);
            ReadStr = SmbiosLibReadString(SearchRecord, ModStrNum3); // Index start from 1~N
            Print(L"ModStr  %d : %a\n", ModStrNum3, ReadStr);
        }
    }

    // Remove SMBIOS Type by Handle
    if (RmFlag)
    {
        // Remove smbios type
        Status = gSmbios->Remove(gSmbios, (EFI_SMBIOS_HANDLE)intParamRm);
        Print(L"Remove SMBIOS Handle %d\n", intParamRm);

        // Verify of remove smbios type
        Status = gSmbios->GetNext(gSmbios,
                                  &intParamRm,
                                  &SearchSmbiosType,
                                  &SearchRecord,
                                  NULL);

        if (Status == EFI_NOT_FOUND)
        {
            Print(L"Remove SMBIOS Type Success! \n");
        }
        else
        {
            Print(L"Remove SMBIOS Type %d Failed! \n", intParamRm);
        }
    }

    return Status;
}