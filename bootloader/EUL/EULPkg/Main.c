/*----------Eos UEFI Loader entry----------
    File name:Main.c

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

EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop;
VIDEOINFO *videoinfo;
EFI_STATUS
EFIAPI
efi_main(
    IN EFI_HANDLE ImageHandle, 
    IN EFI_SYSTEM_TABLE *SystemTable)
{
    EFI_STATUS Status = EFI_SUCCESS;

//  ------------Set up video output------------
// Get video handle
    Status = initvideo(ImageHandle, &Gop);
    CHECK_IF_ERROR_OCCURRED(Status,"initvideo",L"Unable to initalize video!\n")
// Set video resolution
    Status = setvideores(Gop);
    CHECK_IF_ERROR_OCCURRED(Status,"setvideores",L"Unable to set video resolution!\n")

// Save related video information

    videoinfo->FrameBufferBase = Gop->Mode->FrameBufferBase;
    videoinfo->FrameBufferSize = Gop->Mode->FrameBufferSize;
    videoinfo->HorizontalResolution = Gop->Mode->Info->HorizontalResolution;
    videoinfo->VerticalResolution = Gop->Mode->Info->VerticalResolution;
    videoinfo->PixelsPerScanLine = Gop->Mode->Info->PixelsPerScanLine;

//  ------------draw logo------------
    
    CHAR16 *FileName = L"\\logo.BMP"; 
    UINTN Hor = Gop->Mode->Info->HorizontalResolution;
    UINTN Ver = Gop->Mode->Info->VerticalResolution;
    EFI_FILE_PROTOCOL *LogoFile;
    Status = OpenFile(ImageHandle, FileName, &LogoFile);
    
    EFI_PHYSICAL_ADDRESS LogoAddress;
    Status = ReadFile(LogoFile, &LogoAddress);

    BMP_INFO BmpInfo;

    UINTN X = (Hor - BmpInfo.Width) / 2;
    UINTN Y = (Ver - BmpInfo.Height) / 2;

    Status = DrawBmp(Gop, BmpInfo, X, Y);

// ------------Load kernel------------
    
    EFI_FILE_PROTOCOL *KernelFile;
    Status = OpenFile(ImageHandle, FileName, &KernelFile);
    
    EFI_PHYSICAL_ADDRESS KernelAddress;
    Status = ReadFile(KernelFile, &KernelAddress);

//  ------------jump to kernel----------
    asm("jmp %0" : : "m"(KernelAddress));

    return Status;
}
