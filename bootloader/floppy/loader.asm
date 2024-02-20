;----------Omega loader for BIOS----------
;    File name:loader.asm
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
;|Extended BIOS Data Area|
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

ORG 0x10000
 
JMP start    ;2 bytes

%include "./loader.inc"

;the physical address of kernel
;formula: BaseOfStack << 4 + OffsetOfLoader = physical address
BaseOfKernel       EQU 0x00
OffsetOfKernel      EQU 0x100000

;before be loaded to 10000:0000,the kernel will be loaded to here
BaseTmpOfKernel equ 0x00
OffsetTmpOfKernel equ 0x7E00

BaseOfFATBuf EQU 0x6c00
BaseOfRootDirBuf EQU 5c00H
MemoryStructBufferAddr equ 0x7E00

SVGAModInfoBufAddr equ 0x8200


 
RootDirSectors		equ	14     ;How many sectors root directory takes. It can be counted by the following formula (RootEntCnt * 32 + BytesPerSector - 1) / BytesPerSector = RootDirSectors
RootDirStartSector	equ	19   ;ReservedSector + FATSize * FATNumber
FAT1StartSector	equ	1    
SectorBalance		equ	17   ;To balance the differences between the data cluster and that in FAT


[SECTION gdt]

GDT: dd 0,0    ;zero segment, Intel's requirement.
DESC_CODE32: dd 0x0000FFFF,0x00CF9A00    ;code segment
DESC_DATA32: dd 0x0000FFFF,0x00CF9200    ;data segment

GdtLen equ $ - GDT
GdtPtr dw GdtLen - 1
       dd GDT ;be careful about the address(after using org)!!!!!!

SelectorCode32 equ DESC_CODE32 - GDT
SelectorData32 equ DESC_DATA32 - GDT


[SECTION gdt64]

GDT64:	 dq 0x0000000000000000
DESC_CODE64: dq 0x0020980000000000
DESC_DATA64: dq 0x0000920000000000

GdtLen64 equ $ - GDT64
GdtPtr64 dw GdtLen64 - 1
		dd GDT64

SelectorCode64 equ DESC_CODE64 - GDT64
SelectorData64 equ DESC_DATA64 - GDT64

[SECTION .s16]
[BITS 16]
start:
;init registers first...
MOV AX,CS
MOV DS,AX
MOV ES,AX
MOV SS,AX
;because loader is a part of kernel, it should use absolute address
MOV SP,0x00

MOV AX,0B800h
MOV GS,AX

;show welcome message
MOV SI,Welcomemsg
CALL printstr
CALL newline

;enter big real mode

;open a20 address
;a must-do to enter protect mode
IN AL,92h
OR AL,00000010b
OUT 92H,AL

;since there isn't an IDT there, disable interrupt first
CLI

;create GDT
LGDT [GdtPtr]

;open the switch of protect mode
MOV EAX,CR0
OR EAX,1
MOV CR0,EAX

MOV AX,SelectorData32
MOV FS,AX
;rewrite cr0 to enter big real mode
MOV EAX,CR0
AND AL,	11111110b  
MOV CR0,EAX
 
STI

;reset floppy
XOR AH,AH
XOR DL,DL
INT 13H

;search kernel.bin
MOV WORD [SectorNo],RootDirStartSector
 
SearchBegin:
CMP WORD [RootDirSizeForLoop],0
JZ noloader
DEC WORD [RootDirSizeForLoop]
MOV AX,00H
MOV ES,AX
;load root DIR to 0x5c00, we did that in MBR before
;MOV BX,BaseOfRootDirBuf
;MOV AX,[SectorNo]
;MOV CL,1
;CALL read1sector
MOV SI,KernelFileName
MOV DI,BaseOfRootDirBuf
CLD
MOV DX,10H
 
searchforloader:
CMP DX,0
JZ gotonextsec
DEC DX
MOV CX,11
 
cmpfilename:
CMP CX,0
JZ found
DEC CX
LODSB
CMP AL,BYTE [ES:DI]
JZ go_on
JMP different
 
go_on:
INC DI
JMP cmpfilename
 
different:
AND DI,0ffe0h
ADD DI,20h
MOV SI,KernelFileName
JMP searchforloader
 
gotonextsec:
INC WORD [SectorNo]
JMP SearchBegin
 
noloader:
MOV SI,nokernelbin
CALL printstr
JMP die

;move kernel
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
MOV AX,BaseTmpOfKernel
MOV ES,AX
MOV BX,OffsetTmpOfKernel
MOV AX,CX
 
go_onloadingfile:
MOV CL,1
CALL read1sector
POP AX   ;pick the number of this sector in fat

PUSH CX
PUSH EAX
PUSH FS
PUSH EDI
PUSH DS
PUSH ESI

MOV CX,200h
MOV AX,BaseOfKernel
MOV FS,AX
MOV EDI,DWORD [OffsetOfKernelFileCount]

MOV AX,BaseTmpOfKernel
MOV DS,AX
MOV ESI,OffsetTmpOfKernel

movekernel:
;move the kernel to 10000:0000 byte by byte
MOV AL,BYTE [DS:ESI]
MOV BYTE [FS:EDI],AL

INC ESI
INC EDI
;loop until cx=0
LOOP movekernel

MOV AX,0x1000
MOV DS,AX

MOV DWORD [OffsetOfKernelFileCount],edi

POP ESI
POP DS
POP EDI
POP FS
POP EAX
POP CX

CALL getfatentry
CMP AX,0fffh
JE loaded
PUSH AX    ;save again
MOV DX,RootDirSectors
ADD AX,DX
ADD AX,SectorBalance
;ADD BX,BytesPerSector
JMP go_onloadingfile
 
 
loaded:
PUSH SI
MOV SI,KernelOK
CALL printstr
CALL newline
POP SI

killmotor:
PUSH DX
MOV DX,03F2h
MOV AL,0
OUT DX,AL
POP DX

;get memory address size type
;**  Check docs/bootloader/QuerySystemAddressMap.txt for more detailed info.  **

MOV SI,GetMemStrMsg
CALL printstr
CALL newline

MOV EBX,0 
MOV AX,00h
MOV ES,AX
MOV DI,MemoryStructBufferAddr


getmemstruct:     

MOV EAX,0x0E820      
MOV ECX,20           
MOV EDX,0x534D4150     ;‘SMAP’

INT 15H

JC getmemerr
ADD DI,20
INC DWORD [MemStructNumber]
CMP EBX,0
JNE getmemstruct
JMP getmemok

getmemerr:
MOV SI,Floppyerror
CALL printstr
CALL newline
JMP die

getmemok:
MOV SI,FloppyOK
CALL printstr
CALL newline

;get SVGA information
;** See docs/bootloader/SVGA-processing.txt for more detailed info. **

MOV SI,Getsvgamsg
CALL printstr
CALL newline

;MOV	CX,	23


MOV AX,00H
MOV ES,AX
MOV DI,0x8000       
MOV AX,4F00H        ;video memory size

INT 10H

CMP AX,004Fh     
JZ getsvgaok


Getsvgafail:
MOV SI,Floppyerror
CALL printstr
CALL newline
JMP die

getsvgaok:
MOV SI,FloppyOK
CALL printstr
CALL newline

;get SVGA mode info

MOV SI,Getsvgamodmsg
CALL printstr
CALL newline

MOV	BX,	000FH
MOV	DX,	0C00H		;ROW 12

MOV AX,00H
MOV ES,AX
MOV SI,800Eh

MOV ESI,DWORD [ES:SI]
MOV EDI,SVGAModInfoBufAddr

getsvgamodinfo:
MOV CX,WORD [ES:ESI]   ; get the first value of SVGA mode
PUSH CX

;display SVGA mode
PUSH AX
MOV AX,00H
MOV AL,CH
CALL displayal
MOV AX,00H
MOV AL,CL
CALL displayal
POP AX

POP CX
CMP CX,0FFFFH
JZ getsvgamodfinished

MOV AX,4F01H
INT 10H
CMP AX,004FH
JNZ getsvgamodfailed
INC DWORD [SVGAModeCounter]

;select the video mode
PUSH AX
MOV AX,WORD [ES:EDI]
AND AX,90H   ;Check if this is a graphics mode with linear frame buffer support
CMP AX,90H
JNZ finished

MOV AX,WORD [ES:EDI + 12H]
CMP AX,[SVGAModWidth]
JNZ continue
MOV AX,WORD [ES:EDI + 14H]
CMP AX,[SVGAModHeight]
JNZ continue
MOV AX,WORD [ES:EDI + 19H]
CMP AX,[SVGAModBpp]
JNZ continue
MOV WORD [BestSVGAModeNo],CX
JMP getsvgamodfinished

continue:
MOV AX,WORD [ES:EDI + 12H]
CMP AX,[SVGAModWidthB]
JNZ finished
MOV AX,WORD [ES:EDI + 14H]
CMP AX,[SVGAModHeightB]
JNZ finished
MOV AX,WORD [ES:EDI + 19H]
CMP AX,[SVGAModBpp]
JNZ finished
MOV WORD [BestSVGAModeNo],CX

finished:
POP AX

ADD ESI,2
;ADD EDI,100H   ;if the mode is not correct, rewrite this buffer.
JMP getsvgamodinfo

getsvgamodfailed:
MOV SI,Floppyerror
CALL printstr
CALL newline

setvbefailed:
JMP die

die:
HLT         ; let the CPU "rest in peace"
JMP die

getsvgamodfinished:
CMP WORD [BestSVGAModeNo],0  ;if this value is 0,
JZ getsvgamodfailed          ;that means no svga mode matches. 

;Rewrite the buffer 0x8200
MOV CX,WORD [BestSVGAModeNo]
MOV EDI,SVGAModInfoBufAddr
MOV AX,4F01H
INT 10H
;there's no need to check again

MOV SI,FloppyOK
CALL printstr
CALL newline

;set the SVGA mode(VESA VBE)

MOV AX,4F02h
MOV BX,WORD [BestSVGAModeNo]	; BX -> VESA/SVGA Video Mode number (may also be std modes 00-13H)
                ;(bit 15 set = don't clear video memory on mode set)
				;set the selected video mode
ADD BX,4000H
INT 10H
CMP AX,004Fh  ;AH=0 means successful (else failed)
              ;AL=4fH means function supported
JNZ getsvgamodfailed

;enter protect mode
CLI
LGDT [GdtPtr]
;LIDT [IDT_POINTER]

;open the switch of protect mode
MOV EAX,CR0
OR EAX,1
MOV CR0,EAX

;jump to protect mode
MOV SI,Jumptoprmodemsg
CALL printstr
CALL newline
JMP DWORD SelectorCode32:gototmpprot

[SECTION .s32]
[BITS 32]

gototmpprot:
;go to tmp long mode
MOV AX,10H
MOV DS,AX
MOV ES,AX
MOV FS,AX
MOV SS,AX
MOV ESP,0x7E00

CALL longmodeissupported
TEST EAX,EAX
JZ notsupported

;init temporary page table 0x90000

MOV DWORD [0x90000],0x91007
MOV DWORD [0x90800],0x91007		
MOV DWORD [0x91000],0x92007
MOV DWORD [0x92000],0x000083
MOV DWORD [0x92008],0x200083
MOV DWORD [0x92010],0x400083
MOV DWORD [0x92018],0x600083
MOV DWORD [0x92020],0x800083
MOV DWORD [0x92028],0xa00083

;load GDT

LGDT [GdtPtr64]
MOV AX,10H
MOV DS,AX
MOV ES,AX
MOV FS,AX
MOV GS,AX
MOV SS,AX
MOV ESP,0x7E00

;open PAE
MOV EAX,CR4
BTS EAX,5
MOV CR4,EAX

;load cr3
MOV EAX,0x90000
MOV CR3,EAX

;enable long mode
MOV ECX,0C0000080h		;IA32_EFER
RDMSR
BTS EAX,8
WRMSR

;open PE and paging
MOV EAX,CR0
BTS EAX,0
BTS EAX,31
MOV CR0,EAX

;enter long mode and jump to kernel
JMP SelectorCode64:OffsetOfKernel


longmodeissupported:
MOV EAX,0x80000000
CPUID
CMP EAX,0x80000001
SETNB AL
JB done
MOV EAX,0x80000001
CPUID
BT EDX,29
SETC AL
done:
MOVZX EAX,AL
RET

notsupported:
HLT
JMP notsupported

[SECTION .s116]
[BITS 16]
;==========functions===============
;Function name: numtoascii
;usage: turn number to ascii code
;input value: cl - input number
;return value: al - the second digits ascii code of the number
;              ah - the first digits ascii code of the number
numtoascii:     
     mov ax,0
     mov al,cl  
     mov bl,10
     div bl
     add ax,3030h
     ret
 
;Function name: displayal
;usage: print the value in al
;input value: al - input number
;return value: no
displayal:
	push	ecx
	push	edx
	push	edi
	
	mov	edi,	[DisplayPosition]
	mov	ah,	0Fh
	mov	dl,	al
	shr	al,	4
	mov	ecx,	2
.begin:

	and	al,	0Fh
	cmp	al,	9
	ja	.1
	add	al,	'0'
	jmp	.2
.1:

	sub	al,	0Ah
	add	al,	'A'
.2:

	mov	[gs:edi],	ax
	add	edi,	2
	
	mov	al,	dl
	loop	.begin

	mov	[DisplayPosition],	edi

	pop	edi
	pop	edx
	pop	ecx
	
	ret

;Function name: readinfo
;usage: print where it reads
;input value: no
;return value: no
readinfo:       
     mov si,cylind
     call  printstr
     mov si,header
     call  printstr
     mov si,sector
     call  printstr
     ret
 

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
;input value: ah - data cluster in fat
;return value: no
getfatentry:
 
	push	es
	push	bx
	push	ax
	mov	ax,	0   ;use absolute address
	mov	es,	ax
	pop	ax
;1 2 3 4 5 6 7 8  1 2 3 4|5 6 7 8  1 2 3 4 5 6 7 8
;(    first FAT data    )|(    second FAT data   )
;test if the fat data is in the lasr 12 bit
	mov	byte	[Odd],	0
	mov	bx,	3
	mul	bx
	mov	bx,	2
	div	bx
	cmp	dx,	0
	jz even
;if not, it is in the first 12 bit
	mov	byte	[Odd],	1
 
even:
 
	xor	dx,	dx
	mov	bx,	[BytesPerSector]
	div	bx
	push	dx
	mov	bx,	BaseOfFATBuf
	add	ax,	FAT1StartSector
	mov	cx,	2
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


;IDT tmp

IDT:
	times 0x50 dq 0
IDT_END:

IDT_POINTER:
	dw IDT_END - IDT - 1
	dd IDT

;some necessary string
Welcomemsg db 'Loaded successfully!',0dh,0ah
           db 'Welcome to EOS!',0dh,0ah,'$'
       
nok DB '.$'
KernelFileName:		DB	"KERNEL  BIN",0
nokernelbin DB 'no kernel!','$'
Floppyerror DB 'ERR','$'
FloppyOK DB 'OK','$'
KernelOK DB 'Kernel moved!','$'
GetMemStrMsg DB 'Getting memory struct...','$'
Getsvgamsg DB 'Getting SVGA information...','$'
Getsvgamodmsg DB 'Getting SVGA mode info...','$'
Jumptoprmodemsg DB 'Jumping to Protect mode','$'
;tmp variable
RootDirSizeForLoop	dw	RootDirSectors
SectorNo		dw	0
Odd			db	0

OffsetOfKernelFileCount dd OffsetOfKernel

MemStructNumber DD 0
SVGAModeCounter DD 0
BestSVGAModeNo  DW 0
DisplayPosition DD 0


cylind  db 'cyl:?? $',0    
header  db 'hds:?? $',0    
sector  db 'sct:?? $',1    
