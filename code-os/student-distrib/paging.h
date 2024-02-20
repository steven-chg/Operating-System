#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"
#define NUM_MAX 1024    // each page directory has 1024 tables, each table has 1024 frames
#define B_IN_4KB 4096  // 4kB = 4096 B
#define PAGE_FRAME_BITS 12 // 2^12 = 4096
#define USERSPACE_ADDR  0x8000000   // 128MB
#define MB_4  0x400000   // 4MB

#define TERMINAL1_ADDR  0xB9000
#define TERMINAL2_ADDR  0xBA000
#define TERMINAL3_ADDR  0xBB000

typedef struct __attribute__((packed)) page_directory_entry_4kb{
    // 1 if the page is in physical memory
    uint32_t present                : 1;
    // 1 is the page is read/write, otherwise read-only
    uint32_t read_write             : 1;
    // 1 if page may be accessed by all, otherwise only the supervisor
    uint32_t user_supervisor        : 1;
    // 1 if write-through caching is enabled
    uint32_t page_write_through     : 1;
    // 1 if page will not be cached, otherwise, 0
    uint32_t page_cache_disable     : 1;
    // discover whether a PDE or PTE was read
    uint32_t accessed               : 1;
    uint32_t avl_6                  : 1;
    uint32_t page_size              : 1;  
    // available bit
    uint32_t  avl_11_8               : 4;
    // 20 bits of table address
    uint32_t bit_addr_31_12         : 20;
    
    
}page_directory_entry_4kb;

typedef struct __attribute__((packed)) page_table_entry{
    // 1 if the page is in physical memory
    uint32_t present                : 1;
    // 1 is the page is read/write, otherwise read-only
    uint32_t read_write             : 1;
    // 1 if page may be accessed by all, otherwise only the supervisor
    uint32_t user_supervisor        : 1;
    // 1 if write-through caching is enabled
    uint32_t page_write_through     : 1;
    // 1 if page will not be cached, otherwise, 0
    uint32_t page_cache_disable     : 1;
    // discover whether a PDE or PTE was read  
    uint32_t accessed               : 1;
    // ignored
    uint32_t dirty                  : 1; 
    // if PAT is supported
    uint32_t page_attribute_table   : 1;  
    // enable global pages
    uint32_t global                 : 1;  
    // available bits
    uint32_t  avl_11_9               : 3;  
    // 20 bits of table address
    uint32_t bit_addr_31_12         : 20;
}page_table_entry;

typedef struct __attribute__((packed)) page_directory_entry_4mb {
    // 1 if the page is in physical memory
    uint32_t present                : 1;
    // 1 is the page is read/write, otherwise read-only
    uint32_t read_write             : 1;
    // 1 if page may be accessed by all, otherwise only the supervisor
    uint32_t user_supervisor        : 1;
    // 1 if write-through caching is enabled
    uint32_t page_write_through     : 1;
    // 1 if page will not be cached, otherwise, 0
    uint32_t page_cache_disable     : 1;
    // discover whether a PDE or PTE was read
    uint32_t accessed               : 1;
    // determine whether a page has been written to  
    uint32_t dirty                  : 1; 
    uint32_t page_size              : 1;
    // enable global pages
    uint32_t global                 : 1;    
    // available bit
    uint32_t  avl_11_9               : 3;
    // if PAT is supported
    uint32_t page_attribute_table   : 1; 
    uint32_t  bit_addr_20_13         : 8;  
    uint32_t  reserved               : 1; 
    // 20 bits of table address
    uint32_t bit_addr_31_22         : 10;
    
    
} page_directory_entry_4mb;

typedef union page_directory_entry{     // group the page directory entry of 4mb and 4kb with union
    page_directory_entry_4mb mb;
    page_directory_entry_4kb kb;
}page_directory_entry;

page_directory_entry pde[NUM_MAX] __attribute__((aligned (B_IN_4KB)));  // initialize page directory entry
page_table_entry pte[NUM_MAX] __attribute__((aligned (B_IN_4KB)));  // initialize page table entry
page_table_entry user_pte[NUM_MAX] __attribute__((aligned (B_IN_4KB)));  // initialize page table entry

extern void paging_init();

#endif 
