#ifndef EXCEPTION_HANDLER_H
#define EXCEPTION_HANDLER_H

// Function headers for handlers

/* 
 * divide_error_exception
 *   DESCRIPTION: Prints prompt that divide error exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints divide error exception to the screen and freeze by using while loop
 */
extern void divide_error_exception(void);

/* 
 * debug_exception
 *   DESCRIPTION: Prints prompt that debug exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints debug exception to the screen and freeze by using while loop
 */
extern void debug_exception(void);

/* 
 * nmi_interrupt
 *   DESCRIPTION: Prints prompt that non-maskable interrupt was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints nmi interrupt to the screen and freeze by using while loop
 */
extern void nmi_interrupt(void);

/* 
 * breakpoint_exception
 *   DESCRIPTION: Prints prompt that breakpoint exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints breakpoint exception to the screen and freeze by using while loop
 */
extern void breakpoint_exception(void);

/* 
 * overflow_exception
 *   DESCRIPTION: Prints prompt that overflow exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints overflow exception to the screen and freeze by using while loop
 */
extern void overflow_exception(void);

/* 
 * bound_range_exceed_exception
 *   DESCRIPTION: Prints prompt that bound range exceeded exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints bound range exceeded exception to the screen and freeze by using while loop
 */
extern void bound_range_exceed_exception(void);

/* 
 * invalid_opcode_exception
 *   DESCRIPTION: Prints prompt that invalid opcode exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints invalid opcode exception to the screen and freeze by using while loop
 */
extern void invalid_opcode_exception(void);

/* 
 * device_not_available_exception
 *   DESCRIPTION: Prints prompt that device not available exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints device not available exception to the screen and freeze by using while loop
 */
extern void device_not_available_exception(void);

/* 
 * double_fault_exception
 *   DESCRIPTION: Prints prompt that double fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints double fault exception to the screen and freeze by using while loop
 */
extern void double_fault_exception(void);

/* 
 * coprocessor_segment_overrun
 *   DESCRIPTION: Prints prompt that coprocessor segment overrun exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints coprocessor segment overrun exception to the screen and freeze by using while loop
 */
extern void coprocessor_segment_overrun(void);

/* 
 * invalid_TSS_exception
 *   DESCRIPTION: Prints prompt that invalid TSS exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints invalid TSS exception to the screen and freeze by using while loop
 */
extern void invalid_TSS_exception(void);

/* 
 * segment_not_present
 *   DESCRIPTION: Prints prompt that segment not present exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints segment not present exception to the screen and freeze by using while loop
 */
extern void segment_not_present(void);

/* 
 * stack_fault_exception
 *   DESCRIPTION: Prints prompt that stack fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints stack fault exception to the screen and freeze by using while loop
 */
extern void stack_fault_exception(void);

/* 
 * general_protection_fault
 *   DESCRIPTION: Prints prompt that general protection fault was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints general protection fault to the screen and freeze by using while loop
 */
extern void general_protection_fault(void);

/* 
 * page_fault_exception
 *   DESCRIPTION: Prints prompt that page fault exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints page fault exception to the screen and freeze by using while loop
 */
extern void page_fault_exception(void);

/* 
 * FPU_floating_point_error
 *   DESCRIPTION: Prints prompt that FPU floating point error exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints FPU floating point error exception to the screen and freeze by using while loop
 */
extern void FPU_floating_point_error(void);

/* 
 * alignment_check_exception
 *   DESCRIPTION: Prints prompt that alignment check exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints alignment check exception to the screen and freeze by using while loop
 */
extern void alignment_check_exception(void);

/* 
 * machine_check_exception
 *   DESCRIPTION: Prints prompt that machine check exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints machine check exception to the screen and freeze by using while loop
 */
extern void machine_check_exception(void);

/* 
 * SIMD_floating_point_exception
 *   DESCRIPTION: Prints prompt that SIMD floating point exception was raised
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Prints SIMD floating point exception to the screen and freeze by using while loop
 */
extern void SIMD_floating_point_exception(void);

#endif
