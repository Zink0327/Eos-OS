/*----------EOS interrupt handler for x86_64----------
    File name:int.h

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

#ifndef __INT_HANDLER_FUUVT
#define __INT_HANDLER_FUUVT

#include "linkage.h" 
#include "print.h" 
#include "lib_k.h" 

/* defined for int.asm */
void DE_fault(); 
void DB_fault(); 
void nmi_int(); 
void BP_trap(); 
void OF_trap(); 
void BR_fault(); 
void UD_fault(); 
void NM_fault(); 
void DF_abort(); 
void cso_fault(); 
void TS_fault(); 
void NP_fault(); 
void SS_fault(); 
void GP_fault(); 
void PF_fault(); 
void MF_fault(); 
void AC_fault(); 
void MC_abort(); 
void XM_fault(); 
void VE_fault();

void int_vector_init();

#endif