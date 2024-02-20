#include "paging.h"
#include "terminal.h"         

#define KERNAL_ADDRESS 0x400000 // 4MB
#define VIDEO_MEM 0xb8000 //The text screen video memory for colour monitors resides at 0xB8000 (according to OS dev)



/*
 * paging_init()
 *  DESCRIPTION: Function to initialize paging by setting the page directory and table entries
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: set up the paging for the OS
 */
void paging_init(){
    uint32_t i;

    for(i=0;i<NUM_MAX;i++){     // loop through each directory and table entries
        if(i==0){
            // as each table has 4096 page frames, we get table address by table entries/4096
            pde[i].kb.bit_addr_31_12           = ((int)pte >> PAGE_FRAME_BITS);
            
            pde[i].kb.avl_11_8                 = 0;
            // reserved bit
            pde[i].kb.page_size                = 0;
            // 4kb size for directory entry
            pde[i].kb.avl_6                    = 0;
            // not read yet
            pde[i].kb.accessed                 = 0;
            // page will not be cached
            pde[i].kb.page_cache_disable       = 0;
            // write-through caching not enabled
            pde[i].kb.page_write_through       = 0;
            // supervisor only
            pde[i].kb.user_supervisor          = 0; //1
            // read and write enabled
            pde[i].kb.read_write               = 1; //1
            // pde[i].kb in physical memory
            pde[i].kb.present                  = 1;
        }
        else if(i==1){
            // as each table has 4096 page frames, we get table address by table entries/4096
            pde[i].kb.bit_addr_31_12           = ((int)KERNAL_ADDRESS >> PAGE_FRAME_BITS);
            
            pde[i].kb.avl_11_8                 = 0;
            // reserved bit
            pde[i].kb.page_size                = 1;
            // 4kb size for directory entry
            pde[i].kb.avl_6                    = 0;
            // not read yet
            pde[i].kb.accessed                 = 0;
            // page will not be cached
            pde[i].kb.page_cache_disable       = 0;
            // write-through caching not enabled
            pde[i].kb.page_write_through       = 0;
            // supervisor only
            pde[i].kb.user_supervisor          = 0; //1
            // read and write enabled
            pde[i].kb.read_write               = 1; //1
            // pde[i].kb in physical memory
            pde[i].kb.present                  = 1;
        }
        else if(i== 32){
            // as each table has 4096 page frames, we get table address by table entries/4096
            pde[i].kb.bit_addr_31_12           = ((int)0x8000000 >> PAGE_FRAME_BITS);
            
            pde[i].kb.avl_11_8                 = 0;
            // reserved bit
            pde[i].kb.page_size                = 1;
            // 4kb size for directory entry
            pde[i].kb.avl_6                    = 0;
            // not read yet
            pde[i].kb.accessed                 = 0;
            // page will not be cached
            pde[i].kb.page_cache_disable       = 0;
            // write-through caching not enabled
            pde[i].kb.page_write_through       = 0;
            // supervisor only
            pde[i].kb.user_supervisor          = 1;
            // read and write enabled
            pde[i].kb.read_write               = 1;
            // pde[i].kb in physical memory
            pde[i].kb.present                  = 1;
        }
        else{
            // as each table has 4096 page frames, we get table address by table entries/4096
            pde[i].kb.bit_addr_31_12           = 0;
            
            pde[i].kb.avl_11_8                 = 0;
            // reserved bit
            pde[i].kb.page_size                = 1;
            // 4kb size for directory entry
            pde[i].kb.avl_6                    = 0;
            // not read yet
            pde[i].kb.accessed                 = 0;
            // page will not be cached
            pde[i].kb.page_cache_disable       = 0;
            // write-through caching not enabled
            pde[i].kb.page_write_through       = 0;
            // supervisor only
            pde[i].kb.user_supervisor          = 0;
            // read and write enabled
            pde[i].kb.read_write               = 1;
            // pde[i].kb in physical memory
            pde[i].kb.present                  = 0;
        }
        pte[i].bit_addr_31_12           = i;
        pte[i].avl_11_9                 = 0;
        pte[i].global                   = 0;
        pte[i].page_attribute_table     = 0;
        pte[i].dirty                    = 0;
        pte[i].accessed                 = 0;
        pte[i].page_cache_disable       = 0;
        pte[i].page_write_through       = 0;
        pte[i].user_supervisor          = 0;      // originally 0
        pte[i].read_write               = 1;      //1
        if(i==(VIDEO_MEM >> PAGE_FRAME_BITS)){    // only use 0xb8 table for physical memory
            pte[i].present=1;
            pte[i].global =1;
        }
        else{
            pte[i].present=0;
        }
    }

    pte[TERMINAL1_ADDR >> PAGE_FRAME_BITS].present = 1;
    pte[TERMINAL1_ADDR >> PAGE_FRAME_BITS].user_supervisor = 0;
    pte[TERMINAL1_ADDR >> PAGE_FRAME_BITS].accessed = 1;
    pte[TERMINAL1_ADDR >> PAGE_FRAME_BITS].bit_addr_31_12 = (TERMINAL_VID_BUF_START + (0*FOUR_KB)) >> PAGE_FRAME_BITS;

    pte[TERMINAL2_ADDR >> PAGE_FRAME_BITS].present = 1;
    pte[TERMINAL2_ADDR >> PAGE_FRAME_BITS].user_supervisor = 0;
    pte[TERMINAL2_ADDR >> PAGE_FRAME_BITS].accessed = 1;
    pte[TERMINAL2_ADDR >> PAGE_FRAME_BITS].bit_addr_31_12 = (TERMINAL_VID_BUF_START + (1*FOUR_KB)) >> PAGE_FRAME_BITS;
    
    pte[TERMINAL3_ADDR >> PAGE_FRAME_BITS].present = 1;
    pte[TERMINAL3_ADDR >> PAGE_FRAME_BITS].user_supervisor = 0;
    pte[TERMINAL3_ADDR >> PAGE_FRAME_BITS].accessed = 1;
    pte[TERMINAL3_ADDR >> PAGE_FRAME_BITS].bit_addr_31_12 = (TERMINAL_VID_BUF_START + (2*FOUR_KB)) >> PAGE_FRAME_BITS;

    pte[0xBC000 >> PAGE_FRAME_BITS].present = 1;
    pte[0xBC000 >> PAGE_FRAME_BITS].user_supervisor = 0;
    pte[0xBC000 >> PAGE_FRAME_BITS].accessed = 1;
    pte[0xBC000 >> PAGE_FRAME_BITS].bit_addr_31_12 = VIDEO_MEM >> PAGE_FRAME_BITS;

    asm volatile(
        "movl $pde, %%eax           ;"      // store the page directory entry into eax
        "andl $0xFFFFFC00, %%eax    ;"      // clear the last 10 bits
        "movl %%eax, %%cr3          ;"      // store the value in control register 3 (cr3) to point to the start of address

        "movl %%cr4, %%eax          ;"      // store cr4 to eax
        "orl $0x00000010, %%eax     ;"      // set the 5th bit to 1 to enable 4mb
        "movl %%eax, %%cr4          ;"      // store the value back to cr4

        "movl %%cr0, %%eax          ;"      // store cr0 to eax
        "orl $0x80000000, %%eax     ;"      // set the cr0's msb to 1 to enable paging
        "movl %%eax, %%cr0          ;"      // store the value back to cr0

        : : : "eax", "cc", "memory" );
}
