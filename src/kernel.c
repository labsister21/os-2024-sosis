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
    uint8_t arr1[CLUSTER_SIZE+1] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [CLUSTER_SIZE-1] = '1',
    [CLUSTER_SIZE] = '2',
    };
    struct FAT32DriverRequest requestWRITE1 = {
        .buf                   = arr1,
        .name                  = "file1",
        .ext                   = "",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE+1,
    } ;

    framebuffer_write(1,1,write(requestWRITE1)+'0',0xF,0);
    // char ar[14];
    // request.buf = ar;
    // framebuffer_write(0,2,read(request)+'0',0xF,0);
    uint8_t arr2[CLUSTER_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [CLUSTER_SIZE-2] = 'O',
    [CLUSTER_SIZE-1] = 'k',
    };
    struct FAT32DriverRequest requestWRITE2 = {
        .buf                   = arr2,
        .name                  = "file2",
        .ext                   = "",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    } ;
    framebuffer_write(1,2,write(requestWRITE2)+'0',0xF,0);
    uint8_t arr3[CLUSTER_SIZE] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
    [CLUSTER_SIZE-2] = 'O',
    [CLUSTER_SIZE-1] = 'k',
    };
    struct FAT32DriverRequest requestWRITE3 = {
        .buf                   = arr3,
        .name                  = "file3",
        .ext                   = "",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = CLUSTER_SIZE,
    } ;
    framebuffer_write(1,3,write(requestWRITE3)+'0',0xF,0);
    struct FAT32DriverRequest requestDELETE = {
        .buf                   = NULL,
        .name                  = "file1",
        .ext                   = "",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0,
    } ;
    framebuffer_write(1,4,delete(requestDELETE)+'0',0xF,0);
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
