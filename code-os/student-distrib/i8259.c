/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask = 0xFF; /* IRQs 0-7  */
uint8_t slave_mask = 0xFF;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
 
	// a1 = inb(MASTER_8259_DATA);                        // save masks
    // a2 = inb(SLAVE_8259_DATA);
    //mask all the interupts before taking in
    outb(0xFF, MASTER_8259_DATA);
    outb(0xFF, SLAVE_8259_DATA);
    //send ICW1 to PIC inform initialization to PIC
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);
    //send ICW2 to PIC specify the offset
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    //send ICW3 to PIC configure the CAS
    outb(ICW3_MASTER, MASTER_8259_DATA);
    outb(ICW3_SLAVE, SLAVE_8259_DATA);
    //send ICW4 to PIC
    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);
    
    outb(0xFF, MASTER_8259_DATA);             //restore the mask settings
    outb(0xFF, SLAVE_8259_DATA);
    //enable the slave PIC
    enable_irq(SLAVE_IRQ_ON_MASTER);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    //if the input is out of range return
    if (irq_num > MAX_IRQ_NUM) {
        return;
    }
    //slave PIC's IRQ
    if (irq_num >= PIC_IRQ_NUM) {
        slave_mask &= ~(1 << (irq_num - PIC_IRQ_NUM));
        outb(slave_mask, SLAVE_8259_DATA);
        //return;
    } else {
        // master PIC's IRQ
        master_mask &= ~(1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);
        //return;
    }


}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if (irq_num > MAX_IRQ_NUM) {
        return;
    }
    //slave PIC's IRQ
    if (irq_num >= PIC_IRQ_NUM) {
        slave_mask |= (1 << (irq_num - PIC_IRQ_NUM));
        outb(slave_mask, SLAVE_8259_DATA);
        //return;
    } else {
        // master PIC's IRQ
        master_mask |= (1 << irq_num);
        outb(master_mask, MASTER_8259_DATA);
        //return;
    }

}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num > 15) {
        return;
    }
    //slave PIC's IRQ
    if (irq_num > 7) {
        //send EOI to slave PIC
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
        //send EOI to master PIC
        outb(EOI | 2, MASTER_8259_PORT);
        //return;
    } else {
        //send EOI to master PIC
        outb(EOI | irq_num, MASTER_8259_PORT);
        //return;
    }
}
