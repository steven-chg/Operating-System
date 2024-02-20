#include "terminal.h"
#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "paging.h"
#include "systemcall.h"

#define NUM_COLS    80

char command[BUFFER_SIZE];          // buffer
int command_idx = 0;                // buffer index
extern void flush_tlb();

/* void switch_terminal();
 * switch to the other terminal
 * 
 * Inputs: terminal_num 
 * Return Value: none
 * Side effects: switch to the other terminal */

void update_video_memory_paging(terminal_struct* next_term){
    // if(get_pcb_ptr()->terminal_num == next_term->terminal_num){
    //     pte[VIDEO_MEM_ADDR>>12].bit_addr_31_12 = VIDEO_MEM_ADDR>>12;   // map the page to pte in physical mem
    //     // set up the physical mem video page
    //     // pte[0].present = 1;
    //     // pte[0].bit_addr_31_12 = (int)(VIDEO_MEM_ADDR>>PAGE_FRAME_BITS);   // map the page to virtual mem
    // } else{
    //     pte[VIDEO_MEM_ADDR>>12].bit_addr_31_12 = (VIDEO_MEM_ADDR>>12) + 1 + next_term->terminal_num;   // map the page to pte in physical mem

    // }
    pte[VIDEO_MEM_ADDR>>12].bit_addr_31_12 = (VIDEO_MEM_ADDR>>12) + 1 + next_term->terminal_num; // map the page to pte in physical mem
    flush_tlb(); // flush tlb
}

extern void switch_terminal(int terminal_num){
    // int i;
    //sanity check return if switch to the same one
    if ((terminal_num - 1) ==current_terminal->terminal_num){
        return;
    }
    

    // if(terminal_num == 2){
    //     printf("terminal 2");
    // }
    send_eoi(1);
    /* Save Terminal */

    // printf("t0");

    // //store command idx 
    //current_terminal->command_idx = command_idx;
    // //store command buffer
    // for (i = 0; i < BUFFER_SIZE; i++){
    //     current_terminal->command[i] = command[i];
    // }

    // printf("t1");

    int cur_pos = get_cursor_position();
    current_terminal->cursor_x = cur_pos%NUM_COLS; //store cursor location
    current_terminal->cursor_y = cur_pos/NUM_COLS;

    //pte[VIDEO_MEM_ADDR>>12].bit_addr_31_12 = VIDEO_MEM_ADDR>>12;
    //update_video_memory_paging(current_terminal);

    // printf("t02");

    //save current terminal screen to video page assign to it
    memcpy((void*)current_terminal->video_buffer,(void*)VIDEO_MEM_ADDR,FOUR_KB);


    /* Get New Terminal */
    current_terminal = (terminal_struct*) &terminal_array[terminal_num-1];
    
    //restore command buffer and position 
    // for (i = 0; i < BUFFER_SIZE; i++){
    //     command[i] = current_terminal->command[i];
    // }
    // printf("t03");


    command_idx = current_terminal->command_idx; // update terminal with command idx
    // update cursor to new position 
    update_cursor(current_terminal->cursor_x, current_terminal->cursor_y);
    // printf("t04");

    //restore new current terminal screen to video page assign to it
    memcpy((void*)VIDEO_MEM_ADDR,(void*)current_terminal->video_buffer,FOUR_KB);
    // printf("t05");

    
    if(current_terminal->number_of_processes == 0){ //print the current terminal after switching
        //execute((uint8_t*) "shell");
        printf("\nTerminal %d:\n", terminal_num);
        execute((uint8_t*) "shell");
        //execute((uint8_t*) "halt");
    }
    //update_video_memory_paging(current_terminal);

    //flush_tlb();


    // if(terminal_array[current_terminal->terminal_num].number_of_processes == 0){
    //     // if no processes have been running, then run shell 
    //         printf("t06");

    //     pte[VIDEO_MEM_ADDR>>12].bit_addr_31_12 = VIDEO_MEM_ADDR>>12;   // map the page to pte in physical mem
    //     flush_tlb();
    //     execute((uint8_t*) "shell");
    //         printf("t07");

    // } else{
    //     /* Switch Execution */
    //     update_video_memory_paging(current_terminal);
    //         printf("t08");

    // }
}

/* void terminal_init();
 * init terminal with default settings
 * 
 * Inputs: NONE
 * Return Value: none
 * Side effects: modify the terminal_array */

extern void terminal_init(){ 
    int i;
    int j;
    for (i = 0; i < 3; i++){
        terminal_array[i].processing = 0; // set the terminal to its default setting
        terminal_array[i].pid = 0;
        terminal_array[i].parent_pid = -1;
        terminal_array[i].terminal_num = i;
        terminal_array[i].cursor_x = 0;
        terminal_array[i].cursor_y = 0;
        terminal_array[i].command_idx = 0;
        terminal_array[i].video_buffer = TERMINAL_VID_BUF_START + (i*FOUR_KB);
        terminal_array[i].curr_pcb_ptr = NULL;
        terminal_array[i].number_of_processes=0;
        terminal_array[i].enter_flag = 0;
        for(j = 0; j < BUFFER_SIZE; j++){
            terminal_array[i].command[j] = '\0';
        }
    }
    //let the current terminal be the 0th terminal (0-index)
    current_terminal = &terminal_array[0];
    terminal_array[0].processing = 1;
}
// /* void terminal_store_char(char c);
//  * Store the typed character into buffer and allow terminal_read when
//  * enter is typed
//  * 
//  * Inputs: c - typed character from keyboard
//  * Return Value: none
//  * Side effects: changed the enter_stat and command and command_idx */

// void terminal_store_char(char c){
//     if(c=='\n'){
//         if(command_idx >= BUFFER_SIZE){     // if buffer is full
//             command[BUFFER_SIZE-1] = '\n';  // turn last character to \n
//         }
//         else{
//             command[command_idx] = '\n';    // else store enter
//         }
//         enter_stat = 1;                     // if enter is typed, allow terminal_read
//     }
//     else if(c=='\b'){                       // if backspace is typed
//         if(command_idx!=0){                 // if the buffer is not at start
//             if(command[command_idx-1]=='\t'){
//                 command[command_idx-1] = ' ';   // delete the last character (make it as space)
//                 command_idx--; 
//             }
//             else{
//                 command[command_idx-1] = ' ';   // delete the last character (make it as space)
//                 command_idx--; 
//             }
//                              // decrement buffer index
//         }
//     }
//     else if(command_idx < BUFFER_SIZE-1){   // otherwise, if the key is not special and buffer not full
//         command[command_idx] = c;           // store the typed character
//         command_idx++;                      // increment the buffer index
//     }
// }

/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * Read nbytes of characters from buf
 * Inputs: fd-file descriptor, buf-buffer to store characters, nbytes-number of characters to read
 * Return Value: number of characters read
 * Side effects: read current command into buf, changed the enter_stat and command and command_idx */

int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
    int i;
    if(buf==NULL)return -1;     // handle NULL buffer

    while(!current_terminal->enter_flag){}        // keep waiting for enter pressed

    cli();
    for(i=0;i<current_terminal->command_idx;i++){ //read buffer from terminal 
        command[i] = current_terminal->command[i];
    }
    command_idx = current_terminal->command_idx;
    for(i=0;i<nbytes;i++){                      // read each characters
        ((char*)buf)[i] = command[i];           // store each character into buf
        command[i] = ' ';                       // clear the current command
        if(((char*)buf)[i] == '\n'){            // when there's new line, end reading
            current_terminal->enter_flag = 0;
            current_terminal->command_idx = 0;
            sti();
            return i+1;                         // return total bytes read
        }
        if(i==nbytes-1 || i==BUFFER_SIZE-1){    // if read 128 or nbytes
            ((char*)buf)[i] = '\n';             // set last character to new line 
            current_terminal->enter_flag = 0;
            current_terminal->command_idx = 0;
            sti();
            return i+1;                         // return total bytes read
        }
    }
    sti();
    return 0;
}

/* int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 * Write n characters from buf to screen
 * Inputs: fd-file descriptor, buf-buffer that stores characters, nbytes-number of characters to write
 * Return Value: Number of characters written to screen
 * Side effects: write to video memory */

int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    cli();
    int i;
    if(buf == NULL || nbytes<0){                        // fail if buf is NULL or nbytes<0
        return -1;
    }
    for(i=0;i<nbytes;i++){                              // otherwise, look at each character
        if(((char*)buf)[i]!='\0'){                      // if it's a valid character
            putc(((char*)buf)[i]);                      // write to video mem
            // if(((char*)buf)[i]=='\n')return i+1;        // if it's new line, end writing
        }
        // else{
        //     return i+1;                                 // if there's invalid character, end writing
        // }
    }
    sti();
    return nbytes;                                      // return total bytes read
}

/* int32_t terminal_open(const uint8_t* filename)
 * Open terminal
 * Inputs: filename
 * Return Value: 0
 * Side effects: */

int32_t terminal_open(const uint8_t* filename){
    return 0;
}

/* int32_t terminal_close(int32_t fd)
 * Close terminal
 * Inputs: filename
 * Return Value: -1
 * Side effects: */
int32_t terminal_close(int32_t fd){
    return -1;
}

