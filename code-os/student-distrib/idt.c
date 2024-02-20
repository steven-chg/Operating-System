#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "exception_handler.h"
#include "keyboard.h"
#include "rtc.h"
#include "asm_linkage.h"

#include "systemcall.h" 

/*
 * idt_init()
 *  DESCRIPTION: Function to iniitialize and populate the IDT with entries 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Initializes the IDT and populates it with entries for exceptions vector 0-19, and for system calls,
 *               keyboard interrupt, and rtc interrup.
 */
void idt_init(){
    // Loop (0 to 255 for a total of 256 vectors in the IDT) and initialize IDT entries 
    int loop;
    for(loop = 0; loop < NUM_VEC; loop++){
        idt[loop].present = 0;                  // Present bit initialize to 0 (not valid interrupt)
        idt[loop].dpl = 0;                      // Descriptor privilege level: 0 for hardware interrupts and exceptions; 3 for system calls
        idt[loop].reserved0 = 0;                // Always set to 0
        idt[loop].size = 1;                     // Gate type value; set to 1 for 32-bit gate descriptor
        idt[loop].reserved1 = 1;                // Gate type value; set to 1
        idt[loop].reserved2 = 1;                // Gate type value; set to 1
        idt[loop].reserved3 = 1;                // Gate type value; 0 for interrupt (32-bit), 1 for exception (32-bit)
        idt[loop].reserved4 = 0;                // Set to 0 (reserved)
        idt[loop].seg_selector = KERNEL_CS;     // Segment selector set to kernel code segment
    }

    /* Set up exceptions & interrupts vector number 0-19 (0x00 through 0x13) */
    
    // Set handler for IDT entry and present bit to 1 (to signify a valid descriptor)
    SET_IDT_ENTRY(idt[0x00], divide_error_exception);
    idt[0x00].present = 1;
    SET_IDT_ENTRY(idt[0x01], debug_exception);
    idt[0x01].present = 1;
    SET_IDT_ENTRY(idt[0x02], nmi_interrupt);
    idt[0x02].present = 1;
    SET_IDT_ENTRY(idt[0x03], breakpoint_exception);
    idt[0x03].present = 1;
    SET_IDT_ENTRY(idt[0x04], overflow_exception);
    idt[0x04].present = 1;
    SET_IDT_ENTRY(idt[0x05], bound_range_exceed_exception);
    idt[0x05].present = 1;
    SET_IDT_ENTRY(idt[0x06], invalid_opcode_exception);
    idt[0x06].present = 1;
    SET_IDT_ENTRY(idt[0x07], device_not_available_exception);
    idt[0x07].present = 1;
    SET_IDT_ENTRY(idt[0x08], double_fault_exception);
    idt[0x08].present = 1;
    SET_IDT_ENTRY(idt[0x09], coprocessor_segment_overrun);
    idt[0x09].present = 1;
    SET_IDT_ENTRY(idt[0x0A], invalid_TSS_exception);
    idt[0x0A].present = 1;
    SET_IDT_ENTRY(idt[0x0B], segment_not_present);
    idt[0x0B].present = 1;
    SET_IDT_ENTRY(idt[0x0C], stack_fault_exception);
    idt[0x0C].present = 1;
    SET_IDT_ENTRY(idt[0x0D], general_protection_fault);
    idt[0x0D].present = 1;
    SET_IDT_ENTRY(idt[0x0E], page_fault_exception);
    idt[0x0E].present = 1;

    /* 0x0F/Vector No. 15 is Intel Reserved; Skip*/

    SET_IDT_ENTRY(idt[0x10], FPU_floating_point_error);
    idt[0x10].present = 1;
    SET_IDT_ENTRY(idt[0x11], alignment_check_exception);
    idt[0x11].present = 1;
    SET_IDT_ENTRY(idt[0x12], machine_check_exception);
    idt[0x12].present = 1;
    SET_IDT_ENTRY(idt[0x13], SIMD_floating_point_exception);
    idt[0x13].present = 1;

    /* Set up keyboard, real-time clock, and system calls handlers */

    // Keyboard setup (vector 0x21, IRQ1 on primary PIC for keyboard)
    SET_IDT_ENTRY(idt[0x21], keyboard_handler_asm);
    idt[0x21].present = 1;              // Set present bit to 1 (to signify valid descriptor)
    idt[0x21].reserved3 = 0;            // interrupt, so set to 0

    // RTC setup (vector 0x28, IRQ8 on secondary PIC for real time clock)
    SET_IDT_ENTRY(idt[0x28], rtc_handler_asm);
    idt[0x28].present = 1;              // Set present bit to 1 (to signify valid descriptor)
    idt[0x28].reserved3 = 0;            // interrupt, so set to 0

    // System Call setup (vector 0x80)
    SET_IDT_ENTRY(idt[0x80], system_call_handler_asm);
    idt[0x80].present = 1;              // Set present bit to 1 (to signify valid descriptor)
    idt[0x80].dpl = 3;                  // Set descriptor privilege level to 3 so system call handler is accessible from user space via int instruction

    // PIT setup (vector 0x20, IRQ0 on primary PIC for pit)
    SET_IDT_ENTRY(idt[0x20], pit_handler_asm);
    idt[0x20].present = 1;              // Set present bit to 1 (to signify valid descriptor)
    idt[0x20].reserved3 = 0;            // interrupt, so set to 0
}
