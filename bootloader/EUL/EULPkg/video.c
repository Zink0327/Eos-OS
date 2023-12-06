/*----------Eos UEFI video processing----------
    File name:video.c

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
initvideo(
    IN EFI_HANDLE ImageHandle,
    OUT EFI_GRAPHICS_OUTPUT_PROTOCOL **gop)
{
UINTN NoHandles = 0;
    EFI_HANDLE *Buffer = NULL;

//Get graphics output protocol

    status = gBS->LocateHandleBuffer(
        ByProtocol,
        &gEfiGraphicsOutputProtocolGuid,
        NULL, 
        &NoHandles,
        &Buffer
    );


    if(EFI_ERROR(status))
    {
#ifdef DEBUG
        plogerr(status, "gbs->LocateHandleBuffer");
#endif
        Print("Unable to get graphical handle!");
        return status;
    }

#ifdef DEBUG
    Print(L"Graphics output handle buffer located successfully! ( gbs->LocateHandleBuffer returns %d, with %d handle(s) available )\n", status, NoHandles);
#endif

    Print(L"Graphics output handle buffer located successfully!");

//open this protocol 
    
    status = gBS->OpenProtocol(
        Buffer[0],
        &gEfiGraphicsOutputProtocolGuid,
        (VOID **)&gop,
        ImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );


    if(EFI_ERROR(status))
    {
#ifdef DEBUG
        plogerr(status, "gbs->OpenProtocal");
#endif
        Print("Unable to open graphical protocol!");
        return status;
    }

#ifdef DEBUG
    Print(L"Successfully opened graphical protocol with first handle!( gbs->OpenProtocol returns %d )\n", status);
#endif

    Print(L"Successfully opened graphical protocol with first handle!");
    return status;

}


EFI_STATUS 
setvideores(
    IN EFI_GRAPHICS_OUTPUT_PROTOCOL *gop
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *modinfo;

    UINTN modinfosize = sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION), H = 0, V = 0, index = 0, max=gop->Mode->MaxMode;

// Get screen resolution

    for(UINTN i = 0; i < max; i++)
    {
        Status = gop->QueryMode(
            gop,
            i, 
            &modinfosize, 
            &modinfo);

        H = modinfo->HorizontalResolution;
        V = modinfo->VerticalResolution;

#ifdef DEBUG
        Print(L"index %d, H = %d, V = %d.\n",
        i,
        info->HorizontalResolution,
        info->VerticalResolution);
#endif

        if((H == 1920) && (V == 1080))
        {    
            index = i;            
        }
        else if((H == 1024) && (V == 768))
        {    
            index = i;            
        }
        else if((H == 1440) && (V == 900))
        {
            index = i;
        }
    }

    Status = gop->SetMode(gop, index);    

#ifdef DEBUG
    if(EFI_ERROR(Status))
    {
        Print(L"Unable to set resolution!\n");
        return Status;
    }


    Print(L"Successfully set resolution to index %d!\n", index);
#endif
    Print(L"Set resolution successfully!");

    return Status;
}

