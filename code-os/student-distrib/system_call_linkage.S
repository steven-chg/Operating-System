#define ASM     1
#include "x86_desc.h"

# halt, child back to parent switch
.globl halt_asm
.align 4
halt_asm:
    # set up stack 
    pushl %ebp
    movl %esp, %ebp

    # get third argument or return value 
    movl 16(%ebp), %eax        

    # get the second argument
    movl 12(%ebp), %esp 

    # change stack 
    movl 8(%ebp), %ebp

    leave
    ret

.globl process_asm
.align 4
process_asm:
    # setup stack
    pushl %ebp
    movl %esp, %ebp

    # eip_arg, esp_arg, USER_CS, USER_DS
    # get 4th argument USER_DS
    movl 20(%ebp), %edx

    # get third argument USER_CS
    movl 16(%ebp), %ecx        

    # get the second argument esp_arg
    movl 12(%ebp), %ebx

    # get the first arg eip_arg
    movl 8(%ebp), %eax

    pushal

    andl    $0x00FF, %edx      # USER_DS (mask with 0x00FF to get lowest 8 bits)   
    movw    %dx, %ds           
    pushl   %edx                # push USER_DS
    pushl   %ebx                # push esp args
    pushfl                      # push EFLAG
    popl    %ebx                
    orl     $0x200, %ebx        # sets up interrupt flag
    pushl   %ebx                # push IF
    pushl   %ecx                # push CS
    pushl   %eax                # push return addr
    iret      

    popal
    # tear down
    movl %ebp, %esp
    leave
    ret

# changed to ours below
.globl system_call_handler_asm
.align 4
system_call_handler_asm:
    # save registers
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esp
    pushl %ebp
    pushl %esi
    pushl %edi

    # save flags
    pushfl

    # puts args
    pushl %edx
    pushl %ecx
    pushl %ebx

    # check if arguments above 10 or below 0 (support up to 10 system calls)
    cmpl $0, %eax
    jz invalid_arg
    cmpl $10, %eax
    ja invalid_arg

    call *system_calls_jump_table(, %eax, 4)
    jmp system_call_done

    # invalid argument (nonsupported system call), return -1
    invalid_arg:
        movl $-1, %eax

    system_call_done:
        # argument pop
        popl %ebx
        popl %ecx
        popl %edx

        # restore flags
        popfl

        # restore registers
        popl %edi
        popl %esi
        popl %ebp
        popl %esp
        popl %ebx
        popl %edx
        popl %ecx
        iret

# jump table for system calls
system_calls_jump_table:
    .long 0x0
    .long halt              # checkpoint 3
    .long execute           # checkpoint 3
    .long read              # checkpoint 3
    .long write             # checkpoint 3
    .long open              # checkpoint 3
    .long close             # checkpoint 3
    .long getargs           # checkpoint 4
    .long vidmap            # checkpoint 4
    .long set_handler
    .long sigreturn
