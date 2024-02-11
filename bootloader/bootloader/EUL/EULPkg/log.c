/*----------Eos UEFI log output----------
    File name:log.c

    Copyright (C) 2023 by Zink
    This file is part of Eos UEFI Loader

    Eos UEFI Loader is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Eos UEFI Loader is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Main.h"

void plogerr(EFI_STATUS stat, CHAR8 *funcname)
{
    EFI_STATUS Status = EFI_SUCCESS;
    switch (stat)
    {
    case EFI_INVALID_PARAMETER:
        Print(L"ERROR:EFI_INVALID_PARAMETER ");
        break;

    case EFI_NOT_FOUND:
        Print(L"ERROR:EFI_NOT_FOUND ");
        break;

    case EFI_OUT_OF_RESOURCES:
        Print(L"ERROR:EFI_OUT_OF_RESOURCES");
        break;

    default:
        Print(L"ERROR:There must be something wrong ");
        break;
    }
    Print(L"in function %s", funcname);
}

EFI_STATUS 
floginit(EFI_HANDLE ImageHandle)
{
    EFI_STATUS Status = EFI_SUCCESS;
    
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem = NULL;
    UINTN HandleCount = 0;
    EFI_HANDLE *Buffer = NULL;
    Status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid,
        NULL,
        &HandleCount,
        &Buffer
    );
    CHECK_IF_ERROR_OCCURED(Status,"floginit(): gBS->LocateHandleBuffer",L"Unable to get the handle of simple file system protocol!")
    
    Status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );
  CHECK_IF_ERROR_OCCURRED(Status,"floginit(): gBS->OpenProtocol",L"Unable to open simple file system protocol!")
    Print(L"This disk opened successfully!");

    EFI_FILE_PROTOCOL *File = NULL;
    Status = FileSystem->OpenVolume(
        FileSystem,
        &File
    );
   CHECK_IF_ERROR_OCCURRED(Status,"floginit(): FileSystem->OpenVolume",L"Unable to open this volume!")
    Print(L"This volume opened successfully!");


    File->Open(
        File,
        &LogFile,
        L"\\log.txt",
        EFI_FILE_MODE_READ|EFI_FILE_MODE_WRITE|EFI_FILE_MODE_CREATE,
        EFI_FILE_ARCHIVE
    );

   
CHECK_IF_ERROR_OCCURRED(Status,"floginit(): File->Open",L"Unable to open this file!")
    Print(L"This file opened successfully!");

    return Status;
}

EFI_STATUS 
flogwrite(CHAR8 *msg)
{
    EFI_STATUS Status = EFI_SUCCESS;
    UINTN msgLen = AsciiStrLen(msg);
    Status = LogFile->Write(LogFile, &msgLen, msg);

CHECK_IF_ERROR_OCCURRED(Status,"flogwrite(): 
LogFile->Write",L"Unable to write this file!")
    Print(L"This file was written successfully!");

    return Status;
}

EFI_STATUS 
flogclose()
{
    EFI_STATUS Status = EFI_SUCCESS;
    Status = LogFile->Close(LogFile);
  CHECK_IF_ERROR_OCCURRED(Status,"flogclose():LogFile->Close ",L"Unable to close this file!")
    Print(L"This file was closed successfully!");
    return Status;
}