#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/driver/disk.h"
#include "header/driver/fat32.h"

// void kernel_setup(void) {
//     load_gdt(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     activate_keyboard_interrupt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
    
//     struct BlockBuffer b;
//     for (int i = 0; i < 512; i++) b.buf[i] = i % 16;
//     write_blocks(&b, 17, 1);
//     while (true);

//     int col = 0;
//     keyboard_state_activate();
//     while (true) {
//          char c;
//          get_keyboard_buffer(&c);
//          if (c) {
//             framebuffer_write(0, col++, c, 0xF, 0);
            
//          }
//     }

// }

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();

    // char name[8]="folder1\0",ext[3]="\0\0\0";
    // read_clusters(&fat32_driver_state.dir_table_buf, ROOT_CLUSTER_NUMBER, 1);
    // int idx = findEntry(fat32_driver_state.dir_table_buf,name,ext);
    // uint32_t cluster_parent = (fat32_driver_state.dir_table_buf.table[idx].cluster_high<<16)|(fat32_driver_state.dir_table_buf.table[idx].cluster_low);
    struct FAT32DriverRequest request = {
        .buf                   = NULL,
        .name                  = "matthew",
        .ext                   = "",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 1000,
    } ;

    framebuffer_write(0,1,write(request)+'0',0xF,0);
    framebuffer_write(0,2,read(request)+'0',0xF,0);
    while(true){
        keyboard_state_activate();
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
