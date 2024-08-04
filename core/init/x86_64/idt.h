/*----------EOS IDT Descriptor Definition for x86_64----------
    File name:idt.h

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
#ifndef _EOS_IDT_DSCP
#define _EOS_IDT_DSCP

#define IDT_DPL_LEV_0 0x00     /* 00 */
#define IDT_DPL_LEV_1 0x01     /* 01 */
#define IDT_DPL_LEV_2 0x02     /* 10 */
#define IDT_DPL_LEV_3 0x03     /* 11 */

#define IDT_TASK_GATE_DSCRP 0x05      /* 101 , unused */
#define IDT_INTERRUPT_GATE_DSCRP 0x06 /* 110 */
#define IDT_TRAP_GATE_DSCRP 0x07      /* 111 */

#define IDT_32B_GATE 1
#define IDT_16B_GATE 0


#define IDT_KERNEL_INT_ATTR ((1 << 7) + (IDT_DPL_LEV_0 << 5) + (IDT_32B_GATE << 3) + IDT_INTERRUPT_GATE_DSCRP) /* 10001110 */
#define IDT_KERNEL_TRAP_ATTR ((1 << 7) + (IDT_DPL_LEV_0 << 5) + (IDT_32B_GATE << 3) + IDT_TRAP_GATE_DSCRP)     /* 10001111 */
#define IDT_SYSTEM_INT_ATTR ((1 << 7) + (IDT_DPL_LEV_3 << 5) + (IDT_32B_GATE << 3) + IDT_INTERRUPT_GATE_DSCRP)   /* 11101110 */
#define IDT_SYSTEM_TRAP_ATTR ((1 << 7) + (IDT_DPL_LEV_3 << 5) + (IDT_32B_GATE << 3) + IDT_TRAP_GATE_DSCRP)       /* 11101111 */

struct GDT_struct
{
    uint8_t item[8];
};

struct IDT_struct
{
    uint8_t item[16];
};

extern struct GDT_struct GDT_Table[];
extern struct IDT_struct IDT_Table[];
extern uint32_t TSS64_Table[26];


/*
gateaddr    - the address of IDT
attr        - the attribute defined above
ist         - Descriptor Privilege Level
handleraddr - the address of the procedure entry point
*/
#define _set_ivt_gate(gateaddr,attr,ist,handleraddr)                  \
do                                                                    \
{                                                                     \
    uint64_t __d0 , __d1;                                             \
    __asm__ __volatile__  (    "movw %%dx, %%ax           \n\t"       \
                               "andq $0x7, %%rcx          \n\t"       \
                               "addq %[attrb],%%rcx       \n\t"       \
                               "shlq $32, %%rcx           \n\t"       \
                               "addq %%rcx, %%rax         \n\t"  /* add attributes to bit 32 - 47 of RAX */    \
                               "xorq %%rcx, %%rcx         \n\t"       \
                               "movl %%edx, %%ecx         \n\t"  /* pass interrupt handler address to ECX */   \
                               "shrq $16, %%rcx           \n\t"  /* throw away the low-16 bit of the address */\
                               "shlq $48, %%rcx           \n\t"  /* move the high-16 bit to bit 48-63 of RCX */\
                               "addq %%rcx, %%rax         \n\t"  /* add high-16 bit of the handler's address to RAX */\
                               "movq %%rax, %[high32bit] \n\t"       \
                               "shrq $32, %%rdx           \n\t"      \
                               "movq %%rdx, %[low32bit]  \n\t"       \
    : [high32bit] "=m"(*((uint64_t *)(gateaddr))) ,                  \
      [low32bit] "=m"(*(1 + (uint64_t *)(gateaddr))) ,               \
      "=&a"(__d0) ,             /* "Backup" of (0x8 << 16) */        \
      "=&d"(__d1)    /* "Backup" of (((uint32_t *)(handleraddr))) */ \
    : "c"(ist) ,                                                     \
      [attrb] "i"(attr << 8) ,                                       \
      "2"(0x8 << 16) ,/* it is supposed to be loaded to RAX */       \
      "3"(((uint32_t *)(handleraddr)))      /* ...and RDX */         \
    : "memory"                                                       \
    );                                                               \
} while (0)

/*
     WHICH PERVENT INVENTED THIS UNGODLY EXTENDED ASS...ASSEMBLY GRAMMAR??!!
     WHY CAN'T JUST USE THE PRESENTED VARIABLES???!!!
               \  |    |
    (_/#`=')_/   --------

     After all, for the people who tried to write OS in extended assembly, 
     we don't need to mention the already-dead, if you are still alive 
     after typing extended assembly, live bravely, because you're under 
     the God's protection.
*/

#define io_ltr(A)                                           \
do                                                          \
{                                                           \
    __asm__ __volatile__ ("ltr %%ax"::"a"(A<<3):"memory");  \
}while(0)

#define io_hlt()  __asm__ __volatile__ ("hlt":::"memory");

inline void set_int_gate(uint32_t n, uint8_t prvlev, void *addr)
{
    _set_ivt_gate(IDT_Table + n, IDT_KERNEL_INT_ATTR, prvlev, addr);
}

inline void set_trap_gate(uint32_t n, uint8_t prvlev, void *addr)
{
    _set_ivt_gate(IDT_Table + n, IDT_KERNEL_TRAP_ATTR, prvlev, addr);
}

inline void set_sys_int_gate(uint32_t n, uint8_t prvlev, void *addr)
{
    _set_ivt_gate(IDT_Table + n, IDT_SYSTEM_INT_ATTR, prvlev, addr);
}

inline void set_sys_trap_gate(uint32_t n, uint8_t prvlev, void *addr)
{
    _set_ivt_gate(IDT_Table + n, IDT_SYSTEM_TRAP_ATTR, prvlev, addr);
}

void set_tss64(
    uint64_t rsp0,
    uint64_t rsp1,
    uint64_t rsp2,
    uint64_t ist1,
    uint64_t ist2,
    uint64_t ist3, 
    uint64_t ist4,
    uint64_t ist5,
    uint64_t ist6,
    uint64_t ist7
) 
{
 	*(uint64_t *)(TSS64_Table+1) = rsp0;
 	*(uint64_t *)(TSS64_Table+3) = rsp1;
 	*(uint64_t *)(TSS64_Table+5) = rsp2;
 	*(uint64_t *)(TSS64_Table+9) = ist1;
 	*(uint64_t *)(TSS64_Table+11) = ist2;
 	*(uint64_t *)(TSS64_Table+13) = ist3;
 	*(uint64_t *)(TSS64_Table+15) = ist4;
 	*(uint64_t *)(TSS64_Table+17) = ist5;
 	*(uint64_t *)(TSS64_Table+19) = ist6;
 	*(uint64_t *)(TSS64_Table+21) = ist7;	 
}

#endif