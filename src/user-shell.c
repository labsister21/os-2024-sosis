#include <stdint.h>
#include "header/filesystem/fat32.h"

void* memcpy2(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;  
}

void syscall(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

void puts(char* val, uint32_t len,uint32_t color)
{
    syscall(6, (uint32_t) val, len, color);
}

void commandLI(char *current_dir)
{
    puts("s0sis@OS-IF2230:", (uint32_t) 21, (uint32_t) 0xA);
    puts(current_dir, (uint32_t) 255, (uint32_t) 0x9);
    puts("$ ", (uint32_t) 3, (uint32_t) 0xF);
}

void remove(char* name, char* ext, uint32_t parent_number)
{
    int8_t ret;

    struct ClusterBuffer data_buf;
    struct FAT32DriverRequest request = {
        .buf = &data_buf,
        .name = "\0\0\0\0\0\0\0",
        .ext = "\0\0",
        .buffer_size = 0,
        .parent_cluster_number = parent_number
    };

    memcpy2(request.name, name, 8);
    memcpy2(request.ext, ext, 3);
    syscall(3, (uint32_t)&request, (uint32_t)&ret, 0);
}

int main(void) {
    char dir[100] = "Root/";
    char name[100] = "s0sis@OS-IF2230:";
    char command[100] = "";
    int idx = 0;
    char *buf = NULL;

    while(true){
        syscall(7,0,0,0);
        puts(name, 16, 0xA);
        puts(dir, 5 , 0x9);
        puts("$ ", 3, 0xF);
        // syscall(6,(uint32_t)dir,10,0xF);
        // commandLI(dir);
        syscall(4, (uint32_t) command, 256, 0);

        while(*buf!='\n'){
            syscall(4,(uint32_t)buf,0,0);
            if(*buf!='\0' && *buf!='\n'){
                command[idx]=*buf;
                idx++;
                syscall(5,(uint32_t)buf,0xF,0);
            }
        }
        syscall(6,(uint32_t)command,idx,0xF);
        idx = 0;
        *buf='\0';
    }

    syscall(5,(uint32_t)command[0],0xF,0);


    return 0;
}