//changed
#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

/*
 * keyboard_init()
 *  DESCRIPTION: Function to iniitialize keyboard by enabling its irq 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Initializes the keyboard and enable the irq.
 */
void keyboard_init();   // keyboard initialization

/*
 * keyboard_handler()
 *  DESCRIPTION: Function to handle keyboard by reading the typed-key and printing the key typed on the screen  
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Print the typed-key on the screen and store characters in buffer
 */
void keyboard_handler();    // keyboard handler


/*
 * special_status_key()
 *  DESCRIPTION: Function to update the pressed status of special keys 
 *  INPUTS: key
 *  OUTPUTS: int x
 *  RETURN VALUE: 1 if the key is special key, otherwise 0
 *  SIDE EFFECT: Update the special key status
 */
uint8_t special_status_key(uint8_t key);    // special key handler

#endif
