#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"

void io_wait(void) {
    out(0x80, 0);
}

void pic_ack(uint8_t irq) {
    if (irq >= 8) {
        out(PIC2_COMMAND, PIC_ACK);
    }
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void) {
    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); 
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100); // ICW3: tell Master PIC, slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();

    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();

    // Disable all interrupts
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}

void main_interrupt_handler(struct InterruptFrame frame) {
    switch (frame.int_number) {
        case (PIC1_OFFSET + IRQ_KEYBOARD) :
            // keyboard_isr();
            break;
    }
    // switch (frame.int_number) {
    //     case IRQ_TIMER:
    //         // Handle timer interrupt
    //         break;
    //     case IRQ_KEYBOARD:
    //         // Handle keyboard interrupt
    //         break;
    //     case IRQ_MOUSE:
    //         // Handle mouse interrupt
    //         break;
    //     // Add cases for other interrupt numbers as needed
    //     default:
    //         // No specific handler found, call the generic handler
    //         if (frame.int_number < ISR_STUB_TABLE_LIMIT && isr_stub_table[frame.int_number] != NULL) {
    //             void (*handler)(struct InterruptFrame) = isr_stub_table[frame.int_number];
    //             handler(frame);
    //         } else {
    //             // No handler found, print error message
    //             // You can replace this with your own error handling mechanism
    //             printf("Error: No handler found for interrupt number %u\n", frame.int_number);
    //         }
    //         break;
    // }

    // Acknowledge the interrupt
    pic_ack(frame.int_number);
}

void activate_keyboard_interrupt(void) {
    // Activate only keyboard interrupt (IRQ_KEYBOARD = 1)
    uint8_t mask = in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD);
    out(PIC1_DATA, mask);
}
