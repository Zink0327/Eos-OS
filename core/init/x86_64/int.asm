;----------EOS interrupt handler for x86_64----------
;    File name:int.asm
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


EXTERN diverror_handler
EXTERN debug_handler
EXTERN nmi_handler
EXTERN int3_handler
EXTERN overflow_handler
EXTERN bound_range_exceeded_handler
EXTERN invalid_opcode_handler
EXTERN device_unavailable_handler
EXTERN double_fault_handler
EXTERN coprocessor_segment_overrun_handler
EXTERN invalid_TSS_handler
EXTERN segment_not_present_handler
EXTERN stack_segment_fault_handler
EXTERN general_protection_handler
EXTERN page_fault_handler
EXTERN fpu_error_handler
EXTERN alignment_check_handler
EXTERN machine_check_handler
EXTERN simd_floating_point_handler
EXTERN virtualization_handler



[SECTION .text]
[BITS 64]

POS_R15 EQU 0x00
POS_R14 EQU 0x08
POS_R13 EQU 0x10
POS_R12 EQU 0x18
POS_R11 EQU 0x20
POS_R10 EQU 0x28
POS_R9 EQU 0x30
POS_R8 EQU 0x38
POS_RBX EQU 0x40
POS_RCX EQU 0x48
POS_RDX EQU 0x50
POS_RSI EQU 0x58
POS_RDI EQU 0x60
POS_RBP EQU 0x68
POS_DS EQU 0x70
POS_ES EQU 0x78
POS_RAX EQU 0x80
POS_FUNC EQU 0x88
POS_ERRCODE EQU 0x90
POS_RIP EQU 0x98
POS_CS EQU 0xa0
POS_RFLAGS EQU 0xa8
POS_OLDRSP EQU 0xb0
POS_OLDSS EQU 0xb8


RESTORE_ALL:
	POP R15
	POP R14
	POP R13
	POP R12
	POP R11
	POP R10
	POP R9
	POP R8
	POP RBX
	POP RCX
	POP	RDX
	POP	RSI
	POP	RDI
	POP	RBP
	POP RAX	
	MOV DS,RAX
	POP RAX
	MOV ES,RAX
	POP RAX
	ADD RSP,10
	IRET
	
%define GET_CURRENT(regs)  mov regs,-32768 \
	                         and	regs,RSP


ret_from_exception:
GLOBAL ret_from_intr     ;;;TMP
ret_from_intr:
	JMP	RESTORE_ALL

GLOBAL DE_fault
DE_fault:
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL diverror_handler]
	XCHG [RSP],RAX

error_code:
	PUSH RAX
	MOV RAX,ES
	PUSH RAX
	MOV RAX,DS
	PUSH RAX
	XOR RAX,RAX

	PUSH RBP
	PUSH RDI
	PUSH RSI
	PUSH RDX
	PUSH RCX
	PUSH RBX
	PUSH R8
	PUSH R9
	PUSH R10
	PUSH R11
	PUSH R12
	PUSH R13
	PUSH R14
	PUSH R15	
	
	CLD
	MOV	RSI,[RSP+POS_ERRCODE]
	MOV	RDX,[RSP+POS_FUNC]

	MOV	RDI,0X10
	MOV DS,RDI
	MOV ES,RDI

	MOV RDI,RSP
	;GET_CURRENT(EBX)
	CALL FAR QWORD RDX

	JMP	ret_from_exception	

GLOBAL DB_fault
DB_fault:
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  debug_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL nmi_int
nmi_int: 
	PUSH RAX
	CLD
	PUSH RAX

	PUSH RAX
	MOV RAX,ES
	PUSH RAX
	MOV RAX,DS
	PUSH RAX
	XOR RAX,RAX

	PUSH RBP
	PUSH RDI
	PUSH RSI
	PUSH RDX
	PUSH RCX
	PUSH RBX
	PUSH R8
	PUSH R9
	PUSH R10
	PUSH R11
	PUSH R12
	PUSH R13
	PUSH R14
	PUSH R15	
	
	MOV	RDX,0x10
	MOV DS,RDX
	MOV ES,RDX

	MOV RSI,0
	MOV RDI,RSP

	CALL nmi_handler

	JMP	RESTORE_ALL

GLOBAL BP_trap
BP_trap: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  int3_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL OF_trap
OF_trap:
 	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  overflow_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL BR_fault
BR_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  bound_range_exceeded_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL UD_fault
UD_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  invalid_opcode_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL NM_fault             ;;;TMP
NM_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  device_unavailable_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL DF_abort
DF_abort: 
	PUSH RAX
	LEA RAX,[REL  double_fault_handler]
	XCHG QWORD [RSP],RAX
	JMP RESTORE_ALL

GLOBAL cso_fault
cso_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  coprocessor_segment_overrun_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL TS_fault
TS_fault: 
	PUSH RAX
	LEA RAX,[REL  invalid_TSS_handler]
	XCHG QWORD [RSP],RAX
	JMP RESTORE_ALL

GLOBAL NP_fault
NP_fault: 
	PUSH RAX
	LEA RAX,[REL  segment_not_present_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL SS_fault
SS_fault: 
	PUSH RAX
	LEA RAX,[REL  stack_segment_fault_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL GP_fault
GP_fault: 
	PUSH RAX
	LEA RAX,[REL  general_protection_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL PF_fault
PF_fault: 
	PUSH RAX
	LEA RAX,[REL  page_fault_handler]
	XCHG QWORD [RSP],RAX
	JMP RESTORE_ALL

GLOBAL MF_fault
MF_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  fpu_error_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL AC_fault
AC_fault: 
	PUSH RAX
	LEA RAX,[REL  alignment_check_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL MC_abort
MC_abort: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  machine_check_handler]
	XCHG [RSP],RAX
	JMP error_code

GLOBAL XM_fault
XM_fault: 
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  simd_floating_point_handler]
	XCHG [RSP],RAX
	JMP error_code
GLOBAL VE_fault
VE_fault:
	PUSH QWORD 0
	PUSH RAX
	LEA RAX,[REL  virtualization_handler]
	XCHG [RSP],RAX
	JMP error_code