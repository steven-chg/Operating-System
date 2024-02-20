#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"
#include "types.h"
#include "systemcall.h"

#define VIDEO_MEM_ADDR  0xB8000     // video memory address

#define TERMINAL_VID_BUF_START 0xB9000
#define FOUR_KB 4096

#define BUFFER_SIZE 128             // buffer limit

/* terminal struct  */
typedef struct{
    uint32_t pid;                   // current process id
    uint32_t parent_pid;            // current process's parent id
    int processing;                 
    int terminal_num;
    int cursor_x;
    int cursor_y;
    char command[BUFFER_SIZE];
    int command_idx;
    int video_buffer;
    int number_of_processes;        // variable to track number of processes on terminal
    volatile int enter_flag;
    pcb_struct* curr_pcb_ptr;
} terminal_struct;

/////////////////////////////////////////////////////////////////////////////////
extern void update_video_memory_paging(terminal_struct* next_term);
terminal_struct terminal_array[3];  // construct a terminal array that is use for multiplay terminal 
terminal_struct* current_terminal;



/* void switch_terminal();
 * switch to the other terminal
 * 
 * Inputs: terminal_num 
 * Return Value: none
 * Side effects: switch to the other terminal */

extern void switch_terminal(int terminal_num);  

/* void terminal_init();
 * init terminal with default settings
 * 
 * Inputs: NONE
 * Return Value: none
 * Side effects: modify the terminal_array */

extern void terminal_init();  

/* void terminal_char(char c);
 * Store the typed character into buffer and allow terminal_read when
 * enter is typed
 * 
 * Inputs: c - typed character from keyboard
 * Return Value: none
 * Side effects: changed the enter_stat and command and command_idx */

extern void terminal_store_char(char c);                                              // handle typed character

/* int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes)
 * ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Inputs: c - typed character from keyboard
 * Return Value: none
 * Side effects: changed the enter_stat and command and command_idx */
extern int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);            // terminal read


/* int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes)
 * ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Inputs: c - typed character from keyboard
 * Return Value: none
 * Side effects: changed the enter_stat and command and command_idx */

extern int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);     // terminal write

/* int32_t terminal_open(const uint8_t* filename)
 * ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Inputs: filename
 * Return Value: 0
 * Side effects: */
extern int32_t terminal_open(const uint8_t* filename);                          // open terminal

/* int32_t terminal_close(int32_t fd)
 * ////////////////////////////////////////////////////////////////////////////////////////////////////////////
 * Inputs: filename
 * Return Value: 0
 * Side effects: */
extern int32_t terminal_close(int32_t fd);                                      // close terminal

#endif
