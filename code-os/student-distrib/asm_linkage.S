#define ASM     1
#include "asm_linkage.h"

#define linkage_asm(name, func) \
    .globl name                   ;\
    name:                         ;\
        pushal                    ;\
        pushfl                    ;\
        call func                 ;\
        popfl                     ;\
        popal                     ;\
        iret

linkage_asm(rtc_handler_asm, rtc_handler);              // linkage for rtc handler (store and restore all registers)
linkage_asm(keyboard_handler_asm, keyboard_handler);    // linkage for keyboard handler
linkage_asm(pit_handler_asm, pit_handler);              // linkage for pit handler