#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
        
    int col = 0,row=0;
    keyboard_state_activate();
    while (true) {
         char c;
         get_keyboard_buffer(&c);
         if (c) {
            if(c=='\n'){
                row++;
                col=0;
            }else if(c=='\t'){
                for(int i=0;i<8;i++){
                    framebuffer_write(row,col++,' ',0xF,0);
                }
            }else{
                framebuffer_write(row, col++, c, 0xF, 0);
            }
            framebuffer_set_cursor(row,col+1);
         }
    }
}



// void kernel_setup(void)
// {
//     framebuffer_clear();
//     framebuffer_write(3, 8,  'H', 0, 0xF);
//     framebuffer_write(3, 9,  'a', 0, 0xF);
//     framebuffer_write(3, 10, 'i', 0, 0xF);
//     framebuffer_write(3, 11, '!', 0, 0xF);
//     framebuffer_set_cursor(3, 10);
//     while (true);
// }


// #include <stdint.h>
// #include <stdbool.h>
// #include "header/cpu/gdt.h"
// #include "header/kernel-entrypoint.h"
// #include "header/text/framebuffer.h"

// void kernel_setup(void) {
//     // Load the Global Descriptor Table (GDT)
//     load_gdt(&_gdt_gdtr);

//     // Clear framebuffer
//     framebuffer_clear();

//     // Write "Hi!" to the framebuffer at position (3, 8)
//     framebuffer_write(3, 8, 'H', 0, 0xF);
//     framebuffer_write(3, 9, 'i', 0, 0xF);
//     framebuffer_write(3, 10, '!', 0, 0xF);

//     // Set cursor position to (3, 10)
//     framebuffer_set_cursor(3, 10);

//     // Infinite loop to halt execution
//     while (true);
// }
