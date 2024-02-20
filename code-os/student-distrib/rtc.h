#ifndef _RTC_H
#define _RTC_H

#include "types.h"


//reference website: https://wiki.osdev.org/RTC

//a flag to enable RTC test or not
int RTC_test_enable;
int RTC_test_enable_int;

//define register value with nmi fisabled
#define Reg_A 0x8A
#define Reg_B 0x8B
#define Reg_C 0x8C

#define default_rate 0x06      //1024 case
#define default_interrupt_freq 1024

//define rtc ports
#define rtc_port_reg 0x70 
    //Port 0x70 is used to specify an index or "register number", and to disable NMI
#define rtc_port_data 0x71 
    // Port 0x71 is used to read or write from/to that byte of CMOS configuration space

#define rtc_irq_line 8

//initalizes values used by RTC
extern void rtc_init();

//handles RTC interrupts 
extern void rtc_handler();

//changes the rtc frq
extern int32_t rtc_change_freq(int32_t frequency);

//changes the rtc frq by given input
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

//changes the rtc freq to 2
extern int32_t rtc_open(const uint8_t* filename);
//blocks a function for a period of time
extern int32_t rtc_read();
//closes the file
extern int32_t rtc_close(int32_t fd);





#endif
