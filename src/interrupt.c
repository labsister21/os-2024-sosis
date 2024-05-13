#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/cpu/portio.h"
#include "header/driver/framebuffer.h"
#include "header/filesystem/fat32.h"

#define GDT_KERNEL_DATA_SEGMENT_SELECTOR 0x10

void io_wait(void)
{
    out(0x80, 0);
}

void pic_ack(uint8_t irq)
{
    if (irq >= 8)
    {
        out(PIC2_COMMAND, PIC_ACK);
    }
    out(PIC1_COMMAND, PIC_ACK);
}

void pic_remap(void)
{
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

void putchar(char* c,uint8_t textColor){
    if(*c!='\0')
    {    
        if (*c == '\n') { // Enter
            if (cursor_row < 25) {
                cursor_row++;
                cursor_col = 0;
            }
            framebuffer_set_cursor(cursor_row, cursor_col);
        }
        else if (*c == '\t') { // Tab
            for(int i = 0; i < 4; i++) {
                if (cursor_col > 79) {
                    cursor_col = 0;
                    cursor_row++;
                }
                framebuffer_write(cursor_row, cursor_col, ' ', 0xF, 0);
                cursor_col++;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
        }
        else if (*c == '\b') { // Backspace
            if (cursor_row > 0 && cursor_col == 0) {
                cursor_col = 79;
                cursor_row--;
                framebuffer_write(cursor_row, cursor_col,' ', 0xF, 0x0);
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
            else if (cursor_col > 0) {
                cursor_col--;
                framebuffer_write(cursor_row, cursor_col,' ', 0xF, 0x0);
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
        }
        else if (*c == '\0')
        {
            // does nothing
        }
        else { // Regular char
            if (cursor_col > 79) {
                cursor_col = 0;
                cursor_row++;
            }
            framebuffer_write(cursor_row, cursor_col, *c, textColor, 0);
            cursor_col++;
            framebuffer_set_cursor(cursor_row, cursor_col);
        }
        framebuffer_write(cursor_row,cursor_col + 1,' ', 0xF, 0x0);
    }
}

void puts(char* ebx,uint32_t ecx,uint32_t edx){
    for(int i=0;i<(int)ecx;i++){
        putchar(ebx+i,edx);
    }
}

void printDirPath(int cluster){
    int current_cluster = cluster,parent_cluster;

    struct FAT32DirectoryTable dir_table;
    read_clusters(&dir_table,current_cluster,1);
    parent_cluster = (dir_table.table[1].cluster_high << 16) | dir_table.table[1].cluster_low;

    while(current_cluster != parent_cluster){
        puts(dir_table.table[0].name,8,0xF);
        char slash='/';
        putchar(&slash,0xF);

        current_cluster = parent_cluster;
        read_clusters(&dir_table,current_cluster,1);
        parent_cluster = (dir_table.table[1].cluster_high << 16) | dir_table.table[1].cluster_low;
    }
    char* root = "Root\n";
    puts(root,6,0xF);
}

void printAllFind(char name[8],char ext[3]){
    int result[50],n_res = 0;
    find(name,ext,result,&n_res);

    if(n_res == 0){
        puts("Tidak ditemukan file/folder dengan nama dan ext tersebut!",57,0xF);
        return;
    }

    for(int i=0;i<n_res;i++){
        char no = (i+1)+'0';
        putchar(&no,0xF);
        puts(". ",2,0xF);
        printDirPath(result[i]);
    }
}

void syscall(struct InterruptFrame frame) {
    switch (frame.cpu.general.eax) {
        case 0:
            *((int8_t*) frame.cpu.general.ecx) = read(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 1:
            *((int8_t*) frame.cpu.general.ecx) = read_directory(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 2:
            *((int8_t*) frame.cpu.general.ecx) = write(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 3:
            *((int8_t*) frame.cpu.general.ecx) = delete(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 4:
            // get_keyboard_buffer((char*) frame.cpu.general.ebx);
            keyboard_state_activate();
            get_keyboard_buffer((char*) frame.cpu.general.ebx);
            // memcpy((char*)frame.cpu.general.ebx, buf, cpu.ecx);
            break;
        case 5:
            putchar((char*)frame.cpu.general.ebx,(uint8_t)frame.cpu.general.ecx);
            break;
        case 6:
            puts(
                (char*) frame.cpu.general.ebx, 
                frame.cpu.general.ecx, 
                frame.cpu.general.edx
            );
            break;
        case 7: 
            keyboard_state_activate();
            break;
        case 8:
            framebuffer_clear();
            framebuffer_set_cursor(0,0);
            break;
        case 9:
            printAllFind((char*)frame.cpu.general.ebx,(char*)frame.cpu.general.ecx);
            break;
        case 10:
            read_clusters((void*)frame.cpu.general.ebx,frame.cpu.general.ecx,1);
            break;
        case 11:
            *((int32_t*) frame.cpu.general.ecx) = findCluster(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 99:
            struct FAT32DirectoryTable *dirtable = (struct FAT32DirectoryTable*)frame.cpu.general.ebx;
            read_clusters(dirtable, frame.cpu.general.ecx, 1);
    }
}

void main_interrupt_handler(struct InterruptFrame frame)
{
    switch (frame.int_number)
    {
    case PAGE_FAULT:
        __asm__("hlt");
        break;
    case (PIC1_OFFSET + IRQ_KEYBOARD):
        keyboard_isr();
        break;
    case (SYSCALL):
        syscall(frame);
        break;
    }


    pic_ack(frame.int_number);
}

void activate_keyboard_interrupt(void)
{
    uint8_t mask = in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD);
    out(PIC1_DATA, mask);
}

struct TSSEntry _interrupt_tss_entry = {
    .ss0 = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
    .unused_register = {0},
};

void set_tss_kernel_current_stack(void)
{
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile("mov %%ebp, %0" : "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8;
}
