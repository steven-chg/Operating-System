#include "systemcall.h"
#include "rtc.h"
#include "paging.h"
#include "terminal.h"
#include "x86_desc.h"

//variables for keeping track of the pid values
uint32_t cur_pid = 0;
// uint32_t parent_pid = 0;
uint32_t pid_array[MAX_PID_NUM];

//Assembly functions. Descriptions in sycall_support.S
extern void flush_tlb();
extern void halt_asm(uint32_t execute_ebp, uint32_t execute_esp, uint8_t status);
extern void process_asm(uint32_t eip_arg, uint32_t user_ds, uint32_t user_cs, uint32_t esp_arg);

/* 
 * switch_process
 *   DESCRIPTION: called by the pit handler to do schdeling 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: switch the process
 */
extern void switch_process(){
    /*fetch the curr pid and related info*/
    // pcb_struct* cur_pcb_ptr = get_pcb_ptr();
    /*store the current process stack info*/
    // cur_pcb_ptr->esp_user = ok;
    // cur_pcb_ptr->ebp_execute = ok;
    // cur_pcb_ptr->esp0_tss = ok;
    /*fetch the next process info/ terminal info*/

    /*switch process*/
    //if no process create a new shell
}
/* 
 * halt
 *   DESCRIPTION: Halts the system
 *   INPUTS: status
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 *   SIDE EFFECTS: halts current task, returns to parent
 */
int32_t halt(uint8_t status){
  int i;
  int process_terminal;
//---------Restore parent data-----------------------------------------
    //printf("0");

    pcb_struct* cur_pcb_ptr = current_terminal->curr_pcb_ptr;
    process_terminal = cur_pcb_ptr->terminal_num;
    //return to shell if it is the base shell
    if(cur_pcb_ptr->pid_parent == -1){
            //printf("0.1");

        uint32_t eip_arg = cur_pcb_ptr->eip_user; //getting eip & esp arguments from user
        uint32_t esp_arg = cur_pcb_ptr->esp_user;
    //Enable interrupts
     sti();
         //printf("0.2");

        process_asm(eip_arg,esp_arg,USER_CS,USER_DS);
    }
    // get updated pid with parent's pcb
    // printf("0.5");
    // printf("%d", cur_pcb_ptr->pid_parent);

    pcb_struct* parent_pcb_ptr = get_pcb(cur_pcb_ptr->pid_parent);

        //printf("0.6");

    current_terminal->pid = parent_pcb_ptr->pid;
    cur_pid = parent_pcb_ptr->pid;
    

    current_terminal->curr_pcb_ptr = parent_pcb_ptr;
    //printf("0.7");

    // current_terminal->parent_pid = parent_pcb_ptr->pid;
    // terminal_array[process_terminal].pid = parent_pcb_ptr->pid;
    // terminal_array[process_terminal].parent_pid = parent_pcb_ptr->pid;


    //printf("0.8");
    pid_array[cur_pcb_ptr->pid] = 0;
    //printf("1");
// Restore Parent Paging
    uint32_t physical_addr = (PHYSICAL_MEMORY_START_IDX + cur_pid) * 0x400000;     // starts at 8mb
    pde[32].kb.bit_addr_31_12 = physical_addr >> 12; // get pde address by physical address / 4096 as each stores 4kb
    flush_tlb();    // flush tlb

// Reset all File descriptor to unused
    for(i=0;i<8;i++){
        cur_pcb_ptr->file_array[i].used = 0;
    }

// Store parent pid back to TSS
    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x800000 - (0x2000 *parent_pcb_ptr->pid) - sizeof(int32_t);
    //printf("2");
    current_terminal->number_of_processes--;
    //printf("2.5");

    // terminal_array[process_terminal].curr_pcb_ptr->pid = parent_pid;
    // terminal_array[process_terminal].curr_pcb_ptr->pid = current_terminal->parent_pid;
    //printf("3");
// Jump to Execute Return
    halt_asm(cur_pcb_ptr->ebp_execute,cur_pcb_ptr->esp_execute,status);                                     // error

    // printf("past halt");
    //  asm volatile (
    //     "movl %0, %%esp;"
    //     "movl %1, %%ebp;"
    //     :
    //     : "g"(cur_pcb_ptr->esp_execute), "g"(cur_pcb_ptr->ebp_execute) , "a"(status)
    // );
    //asm("movl %1, %0" : "=a"(value) : "r"(value));
    // asm("\t movl %%ebp, %0" : "=r"(cur_pcb_ptr->ebp_execute));

    // asm("\t movl %%esp, %0" : "=r"(cur_pcb_ptr->esp_execute));
    // asm("\t movl %%eax, %0" : "=g"(status));

    return -1;
}


/* execute
 *   DESCRIPTION: Execute file
 *   INPUTS: command - word for command
 *   OUTPUTS: none
 *   RETURN VALUE: -1 for failure if file doesn't exist; value between 0 and 255 for halt 256 if exception;
 *   SIDE EFFECTS: Executes program 
 */
int32_t execute(const uint8_t* command){
    int i;
    int cmd_idx = 0;    // start of cmd
    int arg_idx = 0;    // start of arg

    int length = strlen((const int8_t*)command);
    // printf("c1");

    //File system variables
    uint8_t cmd[32];
    uint8_t arg[32];

    for (i=0;i<32;++i){ // initialize array
      cmd[i] = '\0';
      arg[i] = '\0';
    }
    // printf("c2");

    dentry_t dentry_enter;
    uint8_t buf[sizeof(int32_t)];

    //Arguments for switching to user context.
    uint32_t eip_arg;
    uint32_t esp_arg;
    // printf("c3");


// Parse arguments
    i=0;
    while(i<length && command[i]==' '){ //space before command
        i++;
    }
    cmd_idx = i;
    while(i<length && command[i]!=' '){ //command 
        cmd[i-cmd_idx] = command[i];
        i++;
    }
    while(i<length && command[i]==' '){ //space between command and argument
        i++;
    }
    arg_idx = i;
    while(i<length && command[i]!=' '){ //argument
        arg[i-arg_idx] = command[i];
        i++;
    }
    // printf("c4");

// File executable check

    if(read_dentry_by_name(cmd, &dentry_enter)==-1){
        return -1;  // file DNE
    }

    if(read_data(dentry_enter.inode_number, 0, buf, sizeof(int32_t)) == -1){
        return -1;  // read data fail
    }

    if((buf[0] == EXECUTABLE_BYTE1 && buf[1] == EXECUTABLE_BYTE2 && buf[2] == EXECUTABLE_BYTE3 && buf[3] == EXECUTABLE_BYTE4)!=1) {
        return -1; /* ELF not found, not an executable*/
    }

    // printf("5");
// Paging set up
    pcb_struct* cur_pcb;
    for(i = 0; i < 8 ;i++){         
        if(pid_array[i] == 0){      // find unused pid
            pid_array[i] = 1;       // set to used
            cur_pid = i;                  
            break;
        }
    }
    if(i == 8){
        return -1;          // pid all used
    }
    // printf("6");
    uint32_t memory_index = PHYSICAL_MEMORY_START_IDX + cur_pid;               // starts from 8mb
    uint32_t physical_memory_addr = memory_index * 0x400000;        // find physical memory as each occupies 4mb 
    pde[32].kb.bit_addr_31_12 = physical_memory_addr >> 12;         // set user virtual memory to physical memory
    flush_tlb();

// memory load (file)
    index_node* temp_inode_ptr = (index_node *)(index_nodes_ptr+dentry_enter.inode_number);
    uint8_t* image_addr = (uint8_t*)IMAGE_ADDR;           //it should stay the same, overwriting existing program image
    read_data(dentry_enter.inode_number, (uint32_t)0, image_addr,temp_inode_ptr->file_length); //copying entire file to memory starting at Virt addr 0x08048000
    // printf("7");
// PCB creation

    cur_pcb = get_pcb(cur_pid);          // create pcb for current pcb
    cur_pcb->pid = cur_pid;              // store current pid

    // printf("8");

    /* Set Up Relevant Terminal Information */
    // Check if the current terminal has any processes
    if(current_terminal->number_of_processes == 0){
        // printf("a");
        // if not, then set parent of current process as -1
        cur_pcb->pid_parent = -1;
        // printf("b");
        cur_pcb->terminal_num = current_terminal->terminal_num;
        current_terminal->pid = cur_pid;
        // printf("c");
    } else{
        // if it has, then set parent process id accordingly
        // printf("d");
        cur_pcb->pid_parent = current_terminal->pid;
        // printf("e");
        current_terminal->pid = cur_pid;
        // printf("f");
        cur_pcb->terminal_num = current_terminal->terminal_num;
        // printf("g");
    }
    // printf("9");
    current_terminal->number_of_processes++;
    current_terminal->curr_pcb_ptr = cur_pcb;

    //printf("\nPID: %d ParentPID: %d Terminal: %d\n", cur_pcb->pid, cur_pcb->pid_parent, cur_pcb->terminal_num);
    // printf("10");
    // if(cur_pid != 0 ){
    //     cur_pcb->pid_parent = parent_pid;
    //     parent_pid = cur_pid;
    // }else{
    //     cur_pcb->pid_parent = cur_pid;
    // }


    // printf("11");

    // Initialize the file descriptor array
    for (i = 0; i < 8;i++) {
        if(i==0){
            cur_pcb->file_array[i].fileop_ptr = &stdin_fileop_table; //sender in 
            cur_pcb->file_array[i].used = 1;
        }
        else if(i==1){
            cur_pcb->file_array[i].fileop_ptr = &stdout_fileop_table; //sender out
            cur_pcb->file_array[i].used = 1;
        }
        else{
            cur_pcb->file_array[i].fileop_ptr = NULL;                //file 
            cur_pcb->file_array[i].used = 0;
        }
        cur_pcb->file_array[i].inode_number = 0;
        cur_pcb->file_array[i].file_position = 0;
    }
    // printf("12");

    strncpy((int8_t*)cur_pcb->command_arg, (int8_t*)(arg), 32);

  // Context Switch Set up (store registers)

    uint8_t eip_buffer[4];
    read_data(dentry_enter.inode_number, ELF_START, eip_buffer, sizeof(int32_t)); // Read eip
    eip_arg = *((int*)eip_buffer);
    // printf("13");

    esp_arg = 0x8000000 + 0x400000 - sizeof(int32_t); // where program starts

    cur_pcb->eip_user = eip_arg;
    cur_pcb->esp_user = esp_arg;

    //For privilege level switch
    tss.ss0 = KERNEL_DS;
    tss.esp0 = ADDRESS_8MB - (NUM_BITS_8KB*cur_pid) - sizeof(int32_t);
    cur_pcb->esp0_tss = tss.esp0;
    // printf("14");

    //Get the esp and ebp values for the user context switch.
    uint32_t esp;
    uint32_t ebp;
    asm("\t movl %%esp, %0" : "=r"(esp));
    asm("\t movl %%ebp, %0" : "=r"(ebp));
    cur_pcb->esp_execute = esp;
    cur_pcb->ebp_execute = ebp;
    // printf("15");

    //Enable interrupts
    sti();
        // printf("16");

    //push args for iret and iret
    process_asm(eip_arg,esp_arg,USER_CS,USER_DS);           // ERROR
        // printf("17");

    return 0;
}


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
int32_t open(const uint8_t* filename){
    int i;
    pcb_struct* current_pcb = get_pcb_ptr();            // get current process pcb
    dentry_t directory;

    // need to check for empty string; if empty, return -1
    if(strlen((char*)filename) == 0){
        return -1;
    }

    if(read_dentry_by_name(filename, &directory)==-1){  // read dentry by file name
        return -1;                                      // fail if filename doesn't exist
    }

    // start from index 2 in fd (0 and 1 are for stdin and stdout) through 8 non-inclusive (max 8 files to support)
    for(i=2;i<8;i++){                                   // iterate through each index in file array (not stdin or stdout)
        if(current_pcb->file_array[i].used == 0){         // if it's not used, check file type
            if(directory.file_type==0){
                current_pcb->file_array[i].fileop_ptr = &rtc_fileop_table;          // give rtc its op
            }
            else if(directory.file_type == 1){
                current_pcb->file_array[i].fileop_ptr = &dir_fileop_table;    // give directory its op
            }
            else if(directory.file_type == 2){
                current_pcb->file_array[i].fileop_ptr = &file_fileop_table;         // give file its file op
            }
            current_pcb->file_array[i].inode_number = directory.inode_number;           // assign inode
            current_pcb->file_array[i].file_position = 0;                               // set file position to 0
            current_pcb->file_array[i].used = 1;                                        // set it to used
            return i;                                                                   // return the index modified
        }
    }
    return -1;      // fail if all used

}

// CHANGED; verified
/* 
 * close
 *   DESCRIPTION: Close system call closes the specified file descriptor and makes it
 *                available for return from later calls to open.
 *   INPUTS: fd - file descriptor of file to close 
 *   OUTPUTS: none
 *   RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
 *   SIDE EFFECTS: Should not allow attempts to close default descriptors (0 for input, 1 for output)
 */
int32_t close(int32_t fd){
    // check if fd is an invalid descriptor (valid descriptor needs to be between 2 and 7 inclusive (0 indexed)); if so, return -1
    if(fd < 2 || fd > 7){
        return -1;
    }

    // retrieve the current pcb pointer and file array pointer
    pcb_struct* current_pcb = get_pcb_ptr();

    //check if the file is in use; if not, cannot close
    if (current_pcb->file_array[fd].used == 0) {
        return -1;
    }
    

    // file is open, so set free the descriptor (file_position reset in open)
    current_pcb->file_array[fd].used = 0;                // 0 for free descriptor

    // close the file by calling the appropriate file operation 
    return current_pcb->file_array[fd].fileop_ptr->close(fd);

;
}

/*read
DESCRIPTION: real the file descriptor accoring to each file
INPUTS: fd - file descriptor to read, buf -- space to write to
nbytes - number of bytes to read
OUTPUTS: none
RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
SIDE EFFECTS: read the fd to the buf
*/
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    sti();

    // check if fd is an invalid descriptor (valid descriptor needs to be between 0 and 7 inclusive (0 indexed)); if so, return -1
    // also check if we are given invalid buffer and bytes to read is less than 0
    if (fd < 0 || fd > 7 || buf == NULL || nbytes < 0) return -1;

    // if we are reading from stdout, return -1; invalid
    if (fd == 1) return -1;

    // get the current pcb 
    pcb_struct*  curr_pcb = get_pcb_ptr();

    //check if the file is in use; if not, cannot read from it, so return -1
    if (curr_pcb->file_array[fd].used == 0) {
        return -1;
    }
    
    // read and return result (how many bytes read)
    return curr_pcb->file_array[fd].fileop_ptr->read(fd, buf, nbytes);

}

// CHANGED verified NEED TO CHANGE COMMENT
/*write
DESCRIPTION: systemcall write, write
INPUTS: fd - file desscriptor to write to, nbytes -- number of bytes to write
OUTPUTS: none
RETURN VALUE: -1 if fd is invalid or attempt to close default fd, 0 on success
SIDE EFFECTS: write to the buf accoring to the types of file opened
*/
int32_t write(int32_t fd, void* buf, int32_t nbytes){
    // check if fd is an invalid descriptor (valid descriptor needs to be between 0 and 7 inclusive (0 indexed)); if so, return -1
    // also check if we are given invalid buffer and bytes to read is less than 0
    if (fd < 0 || fd > 8 || buf == NULL || nbytes < 0) return -1;

    // if we are writing to stdin, return -1; invalid
    if (fd == 0) return -1;

    // get current pcb 
    pcb_struct*  curr_pcb = get_pcb_ptr();

    //check if the file is in use, if not, cannot read, so return -1
    if (curr_pcb->file_array[fd].used == 0) {
        return -1;
    }

    // write and return result
    return curr_pcb->file_array[fd].fileop_ptr->write(fd, buf, nbytes);
}






//change verified
/* void init_file_op_table
 * Inputs      : N
 * Return Value: file descriptor  that has access to file
 * Function    : provides access to file system    */
void init_file_op_table() {
    //printf("file op table init\n");
    init_rtc_fileop();
    init_file_fileop();
    init_directory_fileop();
    init_stdout_fileop();
    init_stdin_fileop();
}

/* getargs
DESCRIPTION: 
INPUTS: buf - user-level buffer to store argument from user command line
        nbytes - number of bytes to retrieve 
OUTPUTS: none
RETURN VALUE: -1 (if no arguments or if arguments AND terminal NULL do not fit in the buffer); 0 (on sucess)
SIDE EFFECTS: copies user command line argument into user-level buffer
*/
int32_t getargs(uint8_t* buf, int32_t nbytes){
    
    // retrieve current pcb structure 
    pcb_struct* current_pcb = get_pcb_ptr();

    // if there are no arguments, return -1 
    if(current_pcb->command_arg[0] == NULL){
        return -1;
    }

    // copy the command line argument into the buffer
    strncpy((int8_t*) buf, (int8_t*) current_pcb->command_arg, (uint32_t) nbytes);
    // if arguments AND terminal NULL do not fit in the buffer 
    if(buf[31] != NULL){
        return -1;
    }

    return 0;
}

/* vidmap
DESCRIPTION: 
INPUTS: screen_start - pointer for screen starting addr
OUTPUTS: none
RETURN VALUE: -1 (if no arguments or if arguments AND terminal NULL do not fit in the buffer); 0 (on sucess)
SIDE EFFECTS: map the virtual and physical memory video page together
*/
int32_t vidmap(uint32_t** screen_start){

    //check if input is NULL or not
    if(screen_start == NULL){
        return -1;
    }
    //check if input is looking within the correct virtual mem location
    if(((int)screen_start < ADDRESS_128MB) || (ADDRESS_128MB + ADDRESS_4MB < (int)screen_start)){
        return -1;
    }
    // set up the physical mem video page
    pte[0].present = 1;    // mark the page as present
    pte[0].user_supervisor = 1;       // page can be used by user
    pte[0].bit_addr_31_12 = (int)(VIDEO_MEM_ADDR>>PAGE_FRAME_BITS);   // map the page to virtual mem

    // set up the virtual mem video page
    pde[(USERSPACE_ADDR + ADDRESS_8MB)>>22].kb.present = 1;    // mark the page as present
    pde[(USERSPACE_ADDR + ADDRESS_8MB)>>22].kb.page_size = 0;       // the page is 4 kb in size
    pde[(USERSPACE_ADDR + ADDRESS_8MB)>>22].kb.user_supervisor = 1;       // page can be used by user
    pde[(USERSPACE_ADDR + ADDRESS_8MB)>>22].kb.bit_addr_31_12 = (int)pte >> PAGE_FRAME_BITS;   // map the page to pte in physical mem
    

    flush_tlb(); // flush tlb

    *screen_start = (uint32_t*)(USERSPACE_ADDR + ADDRESS_8MB);

    return 0;
}

int32_t set_handler(int32_t signum, void* handler_address){
    return -1;
}
int32_t sigreturn(void){
    return -1;
}


//////////////////////////helper function///////////////////////////////////
/* x
 * get_pcb_ptr
 *   DESCRIPTION: Get the pointer to the PCB of current process (for file system use)
 *   INPUTS: None
 *   OUTPUTS: pcb pointer to current process
 *   RETURN VALUE: pcb pointer
 *   SIDE EFFECTS: Finds pcb pointer for the current process
 */

pcb_struct* get_pcb_ptr(){
    // return (pcb_struct*)(ADDRESS_8MB-NUM_BITS_8KB*(cur_pid+1));
    // return (pcb_struct*)(ADDRESS_8MB-NUM_BITS_8KB*(current_terminal->curr_pcb_ptr->pid+1));
    return (pcb_struct*)(ADDRESS_8MB-NUM_BITS_8KB*(current_terminal->pid+1));
}

/* 
 * get_pcb
 *   DESCRIPTION: Get the pointer to the PCB of the specific process 
 *   INPUTS: pid - process id of the pcb
 *   OUTPUTS: pcb pointer
 *   RETURN VALUE: pcb pointer
 *   SIDE EFFECTS: Finds pcb pointer for the given process
 */
pcb_struct* get_pcb(uint32_t pid){
    return (pcb_struct*)(ADDRESS_8MB-NUM_BITS_8KB*(pid+1));
}


/*
DESCRIPTION: assign open, close, read, write operations to different operations
INPUTS: NONE
OUTPUTS: NONE
RETURN VALUE: NONE
SIDE EFFFECTS: Assign open, close, read and write operations to 
                    RTC ops, file ops, directory ops, stdout ops and stdin ops
*/
void init_rtc_fileop(){
    rtc_fileop_table.open = rtc_open;
    rtc_fileop_table.close = rtc_close;
    rtc_fileop_table.read = rtc_read;
    rtc_fileop_table.write = rtc_write;
}
void init_file_fileop(){
    file_fileop_table.open = file_open;
    file_fileop_table.close = file_close;
    file_fileop_table.read  = file_read;
    file_fileop_table.write = file_write;
}
void init_directory_fileop(){
    dir_fileop_table.open = directory_open;
    dir_fileop_table.close = directory_close;
    dir_fileop_table.read  = directory_read;
    dir_fileop_table.write = directory_write;
}
void init_stdout_fileop(){
    stdout_fileop_table.open = terminal_open;
    stdout_fileop_table.close = terminal_close;
    stdout_fileop_table.read = NULL;
    stdout_fileop_table.write = terminal_write;
}
void init_stdin_fileop(){
    stdin_fileop_table.open = terminal_open;
    stdin_fileop_table.close = terminal_close;
    stdin_fileop_table.read = terminal_read;
    stdin_fileop_table.write = NULL;
}
