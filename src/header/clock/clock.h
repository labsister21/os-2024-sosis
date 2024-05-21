#ifndef _CLOCK_H
#define _CLOCK_H

#include <stdint.h>

void sleep(int seconds);

void syscaLL(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx);

#endif