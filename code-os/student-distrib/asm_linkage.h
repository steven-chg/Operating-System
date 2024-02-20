#ifndef ASM_LINKAGE_H
#define ASM_LINKAGE_H

#ifndef ASM
    extern void rtc_handler_asm();          // linkage for RTC handler
    extern void keyboard_handler_asm();     // linkage for keyboard handler
    extern void pit_handler_asm();     // linkage for keyboard handler
#endif

#endif
