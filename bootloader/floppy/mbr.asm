;----------EOS mbr for BIOS----------
;    File name:mbr.asm
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

;
;EOS Main structure:
;
;|----------------------|
;|	100000 ~ END	|
;|	   KERNEL	|
;|----------------------|  <--1MB
;|	F0000 ~ FFFFF	|
;| Extended System BIOS |
;|----------------------|
;|	C0000 ~ Effff	|
;|     Expansion Area   |
;|----------------------|
;|	A0000 ~ bffff	|
;|   Legacy Video Area  |
;|----------------------|  <--640KB
;|	9fc00 ~ 9ffff	|
;|	 Extended BIOS Data Area	|
;|----------------------|
;|	9f000 ~ 9fbff	|
;|	 Available	|
;|----------------------|
;|	90000 ~ 9f000	|
;|	 kernel tmpbuf	|
;|----------------------|
;|	10000 ~ 90000	|
;|	   LOADER	|
;|----------------------|
;|	8000 ~ 10000	|
;|	  VBE info	|
;|----------------------|
;|	7e00 ~ 8000	|
;|	  mem info	|
;|----------------------|
;|	7c00 ~ 7e00	|
;|	 MBR (BOOT)	|
;|----------------------|
;|	6c00 ~ 7c00	|
;|	 FAT data buf	|
;|----------------------|
;|	5c00 ~ 6c00	|
;|	 Root DIR Buf	|
;|----------------------|
;|	0500 ~ 5c00	|
;|	 Available	|
;|----------------------|
;|	0400 ~ 04ff	|
;|	 BIOS Data	|
;|----------------------|
;|	0000 ~ 03ff	|
;|interrupt vector table|
;|----------------------|
 
ORG 0x7c00        ;BIOS will load this to 0x7c00
 
BaseOfStack         EQU 0x7c00

BaseOfRootDirBuf EQU 5C00H
BaseOfFATBuf EQU 0x7c0
;the physical address of loader
;formula: BaseOfStack << 4 + OffsetOfLoader = physical address
BaseOfLoader       EQU 0x1000
OffsetOfLoader      EQU 0x00
 
RootDirSectors		equ	14     ;How many sectors root directory takes. It can be counted by the following formula (RootEntCnt * 32 + BytesPerSector - 1) / BytesPerSector = RootDirSectors
RootDirStartSector	equ	19   ;ReservedSector + FATSize * FATNumber
FAT1StartSector	equ	1    
SectorBalance		equ	17   ;To balance the differences between the data cluster and that in FAT
 
JMP start
NOP

;BPB data. See docs/bootloader/Fat12.txt for more detailed information.

OEMName           db 'EOS OS  '  
BytesPerSector    dw 512       
SectorPerCluster  db 1           
ReservedSector    dw 1           
FATNumber         db 2           
RootEntCnt        dw 224       
SectorNum16       dw 2880        
Media             db 0xf0       
FATSize           dw 9           
SectorPerTrack    dw 18          
NumberOfHeads     dw 2           
HiddenSector      dd 0           
SectorNum32       dd 0           
DriveNumber       db 0           
Reserved1         db 0           
BootSign          db 29h         
VolumeID          dd 0           
VolumeLabel       db 'Eos OS     '
FileSystem        db 'FAT12   '  
;========================

start:
;init registers first
MOV AX,CS
MOV DS,AX
MOV ES,AX
MOV SS,AX
MOV SP,BaseOfStack
 
;show welcome message
MOV SI,WelcomeMsg
CALL printstr
CALL newline

;reset floppy
xor ah,ah
xor dl,dl
int 13h

;search loader.bin
MOV WORD [SectorNo],RootDirStartSector
 
SearchBegin:
CMP WORD [RootDirSizeForLoop],0
JZ noloader
DEC WORD [RootDirSizeForLoop]
MOV AX,00H
MOV ES,AX
MOV BX,BaseOfRootDirBuf
MOV AX,[SectorNo]
MOV CL,1
CALL read1sector
MOV SI,LoaderFileName
MOV DI,BaseOfRootDirBuf
CLD	;si++;di++;
MOV DX,10H
 
searchloader:
CMP DX,0
JZ gotonextsec
DEC DX
MOV CX,11
 
cmpfilename:
CMP CX,0
JZ found
DEC CX
LODSB	;load 1 byte to al
CMP AL,BYTE [ES:DI]
JZ go_on
JMP different
 
go_on:
INC DI
JMP cmpfilename
 
different:
AND DI,0ffe0h   ;make di point to the begin of the present root dir entry
ADD DI,20h   ;point to the next root dir entry
MOV SI,LoaderFileName
JMP searchloader
 
gotonextsec:
INC WORD [SectorNo]
JMP SearchBegin
 
noloader:
MOV SI,noloaderbin
CALL printstr
JMP $
 
found:
MOV SI,nok
CALL printstr
MOV AX,RootDirSectors
AND DI,	0ffe0h   ;make di point to the beginning of the present root dir entry
ADD DI,01ah     ;point to the first sector of the file
;root directory structure in fat 12
;member		offset	length	description
;DIR_NAME	0x00	0x0B	File name
;DIR_ATTR	0x0B	0x01	File property
;Reserved	0x0C	0x0A	Reserved
;DIR_WRTTIME	0x16	0x02	The latest writing time
;DIR_WRTDATE	0x18	0x02	The latest writing date
;DIR_FSTCLUS	0x1A	0x02	The beginning cluster of the file
;DIR_FILESIZE	0x1C	0x04	File size

MOV CX,WORD [ES:DI]
PUSH CX    ;save the number of this sector in fat

ADD CX,AX
ADD CX,SectorBalance

;the address of loader
MOV AX,BaseOfLoader
MOV ES,AX
MOV BX,OffsetOfLoader

;the LBA of the sector
MOV AX,CX
 
go_onloadingfile:
;how many sectors to read
MOV CL,1
CALL read1sector
POP AX   ;pick the number of this sector in fat to ax(which is pushed by'PUSH CX')
CALL getfatentry
CMP AX,0fffh
JZ loaded
PUSH AX    ;save again
MOV DX,RootDirSectors
ADD AX,DX
ADD AX,SectorBalance
ADD BX,[BytesPerSector]
JMP go_onloadingfile
 
loaded:
JMP BaseOfLoader:OffsetOfLoader
 
 
;==========Functions==========
;Function name: numtoascii
;usage: turn number to ascii code
;input value: cl - input number
;return value: al - the second digits ascii code of the number
;              ah - the first digits ascii code of the number
;numtoascii:     
;     mov ax,0
;     mov al,cl  
;     mov bl,10
;     div bl
;     add ax,3030h
;     ret
 
 
;Function name: readinfo
;usage: read a sector
;input value: no
;return value: no
;readinfo:       
;     mov si,cylind
;     call  printstr
;     mov si,header
;     call  printstr
;     mov si,sector
;     call  printstr
;     ret
 
;Function name: read1sector
;usage: read a sector
;input value: ax - lba number
;             cl - the number of sectors
;             es:bx - buffer
;return value: no
read1sector:
        PUSH BP
        MOV BP,SP
        SUB SP,2
        MOV BYTE [BP - 2],CL;to save the value of cl
 
       PUSH AX

;transfer lba to chs
;it cannot be in a function
       
       MOV CL,[SectorPerTrack]
       DIV CL                           ; al = LBA / SPT, ah = LBA % SPT


; cylinder = LBA / SPT / HPC

        MOV CH,AL
        SHR CH,1                ; ch = cylinder               


; head = (LBA / SPT) % HPC
       
       MOV DH,AL
       AND DH,1                        ; dh = head


; sector = LBA % SPT + 1

      MOV CL,AH
	INC CL                           ; cl = sector

       
 
 ;      MOV        [cylind+8] ,CH
;       MOV        [header+8],DH
;       MOV        [sector+8],cl
 
;       mov   cl,[cylind+8]
;       call  numtoascii
;       mov   [cylind+5],al
;       mov   [cylind+6],ah
 
;       mov   cl,[header+8]
;       call  numtoascii
;       mov   [header+5],al
;       mov   [header+6],ah
 
;       MOV   cl, [sector+8]     
;       CALL  numtoascii
;       mov   [sector+5],al
;       mov   [sector+6],ah
        POP AX
        MOV        DI,0
retry:
        MOV        AH,02H            ;read disk
        MOV        AL,BYTE [BP - 2] ;how many sectors to read           
        MOV        DL,00H           ;Drive A
        INT        13H               ;BIOS Disk Function
        JNC        readok           
        INC     DI
        CMP     DI, 5           ; Floppy is weak, if it is not successful for over 5 times, give it up
        JAE     errorp            ;>=
        MOV     AH,0x00        ;reset disk
        MOV     DL,0x00         ; A:
        INT     13H            
        JMP retry
errorp:
           MOV     SI, Floppyerror
           CALL    printstr
           CALL    newline
           JMP     exitread
readok:    MOV     SI, FloppyOK
           CALL    printstr
           CALL    newline
exitread:
        ADD SP,2
        POP BP
           RET
 
;Function name: getfatentry
;usage: get the data cluster from that in fat
;input value: ax - data cluster in fat
;return value: ax - the lba number of the cluster
getfatentry:
 
	push	es
	push	bx
	push	ax
	mov	ax,	BaseOfFATBuf
	sub ax,0100h
	mov	es,	ax
	pop	ax
;1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8  1 2 3 4 5 6 7 8
;(         first FAT data         )(        second FAT data         )
;test if the fat data is in the lasr 12 bit
	mov	byte	[Odd],	0
	mov	bx,	3   
	mul	bx   ;dx = ax *3
	mov	bx,	2
	div	bx   ;dx = ax mod 2
	cmp	dx,	0
	jz even
;if not, it is in the first 12 bit
	mov	byte	[Odd],	1
 
even:
 
	xor	dx,	dx
	mov	bx,	[BytesPerSector]
	div	bx
	push	dx
	mov	bx,	0
	add	ax,	FAT1StartSector
	mov	cl,	2
	call	read1sector
 
	pop	dx
	add	bx,	dx
	mov	ax,	[es:bx]
	cmp	byte	[Odd],	1
	jnz	even2
	shr	ax,	4
 
even2:
	and	ax,	0fffh
	pop	bx
	pop	es
	ret
 
;function name: printstr
;usage: print a string end with '$'
;input value: si - string
;return value: no
printstr:               
      mov al,[si]
      cmp al,'$'
      je disover
      mov ah,0eh
      int 10h
      inc si
      jmp printstr
disover:
      ret
 
;function name: newline
;usage: print return
;input value: no
;return value: no
newline:                     
      mov ah,0eh
      mov al,0dh
      int 10h
      mov al,0ah
      int 10h
      ret
 
;some necessary string
WelcomeMsg DB 'Booting','$'
LoaderFileName:		DB	"LOADER  BIN",0
noloaderbin DB 'no loader!','$'
Floppyerror DB 'ERR','$'
FloppyOK DB 'OK','$'
nok db '.$'
;tmp variable
RootDirSizeForLoop	dw	RootDirSectors
SectorNo		dw	0
Odd			db	0
;cylind  db 'cyl:?? $',0    
;header  db 'hds:?? $',0    
;sector  db 'sct:?? $',1    
 
;magic number
TIMES 510-($-$$) DB 0
DB 0x55, 0xaa