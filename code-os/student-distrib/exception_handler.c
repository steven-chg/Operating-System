#include "idt.h"
#include "x86_desc.h"
#include "lib.h"
#include "exception_handler.h"

/* File that includes all "handlers" for exceptions (prints information) */

// Array that stores exception vector numbers and name of exception in order based on table 5.1 of IA-32 reference manual
static char* exceptionNames[] = {
    "Vector No. 0: Divide Error Exception",
    "Vector No. 1: Debug Exception",
    "Vector No. 2: NMI Interrupt",
    "Vector No. 3: Breakpoint Exception",
    "Vector No. 4: Overflow Exception",
    "Vector No. 5: BOUND Range Exceeded Exception",
    "Vector No. 6: Invalid Opcode Exception",
    "Vector No. 7: Device Not Available Exception",
    "Vector No. 8: Double Fault Exception",
    "Vector No. 9: Coprocessor Segment Overrun",
    "Vector No. 10: Invalid TSS Exception",
    "Vector No. 11: Segment Not Present",
    "Vector No. 12: Stack Fault Exception",
    "Vector No. 13: General Protection Fault",
    "Vector No. 14: Page Fault Exception",
    "Vector No. 15: Reserved",                               // Intel Reserved
    "Vector No. 16: FPU Floating Point Error",
    "Vector No. 17: Alignment Check Exception",
    "Vector No. 18: Machine Check Exception",
    "Vector No. 19: SIMD Floating Point Exception",
    "Vector No. 20: Virtualization Fault",
    "Vector No. 21: System Call Hold"
};

/* 
 * divide_error_exception
 *   DESCRIPTION: Prints prompt that divide error exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints divide error exception to the screen and freeze by using while loop
 */
void divide_error_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x00]);
    while(1);
}

/* 
 * debug_exception
 *   DESCRIPTION: Prints prompt that debug exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints debug exception to the screen and freeze by using while loop
 */
void debug_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x01]);
    while(1);
}

/* 
 * nmi_interrupt
 *   DESCRIPTION: Prints prompt that non-maskable interrupt was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints nmi interrupt to the screen and freeze by using while loop
 */
void nmi_interrupt(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x02]);
    while(1);
}

/* 
 * breakpoint_exception
 *   DESCRIPTION: Prints prompt that breakpoint exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints breakpoint exception to the screen and freeze by using while loop
 */
void breakpoint_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x03]);
    while(1);
}

/* 
 * overflow_exception
 *   DESCRIPTION: Prints prompt that overflow exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints overflow exception to the screen and freeze by using while loop
 */
void overflow_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x04]);
    while(1);
}

/* 
 * bound_range_exceed_exception
 *   DESCRIPTION: Prints prompt that bound range exceeded exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints bound range exceeded exception to the screen and freeze by using while loop
 */
void bound_range_exceed_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x05]);
    while(1);
}

/* 
 * invalid_opcode_exception
 *   DESCRIPTION: Prints prompt that invalid opcode exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints invalid opcode exception to the screen and freeze by using while loop
 */
void invalid_opcode_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x06]);
    while(1);
}

/* 
 * device_not_available_exception
 *   DESCRIPTION: Prints prompt that device not available exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints device not available exception to the screen and freeze by using while loop
 */
void device_not_available_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x07]);
    while(1);
}

/* 
 * double_fault_exception
 *   DESCRIPTION: Prints prompt that double fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints double fault exception to the screen and freeze by using while loop
 */
void double_fault_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x08]);
    while(1);
}

/* 
 * coprocessor_segment_overrun
 *   DESCRIPTION: Prints prompt that coprocessor segment overrun exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints coprocessor segment overrun exception to the screen and freeze by using while loop
 */
void coprocessor_segment_overrun(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x09]);
    while(1);
}

/* 
 * invalid_TSS_exception
 *   DESCRIPTION: Prints prompt that invalid TSS exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints invalid TSS exception to the screen and freeze by using while loop
 */
void invalid_TSS_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x0A]);
    while(1);
}

/* 
 * segment_not_present
 *   DESCRIPTION: Prints prompt that segment not present exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints segment not present exception to the screen and freeze by using while loop
 */
void segment_not_present(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x0B]);
    while(1);
}

/* 
 * stack_fault_exception
 *   DESCRIPTION: Prints prompt that stack fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints stack fault exception to the screen and freeze by using while loop
 */
void stack_fault_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x0C]);
    while(1);
}

/* 
 * general_protection_fault
 *   DESCRIPTION: Prints prompt that general protection fault was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints general protection fault to the screen and freeze by using while loop
 */
void general_protection_fault(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x0D]);
    while(1);
}

/* 
 * page_fault_exception
 *   DESCRIPTION: Prints prompt that page fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints page fault exception to the screen and freeze by using while loop
 */
void page_fault_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x0E]);
    while(1);
}

/* Skip exception vector number 15 (Intel Reserved) */

/* 
 * FPU_floating_point_error
 *   DESCRIPTION: Prints prompt that FPU floating point error exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints FPU floating point error exception to the screen and freeze by using while loop
 */
void FPU_floating_point_error(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x10]);
    while(1);
}

/* 
 * alignment_check_exception
 *   DESCRIPTION: Prints prompt that alignment check exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints alignment check exception to the screen and freeze by using while loop
 */
void alignment_check_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x11]);
    while(1);
}

/* 
 * machine_check_exception
 *   DESCRIPTION: Prints prompt that machine check exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints machine check exception to the screen and freeze by using while loop
 */
void machine_check_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x12]);
    while(1);
}

/* 
 * SIMD_floating_point_exception
 *   DESCRIPTION: Prints prompt that SIMD floating point exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints SIMD floating point exception to the screen and freeze by using while loop
 */
void SIMD_floating_point_exception(void){
    printf("Exception\n");
    printf("%s\n", exceptionNames[0x13]);
    while(1);
}

