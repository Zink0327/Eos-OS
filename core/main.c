/*----------EOS core entry----------
    File name:main.c

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

#include "main.h"

void SystemCollapsed()
{
    // 
    uint32_t i = 0, j = (backupscrn.x_res - 17 - (backupscrn.x_res - 17) % 2) / 2;
    for(i = 0;i < j;i++)
        print(ORANGE,BLACK,"-");
    print(ORANGE,BLACK,"System Collapsed!");
    for(;i < backupscrn.x_res;i++)
        print(ORANGE,BLACK,"-");
    while(1);
}




int Start_Kernel(kernelconfig *config)
{
    kernelconfig osconf;

    //  init everything if necessary
    init(&osconf);  //defined at ./global.h
    print_init(backupscrn,osconf)

    long i=0xffff800000001000;

    print(WHITE,BLACK,"Hello world\nabc %#018lx \n",i);

    gmd.code_start = (addrtype)& _text;
    gmd.code_end   = (addrtype)& _etext;
    gmd.data_end   = (addrtype)& _edata;
    gmd.eoc        = (addrtype)& _end;

    init2(&osconf);
    init_gmd();

    /* initialize bitmap */
    gmd.block_bitmap = (uint64_t *)mem_1k_align(gmd.eoc);
    memblk_get_length(osconf.memory);
    gmd.bitmap_len_byte = mem_8byte_align(gmd.bitmap_length);

    gmd.heap = (addrtype *)mem_1k_align(gmd.block_bitmap + gmd.bitmap_len_byte);

    memset(gmd.block_bitmap,0xff,gmd.bitmap_len_byte);		/* clear all the bitmap first */ 

    /* initialize memblk */
    gmd.blocks = (memblk *)gmd.heap;
    gmd.blkstructsize = (gmd.bitmap_length * sizeof(memblk) + sizeof(uint64_t) - 1) & ( ~ (sizeof(uint64_t) - 1));

    gmd.heap += (addrtype)gmd.blkstructsize;

    memset(gmd.blocks,0x00,gmd.blkstructsize); /* clear block struct first */

    /* super init then... */
    memblk_init(osconf.memory);
    for(uint32_t i = 0;i < gmd.bitmap_length;i++)
    {
        gmd.blocks[i].fragments = (memfragment *)gmd.heap;
        uint64_t tmp = (sizeof(memfragment) + sizeof(uint64_t) - 1) & ( ~ (sizeof(uint64_t) - 1));
        gmd.heap += tmp;
        memset(gmd.blocks[i].fragments,0x00,tmp);
        gmd.blocks[i].fragments->presents = 0;
        gmd.blocks[i].fragments->parent = gmd.blocks;
        
    }

    print(WHITE,BLACK,"memblk bitmap:%*lb\n",gmd.bitmap_length,*gmd.block_bitmap);
    print(WHITE,BLACK,"heap top address: %#018lx\n",gmd.heap);
    //  load and start the "real" kernel
    

    //  receive interrupt from EIH
    while(1)
        ;
    
    //  it isn't supposed to be here
    SystemCollapsed();
}
