#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
#include "lib.h"
#include "file_system.h"


#define NUM_FILE_DES 8
#define IMAGE_ADDR 0x08048000
#define PROGRAM_IMAGE_OFFSET 0x48000

#define MAX_PID_NUM         8
#define PHYSICAL_MEMORY_START_IDX  2
#define ELF_SIZE        4
#define ELF_START       24

#define ADDRESS_8MB 0x800000    // 8MB = 8 * 1024 * 1024 B = 2^3 * 2^10 * 2^10 B = 2^23 B
#define NUM_BITS_8KB 0x2000    // 8MB = 8 * 1024 B = 2^3 * 2^10 B = 2^13 B

#define EXECUTABLE_BYTE1 0X7F       // magic number: first byte of executable file
#define EXECUTABLE_BYTE2 0X45       // magic number: second byte of executable file
#define EXECUTABLE_BYTE3 0X4C       // magic number: third byte of executable file
#define EXECUTABLE_BYTE4 0X46       // magic number: fourth byte of executable file
#define EXECUTABLE_BYTE 4           //the amount of magic numbers to check

#define ADDRESS_128MB 0x8000000     //128MB = 128*1024*1024 = 2^7 * 2^10 * 2^10 B = 2^27 B
#define ADDRESS_4MB  0x400000        // 4MB = 4 * 1024 * 1024 B = 2^2 * 2^10 * 2^10 B = 2^22 B




/* File Operations Jump Table Structure */
typedef struct{
    int32_t (*read)(int32_t fd, void* buf, int32_t nbytes);
    int32_t (*write)(int32_t fd, const void* buf, int32_t nbytes);
    int32_t (*open)(const uint8_t* fname);
    int32_t (*close)(int32_t fd);
} fileop_table_t;

fileop_table_t rtc_fileop_table;
fileop_table_t dir_fileop_table;
fileop_table_t file_fileop_table;
fileop_table_t stdin_fileop_table;
fileop_table_t stdout_fileop_table;

// called by pit handler to handle RR scheduling
extern void switch_process();

void init_file_op_table();
/* Functions to get the file operations jump table for the 4 file types */
/* 
 * init_rtc_fileop
 *   DESCRIPTION: Sets up a file operations jump table for rtc 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up a file operations jump table structure.
 */
void init_rtc_fileop();

/* 
 * init_stdout_fileop, init_stdin_fileop
 *   DESCRIPTION: Sets up a file operations jump table for terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up a file operations jump table structure.
 */
void init_stdout_fileop();
void init_stdin_fileop();

/* 
 * init_file_fileop
 *   DESCRIPTION: Sets up a file operations jump table for regular file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up a file operations jump table structure  
 */
void init_file_fileop();

/* 
 * init_directory_fileop
 *   DESCRIPTION: Sets up a file operations jump table for directory 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets up a file operations jump table structure
 */
void init_directory_fileop();

/* file array structure */
typedef struct {
    fileop_table_t* fileop_ptr; // To implement in later checkpoints, when we implement wrap drivers around a unified file system call interface (like the POSIX API)
    uint32_t inode_number;
    uint32_t file_position;
    uint32_t used;
} file_array_struct;


/* process control block  */
typedef struct{
    uint32_t pid;
    uint32_t pid_parent;
    uint32_t esp_execute;
    uint32_t ebp_execute;
    uint32_t esp0_tss;
    uint32_t eip_user;
    uint32_t esp_user;
    uint8_t command_arg[32];
    file_array_struct file_array[NUM_FILE_DES];
    uint32_t terminal_num;
} pcb_struct;


/*  DESCRIPTION: used to bridge C++ and asm
    INPUTS: NONE
    OUTPUTs: NONE
    RETURN VALUE: NONE
    SIDE EFFECTS: calls the function in the Systemcall linkage
    */
extern void system_call_handler_asm();

/* x
 * get_pcb_ptr
 *   DESCRIPTION: Get the pointer to the PCB of current process (for file system use)
 *   INPUTS: None
 *   OUTPUTS: pcb pointer to current process
 *   RETURN VALUE: pcb pointer
 *   SIDE EFFECTS: Finds pcb pointer for the current process
 */
pcb_struct* get_pcb_ptr();

/* 
 * get_pcb
 *   DESCRIPTION: Get the pointer to the PCB of the specific process 
 *   INPUTS: pid - process id of the pcb
 *   OUTPUTS: pcb pointer
 *   RETURN VALUE: pcb pointer
 *   SIDE EFFECTS: Finds pcb pointer for the given process
 */pcb_struct* get_pcb(uint32_t pid);


/* 
 * halt
 *   DESCRIPTION: Halts the system
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: halts current task, returns to parent
 */
int32_t halt(uint8_t status);


/* execute
 *   DESCRIPTION: Execute file
 *   INPUTS: command - word for command
 *   OUTPUTS: none
 *   RETURN VALUE: -1 for failure if file doesn't exist; value between 0 and 255 for halt 256 if exception;
 *   SIDE EFFECTS: Executes program 
 */
int32_t execute(const uint8_t* command);

/* 
 * open
 *   DESCRIPTION: Open system call provides access to the file system. 
 *   INPUTS: filename - name of the file to open 
 *   OUTPUTS: none
 *   RETURN VALUE: -1 (if file does not exist or no descriptors are free), file descriptor on success
 *   SIDE EFFECTS: Finds the directory entry corresponding to the named file,
 *                 allocate an unused file descriptor (if possible), and sets up 
 *                 necessary data to handle the file type
 */
int32_t open(const uint8_t* fname);

/* 
 * close
 *   DESCRIPTION: Close system call closes the specified file descriptor and makes it
 *                available for return from later calls to open.
 *   INPUTS: fd - file descriptor of file to close 
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
 *   SIDE EFFECTS: Should not allow attempts to close default descriptors (0 for input, 1 for output)
 */int32_t close(int32_t fd);

/*read
DESCRIPTION: real the file descriptor accoring to each file
INPUTS: fd - file descriptor to read, buf -- space to write to
nbytes - number of bytes to read
OUTPUTS: none
RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
SIDE EFFECTS: read the fd to the buf
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes);

/*write
DESCRIPTION: systemcall write, write
INPUTS: fd - file desscriptor to write to, nbytes -- number of bytes to write
OUTPUTS: none
RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
SIDE EFFECTS: write to the buf accoring to the types of file opened
*/
int32_t write(int32_t fd, void* buf, int32_t nbytes);

/* getargs
DESCRIPTION: 
INPUTS: buf - user-level buffer to store argument from user command line
        nbytes - number of bytes to retrieve 
OUTPUTS: none
RETURN VALUE: -1 (if no arguments or if arguments AND terminal NULL do not fit in the buffer); 0 (on sucess)
SIDE EFFECTS: copies user command line argument into user-level buffer
*/
int32_t getargs(uint8_t* buf, int32_t nbytes);



/* vidmap
DESCRIPTION: 
INPUTS: screen_start - pointer for screen starting addr
OUTPUTS: none
RETURN VALUE: -1 (if no arguments or if arguments AND terminal NULL do not fit in the buffer); 0 (on sucess)
SIDE EFFECTS: map the virtual and physical memory video page together
*/
int32_t vidmap(uint32_t** screen_start);


/*For extra credit
    return -1 for now*/
int32_t set_handler(int32_t signum, void* handler_address);
/*For extra credit
    return -1 for now*/
int32_t sigreturn(void);


#endif
