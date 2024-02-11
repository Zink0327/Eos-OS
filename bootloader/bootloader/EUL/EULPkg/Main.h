/*----------Eos UEFI loader include file----------
    File name:Main.h

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

/* General including file */

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>


#include <Library/BaseMemoryLib.h>
#include  <Library/MemoryAllocationLib.h>


#define CHECK_IF_ERROR_OCCURRED(s,f,m) if(EFI_ERROR(s)) {  \
#ifdef DEBUG           \
plogerr(s, f);    \
#endif     \
Print(m);  return s; }

/* log.c */
#include <Guid/FileInfo.h>
#include <Protocol/SimpleFileSystem.h>

// Print error information on screen
void 
plogerr(
    EFI_STATUS stat, 
    CHAR8 *funcname);

EFI_FILE_PROTOCOL *LogFile;

EFI_STATUS 
floginit(EFI_HANDLE ImageHandle);

EFI_STATUS 
flogwrite(CHAR8 *msg);

EFI_STATUS 
flogclose();

/* video.c */
typedef struct {
    EFI_PHYSICAL_ADDRESS       FrameBufferBase;
    UINTN                      FrameBufferSize;
    UINT32                     HorizontalResolution;
    UINT32                     VerticalResolution;
    UINT32                     PixelsPerScanLine;
} VIDEOINFO;


typedef struct
{
    UINTN Size;
    UINTN PageSize;
    UINTN Width;
    UINTN Height;
    UINTN Offset;
    UINT64 PixelStart;
} BMP_INFO;


// Initialize video handle
EFI_STATUS 
initvideo(
    IN EFI_HANDLE ImageHandle, 
    OUT EFI_GRAPHICS_OUTPUT_PROTOCOL **gop);

// Set video resolution
EFI_STATUS 
setvideores(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *gop);

/* file.c */

// Open and get a file's handle
EFI_STATUS 
OpenFile(
     IN EFI_HANDLE ImageHandle, 
     IN CHAR16 *FileName,
     OUT EFI_FILE_PROTOCOL **FileHandle );

// read a file (require to run OpenFile first)
EFI_STATUS 
ReadFile(
    IN EFI_FILE_PROTOCOL *file,
    OUT EFI_PHYSICAL_ADDRESS *Buffer);

/* bmp.c */

EFI_STATUS 
DrawBmp(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *Gop,
    IN BMP_INFO BmpConfig,
    IN UINTN X,
    IN UINTN Y);
