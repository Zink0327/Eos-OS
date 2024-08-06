/*----------EOS interrupt handler for x86_64----------
    File name:int.c

    Copyright (C) 2024 by Zink
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
#include "int.h"
#include "idt.h"
extern inline void set_int_gate(uint32_t n, uint8_t prvlev, void *addr);
extern inline void set_trap_gate(uint32_t n, uint8_t prvlev, void *addr);
extern inline void set_sys_int_gate(uint32_t n, uint8_t prvlev, void *addr);
extern inline void set_sys_trap_gate(uint32_t n, uint8_t prvlev, void *addr);

void diverror_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Divide Error (#DE) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void debug_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT/TRAP ] Debug Exception (#DB) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);
    
    while(1)
        io_hlt()
}

void nmi_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[INTERRUPT] NMI Interrupt with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);
    
    while(1)
        io_hlt()
}

void int3_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Breakpoint (#BP) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void overflow_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Overflow (#OF) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void bound_range_exceeded_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Bound range exceeded (#BR) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void invalid_opcode_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Invalid Opcode (#UD) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void device_unavailable_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Device Unavailable (#NM) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void double_fault_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[ABORT ] Double Fault (#DF) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void coprocessor_segment_overrun_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Coprocessor Segment Overrun with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}



void invalid_TSS_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Invalid TSS (#TS) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);
    do
    {
        if(errcode & 0x01)
        {
            print(YELLOW, BLACK, "Caused by an event external to the currently running program, such as interrupt handler or earlier expectation, ");
        }    
        if(errcode & 0x02)
        {
            print(YELLOW, BLACK, "Refers to a gate descriptor in IDT ");
            break;
        }
        if(errcode & 0x04)
        {
            print(YELLOW, BLACK, "Refers to a segment/gate descriptor in LDT ");
            break;
        }
        else
        {
            print(YELLOW, BLACK, "Refers to a descriptor in current GDT ");
            break;
        }
    } while (0);
    
    print(YELLOW, BLACK, "No.: %#010d \n", (errcode >> 3) & 0xfff);
    
    while(1)
        io_hlt()
}

void segment_not_present_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Segment not Present (#NP) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);
    do
    {
        if(errcode & 0x01)
        {
            print(YELLOW, BLACK, "Caused by an event external to the currently running program, such as interrupt handler or earlier expectation, ");
        }    
        if(errcode & 0x02)
        {
            print(YELLOW, BLACK, "Refers to a gate descriptor in IDT ");
            break;
        }
        if(errcode & 0x04)
        {
            print(YELLOW, BLACK, "Refers to a segment/gate descriptor in LDT ");
            break;
        }
        else
        {
            print(YELLOW, BLACK, "Refers to a descriptor in current GDT ");
            break;
        }
    } while (0);
    
    print(YELLOW, BLACK, "No.: %#010d \n", (errcode >> 3) & 0xfff);
    while(1)
        io_hlt()
}
void stack_segment_fault_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Stack Segment Fault (#SS) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);
    do
    {
        if(errcode & 0x01)
        {
            print(YELLOW, BLACK, "Caused by an event external to the currently running program, such as interrupt handler or earlier expectation, ");
        }    
        if(errcode & 0x02)
        {
            print(YELLOW, BLACK, "Refers to a gate descriptor in IDT, ");
            break;
        }
        if(errcode & 0x04)
        {
            print(YELLOW, BLACK, "Refers to a segment/gate descriptor in LDT, ");
            break;
        }
        else
        {
            print(YELLOW, BLACK, "Refers to a descriptor in current GDT, ");
            break;
        }
    } while (0);
    
    print(YELLOW, BLACK, "No.: %#010d \n", (errcode >> 3) & 0xfff);
    while(1)
        io_hlt()
}
void general_protection_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] General Protection (#GP) with errcode(binary):%#064lb, RIP:%#018x and RSP:%#018x \n", errcode, *p, RSP);
    do
    {
        if(errcode & 0x01)
        {
            print(YELLOW, BLACK, "Caused by an event external to the currently running program, such as interrupt handler or earlier expectation, ");
        }    
        if(errcode & 0x02)
        {
            print(YELLOW, BLACK, "Refers to a gate descriptor in IDT, ");
            break;
        }
        if(errcode & 0x04)
        {
            print(YELLOW, BLACK, "Refers to a segment/gate descriptor in LDT, ");
            break;
        }
        else
        {
            print(YELLOW, BLACK, "Refers to a descriptor in current GDT, ");
            break;
        }
    } while (0);
    
    print(YELLOW, BLACK, "No.: %#010d \n", (errcode >> 3) & 0xfff);
    while(1)
        io_hlt()
}

void page_fault_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL, CR2 = 0;

    __asm__ __volatile__ ("movq %%cr2,%0":"=r"(CR2)::"memory");

    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Page Fault (#PF) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);


    if (!(errcode & 0x01))
    {
        print(YELLOW, BLACK, "Page does not exist!\n");
    }
    if(errcode & 0x02)
    {
        print(YELLOW, BLACK, "Writing page caused the fault\n");
    }
    else
    {
        print(YELLOW, BLACK, "Reading page caused the fault\n");
    }
    if (errcode & 0x04)
    {
        print(YELLOW, BLACK, "Page visited by user(3)\n");
    }
    else
    {
        print(YELLOW, BLACK, "Page visited by supervisor(0,1,2)\n");
    }
    if(errcode & 0x08)
    {
        print(YELLOW, BLACK, "Did you set the reserved bit(s) of the page before?\n");
    }
    if(errcode & 0x10)
    {
        print(YELLOW, BLACK, "Fetching instruction caused the fault \n");
    }
    
    print(YELLOW, BLACK, "With CR2 value(linear address): %018lx\n", CR2);
    
    while(1)
        io_hlt()
}

void fpu_error_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] X87 FPU Error (#MF) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void alignment_check_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Alignment Check (#AC) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void machine_check_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[ABORT ] Machine Check (#MC) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void simd_floating_point_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] SIMD Floating Point Error (#XM) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}
void virtualization_handler(uint64_t RSP, uint64_t errcode)
{
    uint64_t *p = NULL;
    p = (uint64_t *)(RSP + 0x98);
    print(RED, BLACK, "[FAULT ] Virtualization Error (#VE) with errcode(binary):%#064lb, RIP:%#018lx and RSP:%#018lx \n", errcode, *p, RSP);

    while(1)
        io_hlt()
}

void int_vector_init()
{
	set_trap_gate(0,1,DE_fault);
 	set_trap_gate(1,1,DB_fault);
 	set_int_gate(2,1,nmi_int);
 	set_sys_trap_gate(3,1,BP_trap);
 	set_sys_trap_gate(4,1,OF_trap);
 	set_sys_trap_gate(5,1,BR_fault);
 	set_trap_gate(6,1,UD_fault);
 	set_trap_gate(7,1,NM_fault);
 	set_trap_gate(8,1,DF_abort);
 	set_trap_gate(9,1,cso_fault);
 	set_trap_gate(10,1,TS_fault);
 	set_trap_gate(11,1,NP_fault);
 	set_trap_gate(12,1,SS_fault);
 	set_trap_gate(13,1,GP_fault);
 	set_trap_gate(14,1,PF_fault);
 	//15 Intel reserved. ignore.
 	set_trap_gate(16,1,MF_fault);
 	set_trap_gate(17,1,AC_fault);
 	set_trap_gate(18,1,MC_abort);
 	set_trap_gate(19,1,XM_fault);
 	set_trap_gate(20,1,VE_fault);
 	//set_sys_trap_gate(SYSTEM_CALL_VECTOR,7,syscall); 

}