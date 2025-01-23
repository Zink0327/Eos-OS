;----------EOS asm library for x86_64----------
;    File name:asmfun.asm
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

[SECTION .text]
[BITS 64]

GLOBAL io_cli
io_cli:	; void io_cli();
		CLI
		RET

GLOBAL io_sti
io_sti:	; void io_sti();
		STI
		RET

GLOBAL io_mfence
io_mfence:	; void io_mfence();
		MFENCE
		RET

GLOBAL io_in8
io_in8:	; uint8_t io_in8(uint16_t port);
		MOV		DX,DI
		IN		AL,DX
		RET

GLOBAL io_in16
io_in16:	; uint16_t io_in16(uint16_t port);
		MOV		DX,DI
		IN		AX,DX
		RET

GLOBAL io_in32
io_in32:	; uint32_t io_in32(uint16_t port);
		MOV		DX,DI
		IN		EAX,DX
		RET

GLOBAL io_out8
io_out8:	; void io_out8(uint16_t port, uint8_t data);
		MOV		AX,SI
		AND		AX,0xF0
		MOV		DX,DI
		OUT		DX,AL
		RET

GLOBAL io_out16
io_out16:	; void io_out16(uint16_t port, uint16_t data);
		MOV		DX,DI
		MOV		AX,SI
		OUT		DX,AX
		RET

GLOBAL io_out32
io_out32:	; void io_out32(uint16_t port, uint32_t data);
		MOV		DX,DI
		MOV		EAX,ESI
		OUT		DX,EAX
		RET

GLOBAL memcpy
memcpy:
		PUSH   RBP
		MOV    RSP,RBP
		PUSH   RDX
		MOV    RAX,RDX
		LEA    RDX,[RAX + 7]
		TEST   RAX,RAX
		CMOVS  RAX,RDX
		MOV    RCX,RAX
		POP    RAX
		MOV    RDX,RDI
		CLD
		REP 
		MOVSB
		MOV    EDX,ESI
		MOV    DWORD [RBP - 0x4],ECX
		MOV    DWORD [RBP - 0x8],EDI
		MOV    DWORD [RBP - 0xc],EDX
		MOV    RAX, QWORD [RBP - 0x20]
		POP    RBP
		RET
	
GLOBAL memcmp
memcmp:
		PUSH	RBP
		MOV		RBP,RSP
		PUSH	RBX
		MOV		RCX,RDX
		MOV		EAX,0
		CLD
		REPE
		CMPSB
		JE		L2
		MOV		EAX,1
		JL		L2
		NEG		EAX
L2:
		POP		RBX
		POP		RBP
		RET

GLOBAL memset
memset:
		PUSH	RBP
		MOV		RBP,RSP
		MOV		AX,SI
		MOV 	QWORD [RBP - 0x18],RDI
		MOV 	QWORD [RBP - 0x28],RDX
		XOR		RDX,RDX
		MOV		DL,AL
		MOV		EAX,EDX
		MOV		EDX,0x10101010
		IMUL	EDX,EAX
		MOV		QWORD [RBP - 8],RAX
		MOV		RAX,QWORD [RBP - 0x28]
		LEA		RDX,[RAX + 7]
		TEST	RAX,RAX
		CMOVS	RAX,RDX
		MOV		RCX,RAX
		MOV		RAX,QWORD [RBP - 8]
		MOV		RSI,QWORD [RBP - 0x28]
		MOV 	RDI,QWORD [RBP - 0x18]
		CLD
		REP
		STOSB
		MOV		[RBP - 0x10],EDI
		MOV		[RBP - 0xC],ECX
		MOV		RAX,[RBP - 0x18]
		POP		RBP
		RET