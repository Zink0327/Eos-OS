;----------Omega loader include file for BIOS----------
;    File name:loader.inc
;
;    Copyright (C) 2023 by Zink
;    This file is part of EOS
;
;    EOS is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    EOS is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.

;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <https://www.gnu.org/licenses/>.

                           ;offset
SVGAModWidth    dw 1280   ;<--0x02
SVGAModHeight   dw 720    ;0x04
SVGAModBpp       dw 24     ;0x06


BytesPerSector    dw 512         ;How many bytes on each sector(must be 512)
SectorPerCluster  db 1           ;The size of the cluster(must be 1)
ReservedSector    dw 1           ;the number of reserved sector(1 normally)
FATNumber         db 2           ;The number of FAT(must be 2)
RootEntCnt        dw 224         ;How many menu can root directory contain(224 normally)
SectorNum16       dw 2880        ;How many sectors in the disk(must be 2880)
Media             db 0xf0        ;The type of the disk(must be 0xf0)
FATSize           dw 9           ;The size of FAT(must be 9)
SectorPerTrack    dw 18          ;How many sectors on a track(must be 18)
NumberOfHeads     dw 2           ;The number of the heads(must be 2)
HiddenSector      dd 0           ;The number of hidden sector(must be 0)
SectorNum32       dd 0           ;If SectorNum16 is 0, this will be the number of sectors
DriveNumber       db 0           ;The drive number of int 13h
Reserved1         db 0           ;unused
BootSign          db 29h         ;Extended bootstrap sign
VolumeID          dd 0           ;volume id
VolumeLabel       db 'Eos OS     ';Volume Label(must be 11 byte)
FileSystem        db 'FAT12   '  ;The file system(must be 8 byte)

