/*----------Eos UEFI file processing library----------
    File name:file.c

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

EFI_STATUS 
OpenFile(
     IN EFI_HANDLE ImageHandle, 
     IN CHAR16 *FileName,
     OUT EFI_FILE_PROTOCOL **FileHandle )
 { 
     EFI_STATUS Status = EFI_SUCCESS; 
     UINTN HandleCount = 0; 
    EFI_HANDLE *HandleBuffer; 
    Status = gBS->LocateHandleBuffer( 
        ByProtocol,
        &gEfiSimpleFileSystemProtocolGuid, 
        NULL,
        &HandleCount,
        &HandleBuffer
      );
    CHECK_IF_ERROR_OCCURRED(Status,"gBS->LocateHandleBuffer",L"Failed to locate simple file system handle buffer!")

// Open this disk...

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FileSystem; 
    Status = gBS->OpenProtocol( 
        HandleBuffer[0],
        &gEfiSimpleFileSystemProtocolGuid,
        (VOID **)&FileSystem, 
        ImageHandle, 
        NULL, 
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
 );
    CHECK_IF_ERROR_OCCURRED(Status,"gBS->OpenProtocol",L"Unable to open simple file system protocol!")
    Print(L"This disk opened successfully!");

// And open this volume... 

    EFI_FILE_PROTOCOL *Root; 
    Status = FileSystem->OpenVolume( FileSystem, &Root );
    CHECK_IF_ERROR_OCCURRED(Status,"FileSystem->OpenVolume",L"Unable to open this volume!")
    Print(L"This volume opened successfully!");

// Then read this file...

    Status = Root->Open(
        Root, 
        FileHandle,
        FileName, // for example, L"\\example.txt" 
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
 );
    CHECK_IF_ERROR_OCCURRED(Status,"Root->Open",L"Unable to open this file!")
    Print(L"This file opened successfully!");

    return Status;
}



EFI_STATUS 
ReadFile(
    IN EFI_FILE_PROTOCOL *file,
    OUT EFI_PHYSICAL_ADDRESS *Buffer
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_FILE_INFO *FileInfo;

    UINTN InfoSize = sizeof(EFI_FILE_INFO) + 128;
    Status = gBS->AllocatePool(
        EfiLoaderData,
        InfoSize,
        (VOID **)&FileInfo
    );

    CHECK_IF_ERROR_OCCURRED(Status,"gBS->AllocatePool",L"Unable to allocate buffer for the info of this file!")

    Status = file->GetInfo(
        file,
        &gEfiFileInfoGuid,
        &InfoSize,
        FileInfo
    );
    
    CHECK_IF_ERROR_OCCURRED(Status,"file->GetInfo",L"Unable to   get the info of this file!")
    
    UINTN FilePageSize = (FileInfo->FileSize >> 12) + 1;
    
    EFI_PHYSICAL_ADDRESS FileBufferAddress;
    Status = gBS->AllocatePages(
        AllocateAnyPages,
        EfiLoaderData,
        FilePageSize,
        &FileBufferAddress
    );

    CHECK_IF_ERROR_OCCURRED(Status,"gBS->AllocatePages",L"Unable to allocate pages for this file!")

    UINTN ReadSize = FileInfo->FileSize;
    Status = file->Read(
        File,
        &ReadSize,
        (VOID *)FileBufferAddress
    );
    CHECK_IF_ERROR_OCCURRED(Status,"file->Read",L"Unable to read this file!")
    gBS->FreePool(FileInfo);
    *Buffer = FileBufferAddress;
    return Status;
}
