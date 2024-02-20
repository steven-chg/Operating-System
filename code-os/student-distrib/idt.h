#ifndef IDT_H
#define IDT_H

#include "x86_desc.h"
#include "lib.h"
#include "lib.h"
#include "exception_handler.h"
#include "asm_linkage.h"
#include "asm_linkage.h"
// #include "system_call.h"
// #include "keyboard.h"
// #include "rtc.h"

/*
 * idt_init()
 *  DESCRIPTION: Function to iniitialize and populate the IDT with entries 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Initializes the IDT and populates it with entries for exceptions vectors 0-19 and for system calls,
 *               keyboard interrupt, and rtc interrup.
 */
void idt_init();

#endif
