#include "keyboard.h"
#include "terminal.h"
#include "i8259.h"
#include "lib.h"

#define KEYBOARD_IRQ 1          // IRQ number for keyboard
#define KEY_NUM 58              // 58 of keys as keys after 0x3A, such as F1, F2, are not used
#define KEYBOARD_PORT 0x60      // set the keyboard port at 0x60
#define BUFFER_SIZE 128         // buffer size is 128

#define TAB_KEY 0x0F            // scancode for tab

#define ESC_KEY 0x01            // scancode for esc 
#define ESC_RELEASED 0x81       // scancode for esc released

#define BACKSPACE_KEY 0x0E      // scancode for backspace

#define CONTROL_KEY 0x1D        // scancode for control
#define CONTROL_RELEASED 0x9D   // scancode for control released

#define LEFT_SHIFT_KEY 0X2A         // scancode for left shift 
#define LEFT_SHIFT_RELEASED 0XAA    // scancode for left shift released

#define RIGHT_SHIFT_KEY 0x36        // scancode for right shift
#define RIGHT_SHIFT_RELEASED 0xB6   // scancode for right shift released

#define CAPS_LOCK_KEY 0x3A          // scancode for capslock
#define CAPS_LOCK_RELEASED 0xBA     // scancode for capslock released

#define ALT_KEY 0x38            // scancode for alt
#define ALT_RELEASED 0xB8       // scancode for alt released

#define F1_KEY 0x3B            // scancode for F1
#define F1_RELEASED 0xBB       // scancode for F1 released

#define F2_KEY 0x3C            // scancode for F2
#define F2_RELEASED 0xBC       // scancode for F2 released

#define F3_KEY 0x3D            // scancode for F3
#define F3_RELEASED 0xBD       // scancode for F3 released

unsigned char scancode_key[KEY_NUM][2] =                // store the characters for each key and its shifted key
{ {0x0,0x0} , {0x0,0x0} , {'1','!'} ,{'2','@'} ,{'3','#'} , // 0x0 means do nothing
  {'4','$'} , {'5','%'} , {'6','^'} , {'7','&'} ,
  {'8','*'} , {'9','('} , {'0',')'} , {'-','_'} ,
  {'=','+'} , {'\b','\b'} , {'\t','\t'} , {'q','Q'} , {'w','W'} , // \b means backspace and \t means tab
  {'e','E'} , {'r','R'} , {'t','T'} , {'y','Y'} ,
  {'u','U'} , {'i','I'} , {'o','O'} , {'p','P'} ,
  {'[','{'} , {']','}'} , {'\n','\n'} , {0x0,0x0} , // '\n' is new line and 0 is nothing 
  {'a','A'} , {'s','S'} , {'d','D'} , {'f','F'} ,
  {'g','G'} , {'h','H'} , {'j','J'} , {'k','K'} ,
  {'l','L'} , {';',':'} , {'\'','\''} , {'`','~'} ,   
  {0x0,0x0} , {'\\','|'} , {'z','Z'} , {'x','X'} ,
  {'c','C'} , {'v','V'} , {'b','B'} ,{'n','N'} ,
  {'m','M'} , {',','<'} , {'.','>'} , {'/','?'} ,
  {0x0,0x0} , {0x0,0x0} , {0x0,0x0} , {' ',' '} ,      
};

// char buffer[BUFFER_SIZE];                   // character array for buffer   
// uint8_t buffer_idx          = 0;            // buffer index
uint8_t CAPS_LOCK_STAT      = 0;            // status of CAPS_LOCK, 1 = on, 0 = off
uint8_t CAPS_LOCK_PRESSED   = 0;            // 1 if CAPS_LOCK is pressed, 0 otherwise
uint8_t CONTROL_PRESSED     = 0;            // 1 if CONTROL is pressed, 0 otherwise
uint8_t SHIFT_PRESSED       = 0;            // 1 if SHIFT is pressed, 0 otherwise
uint8_t ESC_PRESSED         = 0;            // 1 if ESC is pressed, 0 otherwise
uint8_t ALT_PRESSED         = 0;            // 1 if ALT is pressed, 0 otherwise
int i;

/*
 * keyboard_init()
 *  DESCRIPTION: Function to iniitialize keyboard by enabling its irq 
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Initializes the keyboard and enable the irq.
 */

void keyboard_init(){
    enable_irq(KEYBOARD_IRQ);   // enable the keyboard irq
    // buffer_idx          = 0;            // buffer index
    CAPS_LOCK_STAT      = 0;            // status of CAPS_LOCK, 1 = on, 0 = off
    CAPS_LOCK_PRESSED   = 0;            // 1 if CAPS_LOCK is pressed, 0 otherwise
    CONTROL_PRESSED     = 0;            // 1 if CONTROL is pressed, 0 otherwise
    SHIFT_PRESSED       = 0;            // 1 if SHIFT is pressed, 0 otherwise
    ESC_PRESSED         = 0;            // 1 if ESC is pressed, 0 otherwise
    ALT_PRESSED         = 0;            // 1 if ALT is pressed, 0 otherwise
}


/*
 * special_status_key()
 *  DESCRIPTION: Function to update the pressed status of special keys 
 *  INPUTS: key
 *  OUTPUTS: int x
 *  RETURN VALUE: 1 if the key is special key, otherwise 0
 *  SIDE EFFECT: Update the special key status
 */

uint8_t special_status_key(uint8_t key){
    char*  buffer = current_terminal->command;
    switch(key){                            // determine the key pressed
        case ESC_KEY:
            ESC_PRESSED = 1;                // set ESC_PRESSED if esc is pressed
            return 1;
        case ESC_RELEASED:
            ESC_PRESSED = 0;                // set ESC_PRESSED if esc is released
            return 1;
        case CONTROL_KEY:
            CONTROL_PRESSED = 1;            // set CONTROL_PRESSED if control is pressed
            return 1;
        case CONTROL_RELEASED:
            CONTROL_PRESSED = 0;            // set CONTROL_PRESSED if control is released
            return 1;
        case LEFT_SHIFT_KEY:
            SHIFT_PRESSED = 1;              // set SHIFT_PRESSED if left shift is pressed
            return 1;
        case RIGHT_SHIFT_KEY:
            SHIFT_PRESSED = 1;              // set SHIFT_PRESSED if right shift is pressed
            return 1;
        case LEFT_SHIFT_RELEASED:
            SHIFT_PRESSED = 0;              // set SHIFT_PRESSED if left shift is released
            return 1;
        case RIGHT_SHIFT_RELEASED:
            SHIFT_PRESSED = 0;              // set SHIFT_PRESSED if right shift is released
            return 1;
        case CAPS_LOCK_KEY:
            if(CAPS_LOCK_PRESSED==0){       // if CAPS_LOCK is pressed in the first moment
                CAPS_LOCK_STAT = 1^CAPS_LOCK_STAT;  // Set CAPS_LOCK_STAT to opposite value
                CAPS_LOCK_PRESSED = 1;      // set CAPS_LOCK_PRESSED to 1 to prevent the situation of holding CAPS and switching caps
            }
            return 1;
        case CAPS_LOCK_RELEASED:
            CAPS_LOCK_PRESSED =0;           // if CAPS_LOCK is released, set CAPS_LOCK_PRESSED to 0
            return 1;
        case ALT_KEY:
            ALT_PRESSED = 1;                // set ALT_PRESSED if alt is pressed
            return 1;
        case ALT_RELEASED:
            ALT_PRESSED = 0;                // set ALT_PRESSED if alt is released
            return 1;
        case BACKSPACE_KEY:
            if(current_terminal->command_idx!=0){              // if buffer is not at 0
                if(buffer[current_terminal->command_idx-1]=='\t'){
                    putc('\b');                 // delete a character
                    putc('\b');                 // delete a character
                    putc('\b');                 // delete a character
                    putc('\b');                 // delete a character
                    // terminal_store_char('\b'); 
                }
                else{
                    putc('\b');                 // delete a character
                    // terminal_store_char('\b'); 
                }
                current_terminal->command_idx--;               // decrement the buffer index
                buffer[current_terminal->command_idx] = '\0'; 
            }                         
            return 1;
        // case TAB_KEY:
        //     putc('\t');                     // print tab
        //     terminal_store_char('\t');      // store 4 spaces in terminal buffer
        //     int i;
        //     for(i=buffer_idx; i<BUFFER_SIZE && i< (buffer_idx+4);i++){
        //         buffer[i] = ' '; 
        //         buffer_idx = i;
        //     }                  
        //     return 1;
        default:
            return 0;
    }
}

/*
 * keyboard_handler()
 *  DESCRIPTION: Function to handle keyboard by reading the typed-key and printing the key typed on the screen  
 *  INPUTS: none
 *  OUTPUTS: none
 *  RETURN VALUE: none
 *  SIDE EFFECT: Print the typed-key on the screen and store characters in buffer
 */
void keyboard_handler(void) {
    cli();
    char*  buffer = current_terminal->command;
    uint8_t scancode = inb(KEYBOARD_PORT);              // get the scancode from keyboard
    if(special_status_key(scancode)==1){                // if the scancode is special key
        send_eoi(KEYBOARD_IRQ);     // end of interrupt
        sti();
        return;                                         // don't print it
    }
    if(ALT_PRESSED){                    //switch terminals ALT+F1 or F2 or F3
        if(scancode == F1_KEY){
            switch_terminal(1);
            send_eoi(KEYBOARD_IRQ);     // end of interrupt
            sti();
            return;
        }
        else if(scancode == F2_KEY){
            switch_terminal(2);
            send_eoi(KEYBOARD_IRQ);     // end of interrupt
            sti();
            return;
        }
        else if(scancode == F3_KEY){
            switch_terminal(3);
            send_eoi(KEYBOARD_IRQ);     // end of interrupt
            sti();
            return;
        }
    }

    if(scancode>=KEY_NUM){
        send_eoi(KEYBOARD_IRQ);     // end of interrupt
        sti();
        return;                     // if not print, do nothing
    }
    if(CONTROL_PRESSED){                                
        if(scancode_key[scancode][0]=='l'){             // if control+l is pressed
            clear();                                    // clear video memory
            send_eoi(KEYBOARD_IRQ);     // end of interrupt
            sti();
            return;
        }
    }
    if(current_terminal->command_idx >= BUFFER_SIZE-1){
        if(scancode_key[scancode][0] == '\n'){
            // terminal_store_char('\n');
            buffer[current_terminal->command_idx] = '\n';
            current_terminal->command_idx++;                     // next line -> buffer starts over
            current_terminal->enter_flag = 1;
            putc('\n');                 // print new line
        }
        send_eoi(KEYBOARD_IRQ);     // end of interrupt
        sti();
        return;                     // print nothing when buffer is full
    }
    if(scancode_key[scancode][0] == '\n'){
        // terminal_store_char('\n');
        buffer[current_terminal->command_idx] = '\n';          // store the char into buffer
        current_terminal->command_idx++;                     // next line -> buffer starts over
        current_terminal->enter_flag = 1;
        putc('\n');                 // print new line
        send_eoi(KEYBOARD_IRQ);     // end of interrupt
        sti();
        return;
    }
    
    if('a' <= scancode_key[scancode][0] && scancode_key[scancode][0] <= 'z'){       // if the key is letter
        if(SHIFT_PRESSED){
            buffer[current_terminal->command_idx] = scancode_key[scancode][1^CAPS_LOCK_STAT];          // store the char into buffer
            // terminal_store_char(buffer[buffer_idx]);
            putc(buffer[current_terminal->command_idx]);                         // only print opposite case 
            current_terminal->command_idx++;                                                           // increase buffer index
        }
        else if(CAPS_LOCK_STAT){
            buffer[current_terminal->command_idx] = scancode_key[scancode][1];          // store the char into buffer
            // terminal_store_char(buffer[buffer_idx]);
            putc(buffer[current_terminal->command_idx]);                         // only print upper case 
            current_terminal->command_idx++;                                            // increase buffer index
        }
        else{
            buffer[current_terminal->command_idx] = scancode_key[scancode][0];          // store the char into buffer
            // terminal_store_char(buffer[buffer_idx]);
            putc(buffer[current_terminal->command_idx]);                         // only print lower case 
            current_terminal->command_idx++;                                            // increase buffer index
        }
    }
    else{
        if(scancode_key[scancode][0]!= 0x0){
            buffer[current_terminal->command_idx] = scancode_key[scancode][SHIFT_PRESSED];          // store the char into buffer
            // terminal_store_char(buffer[buffer_idx]);
            putc(buffer[current_terminal->command_idx]);                         // only print character according to shift status 
            current_terminal->command_idx++;                                                       // increase buffer index
        }
    }
    send_eoi(KEYBOARD_IRQ);     // end of interrupt
    sti();
}
