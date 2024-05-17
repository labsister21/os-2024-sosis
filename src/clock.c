#include "header/clock.h"
#include "header/cpu/portio.h"
#include "header/driver/framebuffer.h"
void sleep(int seconds) {
    for (int i = 0; i < seconds * 1000000; i++) {
        asm volatile("nop");
    }
}

void syscaLL(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx) {
    __asm__ volatile("mov %0, %%ebx" : /* <Empty> */ : "r"(ebx));
    __asm__ volatile("mov %0, %%ecx" : /* <Empty> */ : "r"(ecx));
    __asm__ volatile("mov %0, %%edx" : /* <Empty> */ : "r"(edx));
    __asm__ volatile("mov %0, %%eax" : /* <Empty> */ : "r"(eax));
    // Note : gcc usually use %eax as intermediate register,
    //        so it need to be the last one to mov
    __asm__ volatile("int $0x30");
}

int main() {
    uint8_t time[3];
    char time_str[9];
    while (1) {
        syscaLL(16, (uint32_t)time, 0, 0);
        // uint32_t total_seconds = (time[0]*60*90)+(time[1]*90)+(time[2]);
        // uint32_t equiv_seconds = total_seconds-38515;
        // time[0] = (equiv_seconds/3600);
        // time[1] = (equiv_seconds%3600)/60;
        // time[2] = (equiv_seconds%3600)%60;
        time_str[0] = '0' + (time[0] / 10);
        time_str[1] = '0' + (time[0] % 10);
        time_str[2] = ':';
        time_str[3] = '0' + (time[1] / 10);
        time_str[4] = '0' + (time[1] % 10);
        time_str[5] = ':';
        time_str[6] = '0' + (time[2] / 10);
        time_str[7] = '0' + (time[2] % 10);
        time_str[8] = '\0';
        syscaLL(23,(uint32_t)time_str,0,0);
        sleep(1); 
    }
}