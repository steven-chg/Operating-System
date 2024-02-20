#include "rtc.h"
#include "i8259.h"
#include "lib.h"
// #include <cmath> 
//#include <stdio.h> 


volatile int rtc_counter;
volatile int rtc_interrupt;


//int rtc_inti()
//Input: N/A
//Output: N/A
//Effect: Initialize RTC values, turn on IRQ 8 and set frequency to default 1024hz
void rtc_init(){
    char prev;


    cli();                    //turning on IRQ 8
    //NMI_disable();
    outb(Reg_B, rtc_port_reg); // select register B, and disable NMI
    prev = inb(rtc_port_data); // read the current value of register B
    outb(Reg_B, rtc_port_reg); // set the index again (a read will reset the index to register D)
    outb(prev|0x40, rtc_port_data); // write the previous value ORed with 0x40. This turns on bit 6 of register B

    // outb(Reg_A, rtc_port_reg); // set index to register A, disable NMI
    // prev = inb(rtc_port_data); // get initial value of register A
    // outb(Reg_A, rtc_port_reg); // reset index to A
    // outb( (prev & 0xF0) | default_rate , rtc_port_data); //write only our rate to A. Note, rate is the bottom 4 bits
    rtc_change_freq(default_interrupt_freq);
    //NMI_enable();
    sti();
    //enable_irq(2);
    enable_irq(rtc_irq_line); //turing on IRQ 8

    // outb(Reg_C, rtc_port_reg); // select register C
    // inb(rtc_port_data);        // just throw away contents

    //rtc_change_freq(default_rate);
    //printf("initialized RTC\n");

}



//void rtc_handler()
//Input: N/A
//Output: N/A
//Effect: handle interrupts and call test_interrupts
void rtc_handler(){
    //use for testing the interrupt
    if(RTC_test_enable_int){
        test_interrupts();
    }
    rtc_interrupt = 1;
    //read Reg C for irq to happen again:: from OSDev
    outb(Reg_C, rtc_port_reg); // select register C
    inb(rtc_port_data);        // just throw away contents
    send_eoi(rtc_irq_line);
}

//void rtc_write()
//Input: fd, buf 
//Output: 0 for sucess -1 for fail  
//Effect: changes the freq 
int rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    //sanity check
    if (buf == NULL) return -1;

    //check if the input freq is valid
    if (rtc_change_freq(*(int32_t*)(buf)) == -1){
        return -1;
    }
    return 0;

}
//void rtc_change_freq()
//Input: frequency 
//Output: 0 for sucess -1 for fail 
//Effect: changes the freq 
int32_t rtc_change_freq(int32_t frequency){
    unsigned char rate;
    
    //frequency =  32768 >> (rate-1);
    switch (frequency)
    {
    case 1024:
        //frequency =  32768 >> (rate-1);
        rate = 0x06;
        break;
    case 512:
        //frequency =  32768 >> (rate-1);
        rate = 0x07;
        break;
    case 256:
        //frequency =  32768 >> (rate-1);
        rate = 0x08;
        break;
    case 128:
        //frequency =  32768 >> (rate-1);
        rate = 0x09;
        break;
    case 64:
        //frequency =  32768 >> (rate-1);
        rate = 0x0A;
        break;
    case 32:
        //frequency =  32768 >> (rate-1);
        rate = 0x0B;
        break;
    case 16:
        //frequency =  32768 >> (rate-1);
        rate = 0x0C;
        break;
    case 8:
        //frequency =  32768 >> (rate-1);
        rate = 0x0D;
        break;
    case 4:
        //frequency =  32768 >> (rate-1);
        rate = 0x0E;
        break;
    case 2:
        //frequency =  32768 >> (rate-1);
        rate = 0x0F;
        break;
    
    default:
        //if the input freq doesn't match/ return
        return -1;
    }
    //from OSDEVC
    rate &= 0x0F;                   // to mask it under 15
    cli();
    outb(Reg_A, rtc_port_reg);		// set index to register A, disable NMI
    char prev=inb(rtc_port_data);	// get initial value of register A
    outb(Reg_A, rtc_port_reg);		// reset index to A
    outb(((prev & 0xF0) | rate),rtc_port_data); //write only our rate to A. Note, rate is the bottom 4 bits.
    sti();


    return 0;
}




// int32_t rtc_open(const uint8_t* filename)
//Input: filename 
//Output: 0 if sucess   
//Effect: change the freq to 2hz
 int32_t rtc_open(const uint8_t* filename){
    //sanity check
    if (rtc_change_freq(2) == -1){
        return -1;
    } //set to default 2Hz
    return 0;
 }

// int32_t rtc_read()
//Input: N/A
//Output: 0 if sucess   
//Effect: block the function for set amount of time
 int32_t rtc_read(){
    rtc_interrupt = 0;
    //block the function
    while (rtc_interrupt == 0){
        //printf("waiting");
    }
    return 0;
 }


// close(int32_t fd)
//Input: fd
//Output: 0 if sucess   
//Effect: N/A
int32_t rtc_close(int32_t fd){
    return 0;
}
