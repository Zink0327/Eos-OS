/*----------EOS kernel header----------
    File name:head.S

    Copyright (C) 2023 by Zink
    This file is part of EOS

    EOS is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

.section .text

#define pg_present      1
#define pg_write        2
#define pg_supervisor   4
#define pg_writethrough 8
#define pg_cachedisable 16
#define pg_accessed     32
#define pg_dirty        64
#define pg_pat          128

#define pg_2M_mbo          0x80
#define pg_2M_pat          0x1000

#define pg_PDE_2m_attr (pg_2M_mbo + pg_write + pg_present)
#define pg_PML4E_attr (pg_write + pg_present)
#define pg_PML4E_attr_s (pg_supervisor + pg_write + pg_present)

#define PML4E_start_addr 0x101000

#define PDP(offset) (PML4E_start_addr + offset + pg_PML4E_attr)
#define PDP_s(offset) (PML4E_start_addr + offset + pg_PML4E_attr_s)
#define PDE_2M(x) ((x << 21) + pg_PDE_2m_attr)

.globl _start

_start:

	mov	$0x10,	%ax
	mov	%ax,	%ds
	mov	%ax,	%es
	mov	%ax,	%fs
	mov	%ax,	%ss
	mov	$0x7E00,	%esp

//load GDT

	lgdt	GDT_POINTER(%rip)

//load	IDT

	lidt	IDT_POINTER(%rip)

	mov	$0x10,	%ax
	mov	%ax,	%ds
	mov	%ax,	%es
	mov	%ax,	%fs
	mov	%ax,	%gs
	mov	%ax,	%ss

	movq	$0x7E00,	%rsp

// Locate PML4E by loading CR3

	movq	$0x101000,	%rax
	movq	%rax,		%cr3
	movq	switch_seg(%rip),	%rax
	pushq	$0x08
	pushq	%rax
	lretq

//64-bit mode code

switch_seg:
	.quad	entry64

entry64:
	movq	$0x10,	%rax
	movq	%rax,	%ds
	movq	%rax,	%es
	movq	%rax,	%gs
	movq	%rax,	%ss
	movq	$0xffff800000007C00,	%rsp		/* rsp address */


setup_IDT:							
	leaq	ignore_int(%rip),	%rdx        /* set up temporary interrupt handler (ignore_int) for all kinds of interrupt */
	movq	$(0x08 << 16),	%rax
	movw	%dx,	%ax
	movq	$(0x8E00 << 32),	%rcx		
	addq	%rcx,	%rax
	movl	%edx,	%ecx
	shrl	$16,	%ecx
	shlq	$48,	%rcx
	addq	%rcx,	%rax
	shrq	$32,	%rdx
	leaq	IDT_Table(%rip),	%rdi
	mov	$256,	%rcx
repeat_setidt:
	movq	%rax,	(%rdi)
	movq	%rdx,	8(%rdi)
	addq	$0x10,	%rdi
	dec	%rcx
	jne	repeat_setidt

setup_TSS64:
	leaq	TSS64_Table(%rip),	%rdx
	xorq	%rax,	%rax
	xorq	%rcx,	%rcx
	movq	$0x89,	%rax
	shlq	$40,	%rax
	movl	%edx,	%ecx
	shrl	$24,	%ecx
	shlq	$56,	%rcx
	addq	%rcx,	%rax
	xorq	%rcx,	%rcx
	movl	%edx,	%ecx
	andl	$0xffffff,	%ecx
	shlq	$16,	%rcx
	addq	%rcx,	%rax
	addq	$103,	%rax
	leaq	GDT_Table(%rip),	%rdi
	movq	%rax,	64(%rdi)
	shrq	$32,	%rdx
	movq	%rdx,	72(%rdi)
//useless now
//	mov	$0x40,	%ax
//	ltr	%ax

	movq	go_to_kernel(%rip),	%rax		/* movq address */
	pushq	$0x08                           /* JUMP TO */
	pushq	%rax                            /* Start_Kernel */
	lretq

// So long, AT-T assembly!
go_to_kernel:
	.quad	Start_Kernel

//ignore interrupt

ignore_int:
	cld
	pushq	%rax
	pushq	%rbx
	pushq	%rcx
	pushq	%rdx
	pushq	%rbp
	pushq	%rdi
	pushq	%rsi

	pushq	%r8
	pushq	%r9
	pushq	%r10
	pushq	%r11
	pushq	%r12
	pushq	%r13
	pushq	%r14
	pushq	%r15

	movq	%es,	%rax
	pushq	%rax
	movq	%ds,	%rax
	pushq	%rax

	movq	$0x10,	%rax
	movq	%rax,	%ds
	movq	%rax,	%es

	leaq	int_msg(%rip),	%rax			/* leaq: get address */
	pushq	%rax
	movq	%rax,	%rdx
	movq	$0x00ff00,	%rsi
	movq	$0x8000ff,	%rdi
	movq	$0,	%rax
	callq	print
	addq	$0x8,	%rsp

Loop:
	jmp	Loop	

	popq	%rax
	movq	%rax,	%ds
	popq	%rax
	movq	%rax,	%es

	popq	%r15
	popq	%r14
	popq	%r13
	popq	%r12
	popq	%r11
	popq	%r10
	popq	%r9
	popq	%r8

	popq	%rsi
	popq	%rdi
	popq	%rbp
	popq	%rdx
	popq	%rcx
	popq	%rbx
	popq	%rax
	iretq

int_msg:
	.asciz "RIP:Unknown interrupt or fault\n"
//init page
.align 8

.org	0x1000

__PML4E:
// 1 * 512GB
	.quad PDP(0x1000)   //0x0000000000000000
	.fill	255,8,0
	.quad PDP_s(0x1000) //0xffff800000000000
	.fill	255,8,0

.org	0x2000

__PDPTE:
// 4 * 1GB = 4GB
	.quad	PDP(0x2000)
	.quad	PDP(0x3000)
	.quad	PDP(0x4000)
	.quad	PDP(0x5000)
	.fill	508,8,0

.org	0x3000

__PDE:
// 2048 * 2MB = 4GB
	.set i,0
	.rept 2048
		.quad PDE_2M(i)
		.set i,i+1
	.endr

.section .data

.globl GDT_Table

GDT_Table:
	.quad	0x0000000000000000			/*0	NULL descriptor		       	00*/
	.quad	0x0020980000000000			/*1	KERNEL	Code	64-bit	Segment	08*/
	.quad	0x0000920000000000			/*2	KERNEL	Data	64-bit	Segment	10*/
	.quad	0x0020f80000000000			/*3	USER	Code	64-bit	Segment	18*/
	.quad	0x0000f20000000000			/*4	USER	Data	64-bit	Segment	20*/
	.quad	0x00cf9a000000ffff			/*5	KERNEL	Code	32-bit	Segment	28*/
	.quad	0x00cf92000000ffff			/*6	KERNEL	Data	32-bit	Segment	30*/
	.fill	10,8,0					/*8 ~ 9	TSS (jmp one segment <7>) in long-mode 128-bit 40 (unused) */
GDT_END:

GDT_POINTER:
GDT_LIMIT:	.word	GDT_END - GDT_Table - 1
GDT_BASE:	.quad	GDT_Table

//IDT Table

.globl IDT_Table

IDT_Table:
	.fill  512,8,0
IDT_END:

IDT_POINTER:
IDT_LIMIT:	.word	IDT_END - IDT_Table - 1
IDT_BASE:	.quad	IDT_Table

//TSS64 Table

.globl	TSS64_Table

TSS64_Table:
	.fill  13,8,0
TSS64_END:


TSS64_POINTER:
TSS64_LIMIT:	.word	TSS64_END - TSS64_Table - 1
TSS64_BASE:	.quad	TSS64_Table
