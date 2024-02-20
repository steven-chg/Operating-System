#ifndef _PIT_H
#define _PIT_H

//define pit ports
#define pit_irq_line 0x00
#define pit_port_cmd 0x43
//we are using channel 0 for pit that connects the output to the PIC
#define pit_port_channel_0_data 0x40

//using channel 0 and mode 2
//0b00110100 
#define pit_mode 52
//100 hz rate = divisor = 1193180 / hz// from OSDEV;
#define hundread_hz_rate 11931


//initialize the pit
extern void pit_init();
//handles the pit
extern void pit_handler();

#endif
