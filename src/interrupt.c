#include "header/cpu/interrupt.h"
#include "header/cpu/idt.h"
#include "header/driver/keyboard.h"
#include "header/cpu/portio.h"
#include "header/driver/framebuffer.h"
#include "header/filesystem/fat32.h"
#include "header/scheduler/scheduler.h"
#include "header/process/process.h"
#include "header/stdlib/string.h"

void activate_timer_interrupt(void) {
    __asm__ volatile("cli");
    // Setup how often PIT fire
    uint32_t pit_timer_counter_to_fire = PIT_TIMER_COUNTER;
    out(PIT_COMMAND_REGISTER_PIO, PIT_COMMAND_VALUE);
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t) (pit_timer_counter_to_fire & 0xFF));
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t) ((pit_timer_counter_to_fire >> 8) & 0xFF));

    // Activate the interrupt
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_TIMER));
}

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

uint8_t cmos_read(uint8_t reg) {
    out(CMOS_ADDRESS_PORT, reg);
    return in(CMOS_DATA_PORT);
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
            get_keyboard_buffer((char*) frame.cpu.general.ebx);
            if(*(char*)frame.cpu.general.ebx=='\b'){
                delete_command();
            }
            else if(*(char*)frame.cpu.general.ebx!='\0' && *(char*)frame.cpu.general.ebx!='\n' ){
                add_command(*(char*)frame.cpu.general.ebx);
            }
            get_command((char*)frame.cpu.general.ecx,(int*)frame.cpu.general.edx);
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
            process_destroy(1);
            break;
        case 11:
            *((int32_t*) frame.cpu.general.ecx) = findCluster(
                *(struct FAT32DriverRequest*) frame.cpu.general.ebx
            );
            break;
        case 12:
            read_clusters((void*)frame.cpu.general.ebx,frame.cpu.general.ecx,1);
            break;
        case 13:
            *(uint32_t*)frame.cpu.general.ecx = process_create_user_process(*(struct FAT32DriverRequest*) frame.cpu.general.ebx);
            break;
        case 14:
            process_destroy(frame.cpu.general.ebx);
            break;
        case 15:
            getActivePCB((struct ProcessControlBlock*)frame.cpu.general.ebx,(int*)frame.cpu.general.ecx);
            break;
        case 16:
            *((uint8_t*)frame.cpu.general.ebx+0) = cmos_read(0x04);
            *((uint8_t*)frame.cpu.general.ebx+1) = cmos_read(0x02);
            *((uint8_t*)frame.cpu.general.ebx+2) = cmos_read(0x00);
            break;
        case 17:
            clear_command();
            break;
        case 18:
            update_shell_dir(frame.cpu.general.ebx,(char*)frame.cpu.general.ecx,(bool)frame.cpu.general.edx);
            break;
        case 19:
            *(int*)frame.cpu.general.ebx = get_shell_cluster();
            break;
        case 20:
            *(int*) frame.cpu.general.ebx=(cwd_table.table[1].cluster_high<<16)|(cwd_table.table[1].cluster_low);
            break;
        case 21:
            copy_dir((char*)frame.cpu.general.ebx);
            break;
        case 22:
            *(struct FAT32DirectoryTable*)frame.cpu.general.ebx = get_cwd_table();
            break;
        case 23:
            for(int i=0;i<9;i++){
                write_exact_loc(23,70+i,*((char*)frame.cpu.general.ebx+i),0xF,0);
            }
    }
}

void main_interrupt_handler(struct InterruptFrame frame)
{
    pic_ack(frame.int_number);
    switch (frame.int_number)
    {
    case PAGE_FAULT:
        __asm__("hlt");
        break;
    case (PIC1_OFFSET + IRQ_KEYBOARD):
        keyboard_isr();
        break;
    case (PIC1_OFFSET + IRQ_TIMER):
        if(process_manager_state.active_process_count>1){
            struct Context ctx = {
                .cpu = frame.cpu,
                .eflags = frame.int_stack.eflags,
                .eip = frame.int_stack.eip,
            };
            scheduler_save_context_to_current_running_pcb(ctx);
            scheduler_switch_to_next_process();
        }
        break;
    case (SYSCALL):
        syscall(frame);
        break;
    }


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
