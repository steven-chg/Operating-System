#include "pit.h"
#include "i8259.h"
#include "lib.h"

//void pit_init()
//Input: N/A
//Output: N/A
//Effect: Initialize PIT values, turn on IRQ 0 and set frequency to default 100hz
extern void pit_init(){
    cli();
    outb(pit_mode,pit_port_cmd);
    outb((uint8_t)hundread_hz_rate,pit_port_channel_0_data);
    outb((uint8_t)(hundread_hz_rate>>8),pit_port_channel_0_data);
    sti();
    enable_irq(pit_irq_line);
}
//void pit_handler()
//Input: N/A
//Output: N/A
//Effect: handle PIT interrupts/ use for scheduling
extern void pit_handler(){
    //do something related to schedling
    //call process switch
    
    send_eoi(pit_irq_line);
    //switch_process();

}
