#include <stdint.h>
#include <stdbool.h>
#include "header/cpu/gdt.h"
#include "header/kernel-entrypoint.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/driver/disk.h"
#include "header/filesystem/fat32.h"
#include "header/memory/paging.h"
#include "header/process/process.h"
#include "header/scheduler/scheduler.h"

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

// TERAKHIT COMMIT ===================================================================================================

// void kernel_setup(void) {
//     load_gdt(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     activate_keyboard_interrupt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
//     initialize_filesystem_fat32();
//     paging_allocate_user_page_frame(&_paging_kernel_page_directory,(void*)0x500000);
//     *((uint8_t*) 0x500000) = 1;
//     paging_free_user_page_frame(&_paging_kernel_page_directory,(void*)0x500000);
//     *((uint8_t*) 0x500000) = 1;
//     while(true){
//         keyboard_state_activate();
//     }
// }

// TERAKHIT COMMIT ===================================================================================================

void splash_screen(){
    framebuffer_clear();

    framebuffer_write(3, 28, ' ', 0, 0);
    framebuffer_write(3, 29, ' ', 0, 0);
    framebuffer_write(3, 30, ' ', 0, 0);
    framebuffer_write(3, 31, ' ', 0, 0);
    framebuffer_write(3, 32, ' ', 0, 0);
    framebuffer_write(3, 33, ' ', 0, 0);
    framebuffer_write(3, 34, ' ', 0, 0);
    framebuffer_write(3, 35, ' ', 0, 0);
    framebuffer_write(3, 36, ' ', 0, 4);
    framebuffer_write(3, 37, ' ', 0, 12);
    framebuffer_write(3, 38, ' ', 0, 12);
    framebuffer_write(3, 39, ' ', 0, 1);
    framebuffer_write(3, 40, ' ', 0, 13);
    framebuffer_write(3, 41, ' ', 0, 13);
    framebuffer_write(3, 42, ' ', 0, 12);
    framebuffer_write(3, 43, ' ', 0, 0);
    framebuffer_write(3, 44, ' ', 0, 0);
    framebuffer_write(3, 45, ' ', 0, 0);
    framebuffer_write(3, 46, ' ', 0, 0);
    framebuffer_write(3, 47, ' ', 0, 0);
    framebuffer_write(4, 28, ' ', 0, 0);
    framebuffer_write(4, 29, ' ', 0, 0);
    framebuffer_write(4, 30, ' ', 0, 0);
    framebuffer_write(4, 31, ' ', 0, 0);
    framebuffer_write(4, 32, ' ', 0, 0);
    framebuffer_write(4, 33, ' ', 0, 0);
    framebuffer_write(4, 34, ' ', 0, 0);
    framebuffer_write(4, 35, ' ', 0, 12);
    framebuffer_write(4, 36, ' ', 0, 12);
    framebuffer_write(4, 37, ' ', 0, 1);
    framebuffer_write(4, 38, ' ', 0, 12);
    framebuffer_write(4, 39, ' ', 0, 12);
    framebuffer_write(4, 40, ' ', 0, 12);
    framebuffer_write(4, 41, ' ', 0, 7);
    framebuffer_write(4, 42, ' ', 0, 12);
    framebuffer_write(4, 43, ' ', 0, 4);
    framebuffer_write(4, 44, ' ', 0, 0);
    framebuffer_write(4, 45, ' ', 0, 0);
    framebuffer_write(4, 46, ' ', 0, 0);
    framebuffer_write(4, 47, ' ', 0, 0);
    framebuffer_write(5, 28, ' ', 0, 0);
    framebuffer_write(5, 29, ' ', 0, 0);
    framebuffer_write(5, 30, ' ', 0, 0);
    framebuffer_write(5, 31, ' ', 0, 0);
    framebuffer_write(5, 32, ' ', 0, 0);
    framebuffer_write(5, 33, ' ', 0, 0);
    framebuffer_write(5, 34, ' ', 0, 12);
    framebuffer_write(5, 35, ' ', 0, 12);
    framebuffer_write(5, 36, ' ', 0, 4);
    framebuffer_write(5, 37, ' ', 0, 12);
    framebuffer_write(5, 38, ' ', 0, 13);
    framebuffer_write(5, 39, ' ', 0, 7);
    framebuffer_write(5, 40, ' ', 0, 7);
    framebuffer_write(5, 41, ' ', 0, 8);
    framebuffer_write(5, 42, ' ', 0, 7);
    framebuffer_write(5, 43, ' ', 0, 7);
    framebuffer_write(5, 44, ' ', 0, 0);
    framebuffer_write(5, 45, ' ', 0, 0);
    framebuffer_write(5, 46, ' ', 0, 0);
    framebuffer_write(5, 47, ' ', 0, 0);
    framebuffer_write(6, 28, ' ', 0, 0);
    framebuffer_write(6, 29, ' ', 0, 0);
    framebuffer_write(6, 30, ' ', 0, 0);
    framebuffer_write(6, 31, ' ', 0, 15);
    framebuffer_write(6, 32, ' ', 0, 0);
    framebuffer_write(6, 33, ' ', 0, 4);
    framebuffer_write(6, 34, ' ', 0, 12);
    framebuffer_write(6, 35, ' ', 0, 13);
    framebuffer_write(6, 36, ' ', 0, 12);
    framebuffer_write(6, 37, ' ', 0, 7);
    framebuffer_write(6, 38, ' ', 0, 13);
    framebuffer_write(6, 39, ' ', 0, 12);
    framebuffer_write(6, 40, ' ', 0, 7);
    framebuffer_write(6, 41, ' ', 0, 12);
    framebuffer_write(6, 42, ' ', 0, 7);
    framebuffer_write(6, 43, ' ', 0, 7);
    framebuffer_write(6, 44, ' ', 0, 7);
    framebuffer_write(6, 45, ' ', 0, 7);
    framebuffer_write(6, 46, ' ', 0, 7);
    framebuffer_write(6, 47, ' ', 0, 0);
    framebuffer_write(7, 28, ' ', 0, 7);
    framebuffer_write(7, 29, ' ', 0, 8);
    framebuffer_write(7, 30, ' ', 0, 10);
    framebuffer_write(7, 31, ' ', 0, 10);
    framebuffer_write(7, 32, ' ', 0, 10);
    framebuffer_write(7, 33, ' ', 0, 13);
    framebuffer_write(7, 34, ' ', 0, 4);
    framebuffer_write(7, 35, ' ', 0, 12);
    framebuffer_write(7, 36, ' ', 0, 1);
    framebuffer_write(7, 37, ' ', 0, 1);
    framebuffer_write(7, 38, ' ', 0, 12);
    framebuffer_write(7, 39, ' ', 0, 7);
    framebuffer_write(7, 40, ' ', 0, 12);
    framebuffer_write(7, 41, ' ', 0, 12);
    framebuffer_write(7, 42, ' ', 0, 12);
    framebuffer_write(7, 43, ' ', 0, 12);
    framebuffer_write(7, 44, ' ', 0, 8);
    framebuffer_write(7, 45, ' ', 0, 7);
    framebuffer_write(7, 46, ' ', 0, 8);
    framebuffer_write(7, 47, ' ', 0, 8);
    framebuffer_write(8, 28, ' ', 0, 15);
    framebuffer_write(8, 29, ' ', 0, 7);
    framebuffer_write(8, 30, ' ', 0, 7);
    framebuffer_write(8, 31, ' ', 0, 8);
    framebuffer_write(8, 32, ' ', 0, 7);
    framebuffer_write(8, 33, ' ', 0, 0);
    framebuffer_write(8, 34, ' ', 0, 0);
    framebuffer_write(8, 35, ' ', 0, 15);
    framebuffer_write(8, 36, ' ', 0, 15);
    framebuffer_write(8, 37, ' ', 0, 12);
    framebuffer_write(8, 38, ' ', 0, 15);
    framebuffer_write(8, 39, ' ', 0, 7);
    framebuffer_write(8, 40, ' ', 0, 7);
    framebuffer_write(8, 41, ' ', 0, 12);
    framebuffer_write(8, 42, ' ', 0, 4);
    framebuffer_write(8, 43, ' ', 0, 4);
    framebuffer_write(8, 44, ' ', 0, 15);
    framebuffer_write(8, 45, ' ', 0, 0);
    framebuffer_write(8, 46, ' ', 0, 0);
    framebuffer_write(8, 47, ' ', 0, 0);
    framebuffer_write(9, 28, ' ', 0, 0);
    framebuffer_write(9, 29, ' ', 0, 0);
    framebuffer_write(9, 30, ' ', 0, 12);
    framebuffer_write(9, 31, ' ', 0, 15);
    framebuffer_write(9, 32, ' ', 0, 6);
    framebuffer_write(9, 33, ' ', 0, 6);
    framebuffer_write(9, 34, ' ', 0, 15);
    framebuffer_write(9, 35, ' ', 0, 6);
    framebuffer_write(9, 36, ' ', 0, 6);
    framebuffer_write(9, 37, ' ', 0, 2);
    framebuffer_write(9, 38, ' ', 0, 2);
    framebuffer_write(9, 39, ' ', 0, 2);
    framebuffer_write(9, 40, ' ', 0, 2);
    framebuffer_write(9, 41, ' ', 0, 6);
    framebuffer_write(9, 42, ' ', 0, 0);
    framebuffer_write(9, 43, ' ', 0, 0);
    framebuffer_write(9, 44, ' ', 0, 0);
    framebuffer_write(9, 45, ' ', 0, 0);
    framebuffer_write(9, 46, ' ', 0, 0);
    framebuffer_write(9, 47, ' ', 0, 0);
    framebuffer_write(10, 28, ' ', 0, 4);
    framebuffer_write(10, 29, ' ', 0, 12);
    framebuffer_write(10, 30, ' ', 0, 12);
    framebuffer_write(10, 31, ' ', 0, 6);
    framebuffer_write(10, 32, ' ', 0, 6);
    framebuffer_write(10, 33, ' ', 0, 6);
    framebuffer_write(10, 34, ' ', 0, 6);
    framebuffer_write(10, 35, ' ', 0, 6);
    framebuffer_write(10, 36, ' ', 0, 6);
    framebuffer_write(10, 37, ' ', 0, 6);
    framebuffer_write(10, 38, ' ', 0, 6);
    framebuffer_write(10, 39, ' ', 0, 2);
    framebuffer_write(10, 40, ' ', 0, 2);
    framebuffer_write(10, 41, ' ', 0, 2);
    framebuffer_write(10, 42, ' ', 0, 6);
    framebuffer_write(10, 43, ' ', 0, 0);
    framebuffer_write(10, 44, ' ', 0, 12);
    framebuffer_write(10, 45, ' ', 0, 4);
    framebuffer_write(10, 46, ' ', 0, 0);
    framebuffer_write(10, 47, ' ', 0, 0);
    framebuffer_write(11, 28, ' ', 0, 0);
    framebuffer_write(11, 29, ' ', 0, 0);
    framebuffer_write(11, 30, ' ', 0, 0);
    framebuffer_write(11, 31, ' ', 0, 0);
    framebuffer_write(11, 32, ' ', 0, 0);
    framebuffer_write(11, 33, ' ', 0, 0);
    framebuffer_write(11, 34, ' ', 0, 0);
    framebuffer_write(11, 35, ' ', 0, 0);
    framebuffer_write(11, 36, ' ', 0, 0);
    framebuffer_write(11, 37, ' ', 0, 0);
    framebuffer_write(11, 38, ' ', 0, 0);
    framebuffer_write(11, 39, ' ', 0, 6);
    framebuffer_write(11, 40, ' ', 0, 6);
    framebuffer_write(11, 41, ' ', 0, 2);
    framebuffer_write(11, 42, ' ', 0, 15);
    framebuffer_write(11, 43, ' ', 0, 12);
    framebuffer_write(11, 44, ' ', 0, 12);
    framebuffer_write(11, 45, ' ', 0, 12);
    framebuffer_write(11, 46, ' ', 0, 0);
    framebuffer_write(11, 47, ' ', 0, 0);
    framebuffer_write(12, 28, ' ', 0, 0);
    framebuffer_write(12, 29, ' ', 0, 0);
    framebuffer_write(12, 30, ' ', 0, 0);
    framebuffer_write(12, 31, ' ', 0, 0);
    framebuffer_write(12, 32, ' ', 0, 0);
    framebuffer_write(12, 33, ' ', 0, 0);
    framebuffer_write(12, 34, ' ', 0, 0);
    framebuffer_write(12, 35, ' ', 0, 0);
    framebuffer_write(12, 36, ' ', 0, 0);
    framebuffer_write(12, 37, ' ', 0, 0);
    framebuffer_write(12, 38, ' ', 0, 0);
    framebuffer_write(12, 39, ' ', 0, 0);
    framebuffer_write(12, 40, ' ', 0, 6);
    framebuffer_write(12, 41, ' ', 0, 15);
    framebuffer_write(12, 42, ' ', 0, 4);
    framebuffer_write(12, 43, ' ', 0, 0);
    framebuffer_write(12, 44, ' ', 0, 0);
    framebuffer_write(12, 45, ' ', 0, 0);
    framebuffer_write(12, 46, ' ', 0, 0);
    framebuffer_write(12, 47, ' ', 0, 0);

    for(int i = 15 ; i < 16 ; i++){
        for (int j = 26 ; j < 51 ; j++){
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
            framebuffer_write(i, j, ' ', 0, 0xF);
        }
    }

    // int index = 29;
    // framebuffer_write(16,index,'L',0xF,0);
    // framebuffer_write(16,index + 2,'O',0xF,0);
    // framebuffer_write(16,index + 4,'A',0xF,0);
    // framebuffer_write(16,index + 6,'D',0xF,0);
    // framebuffer_write(16,index + 8,'I',0xF,0);
    // framebuffer_write(16,index + 10,'N',0xF,0);
    // framebuffer_write(16,index + 12,'G',0xF,0);

    framebuffer_write(18,29,' ',0xF,0);
    framebuffer_write(18,30,'s',0xF,0);
    framebuffer_write(18,31,'O',0xF,0);
    framebuffer_write(18,32,'S',0xF,0);
    framebuffer_write(18,33,'i',0xF,0);
    framebuffer_write(18,34,'s',0xF,0);
    framebuffer_write(18,35,' ',0xF,0);
    framebuffer_write(18,36,'(',0xF,0);
    framebuffer_write(18,37,'I',0xF,0);
    framebuffer_write(18,38,'F',0xF,0);
    framebuffer_write(18,39,'2',0xF,0);
    framebuffer_write(18,40,'2',0xF,0);
    framebuffer_write(18,41,'3',0xF,0);
    framebuffer_write(18,42,'0',0xF,0);
    framebuffer_write(18,43,')',0xF,0);

    framebuffer_write(20,30,'K',0xF,0);
    framebuffer_write(20,31,'i',0xF,0);
    framebuffer_write(20,32,'e',0xF,0);
    framebuffer_write(20,33,'l',0xF,0);
    framebuffer_write(20,34,' ',0xF,0);
    framebuffer_write(20,35,'-',0xF,0);
    framebuffer_write(20,36,' ',0xF,0);
    framebuffer_write(20,37,'1',0xF,0);
    framebuffer_write(20,38,'3',0xF,0);
    framebuffer_write(20,39,'5',0xF,0);
    framebuffer_write(20,40,'2',0xF,0);
    framebuffer_write(20,41,'2',0xF,0);
    framebuffer_write(20,42,'0',0xF,0);
    framebuffer_write(20,43,'2',0xF,0);
    framebuffer_write(20,44,'9',0xF,0);

    framebuffer_write(21,29,'D',0xF,0);
    framebuffer_write(21,30,'a',0xF,0);
    framebuffer_write(21,31,'n',0xF,0);
    framebuffer_write(21,32,'i',0xF,0);
    framebuffer_write(21,33,'e',0xF,0);
    framebuffer_write(21,34,'l',0xF,0);
    framebuffer_write(21,35,' ',0xF,0);
    framebuffer_write(21,36,'-',0xF,0);
    framebuffer_write(21,37,' ',0xF,0);
    framebuffer_write(21,38,'1',0xF,0);
    framebuffer_write(21,39,'3',0xF,0);
    framebuffer_write(21,40,'5',0xF,0);
    framebuffer_write(21,41,'2',0xF,0);
    framebuffer_write(21,42,'2',0xF,0);
    framebuffer_write(21,43,'0',0xF,0);
    framebuffer_write(21,44,'4',0xF,0);
    framebuffer_write(21,45,'3',0xF,0);

    framebuffer_write(22,30,'W',0xF,0);
    framebuffer_write(22,31,'i',0xF,0);
    framebuffer_write(22,32,'g',0xF,0);
    framebuffer_write(22,33,'a',0xF,0);
    framebuffer_write(22,34,' ',0xF,0);
    framebuffer_write(22,35,'-',0xF,0);
    framebuffer_write(22,36,' ',0xF,0);
    framebuffer_write(22,37,'1',0xF,0);
    framebuffer_write(22,38,'3',0xF,0);
    framebuffer_write(22,39,'5',0xF,0);
    framebuffer_write(22,40,'2',0xF,0);
    framebuffer_write(22,41,'2',0xF,0);
    framebuffer_write(22,42,'0',0xF,0);
    framebuffer_write(22,43,'5',0xF,0);
    framebuffer_write(22,44,'3',0xF,0);

    framebuffer_write(23,29,'M',0xF,0);
    framebuffer_write(23,30,'a',0xF,0);
    framebuffer_write(23,31,'t',0xF,0);
    framebuffer_write(23,32,'t',0xF,0);
    framebuffer_write(23,33,'h',0xF,0);
    framebuffer_write(23,34,'e',0xF,0);
    framebuffer_write(23,35,'w',0xF,0);
    framebuffer_write(23,36,' ',0xF,0);
    framebuffer_write(23,37,'-',0xF,0);
    framebuffer_write(23,38,' ',0xF,0);
    framebuffer_write(23,39,'1',0xF,0);
    framebuffer_write(23,40,'3',0xF,0);
    framebuffer_write(23,41,'5',0xF,0);
    framebuffer_write(23,42,'2',0xF,0);
    framebuffer_write(23,43,'2',0xF,0);
    framebuffer_write(23,44,'0',0xF,0);
    framebuffer_write(23,45,'9',0xF,0);
    framebuffer_write(23,46,'3',0xF,0);

    framebuffer_set_cursor(3, 42);
    // int temp = 1;
    int delay = 10000;
    for (int j = 26 ; j < 51 ; j++){
        for(int n = 0 ; n < delay ; n++){
            for(int i = 15 ; i < 16 ; i++){
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                framebuffer_write(i, j, ' ', 0, 0x2);
                // if (n % delay == 0){
                //     if (temp == 1){
                //         framebuffer_write(16,index + 14,'.',0xF,0);
                //         temp ++;
                //     }
                //     else if (temp == 2){
                //         framebuffer_write(16,index + 16,'.',0xF,0);
                //         temp ++;
                //     }
                //     else if (temp == 3){
                //         framebuffer_write(16,index + 18,'.',0xF,0);
                //         temp ++;
                //     }
                //     else{
                //         framebuffer_write(16,index + 14,' ',0xF,0);
                //         framebuffer_write(16,index + 16,' ',0xF,0);
                //         framebuffer_write(16,index + 18,' ',0xF,0);
                //         temp = 1;
                //     }
                // }
            }
        }
    }
}

void kernel_setup(void) {
    load_gdt(&_gdt_gdtr);
    pic_remap();
    initialize_idt();
    activate_keyboard_interrupt();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);
    initialize_filesystem_fat32();
    gdt_install_tss();
    set_tss_register();
    splash_screen();
    framebuffer_clear();
    framebuffer_set_cursor(0, 0);


    // Write shell into memory
    struct FAT32DriverRequest request = {
        .buf                   = (uint8_t*) 0,
        .name                  = "shell",
        .ext                   = "\0\0\0",
        .parent_cluster_number = ROOT_CLUSTER_NUMBER,
        .buffer_size           = 0x100000,
    };
    uint8_t arr1[CLUSTER_SIZE+1] = {
    'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
    'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
    'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
    'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
    '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-','\n','2', '0', '2', '4', '\n',
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
    write(requestWRITE1);

    // Set TSS.esp0 for interprivilege interrupt
    set_tss_kernel_current_stack();

    // Create init process and execute it
    process_create_user_process(request);
    scheduler_init();
    scheduler_switch_to_next_process();
}


// void kernel_setup(void) {
//     load_gdt(&_gdt_gdtr);
//     pic_remap();
//     initialize_idt();
//     activate_keyboard_interrupt();
//     framebuffer_clear();
//     framebuffer_set_cursor(0, 0);
//     initialize_filesystem_fat32();

//     uint8_t arr1[CLUSTER_SIZE+1] = {
//     'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
//     'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
//     'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
//     'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
//     '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
//     [CLUSTER_SIZE-1] = '1',
//     [CLUSTER_SIZE] = '2',
//     };
//     struct FAT32DriverRequest requestWRITE1 = {
//         .buf                   = arr1,
//         .name                  = "file1",
//         .ext                   = "",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = CLUSTER_SIZE+1,
//     } ;

//     framebuffer_write(1,1,write(requestWRITE1)+'0',0xF,0);
//     // char ar[14];
//     // request.buf = ar;
//     // framebuffer_write(0,2,read(request)+'0',0xF,0);
//     uint8_t arr2[CLUSTER_SIZE] = {
//     'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
//     'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
//     'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
//     'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
//     '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
//     [CLUSTER_SIZE-2] = 'O',
//     [CLUSTER_SIZE-1] = 'k',
//     };
//     struct FAT32DriverRequest requestWRITE2 = {
//         .buf                   = arr2,
//         .name                  = "file2",
//         .ext                   = "",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = CLUSTER_SIZE,
//     } ;
//     framebuffer_write(1,2,write(requestWRITE2)+'0',0xF,0);
//     uint8_t arr3[CLUSTER_SIZE] = {
//     'C', 'o', 'u', 'r', 's', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  ' ',
//     'D', 'e', 's', 'i', 'g', 'n', 'e', 'd', ' ', 'b', 'y', ' ', ' ', ' ', ' ',  ' ',
//     'L', 'a', 'b', ' ', 'S', 'i', 's', 't', 'e', 'r', ' ', 'I', 'T', 'B', ' ',  ' ',
//     'M', 'a', 'd', 'e', ' ', 'w', 'i', 't', 'h', ' ', '<', '3', ' ', ' ', ' ',  ' ',
//     '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '-', '2', '0', '2', '4', '\n',
//     [CLUSTER_SIZE-2] = 'O',
//     [CLUSTER_SIZE-1] = 'k',
//     };
//     struct FAT32DriverRequest requestWRITE3 = {
//         .buf                   = arr3,
//         .name                  = "file3",
//         .ext                   = "",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = CLUSTER_SIZE,
//     } ;
//     framebuffer_write(1,3,write(requestWRITE3)+'0',0xF,0);
//     struct FAT32DriverRequest requestREAD1 = {
//         .buf                   = NULL,
//         .name                  = "file1",
//         .ext                   = "",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = CLUSTER_SIZE+1,
//     } ;
//     framebuffer_write(1,4,read(requestREAD1)+'0',0xF,0);
//     struct FAT32DriverRequest requestDELETE = {
//         .buf                   = NULL,
//         .name                  = "file1",
//         .ext                   = "",
//         .parent_cluster_number = ROOT_CLUSTER_NUMBER,
//         .buffer_size           = 0,
//     } ;
//     framebuffer_write(1,5,delete(requestDELETE)+'0',0xF,0);
//     framebuffer_write(1,6,read(requestREAD1)+'0',0xF,0);
//     while(true){
//         keyboard_state_activate();
//     }
// }

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
