#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/driver/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

uint8_t cursor_col = 0;
uint8_t cursor_row = 0; 

void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= TERMINAL_HEIGHT) {
            framebuffer_auto_down();
            cursor_row = TERMINAL_HEIGHT - 1;
        }
        framebuffer_set_cursor(cursor_row, cursor_col);
    } else {
        if (col >= TERMINAL_WIDTH) {
            col = 0;
            row++;
        }

        if (row >= TERMINAL_HEIGHT) {
            row = TERMINAL_HEIGHT - 1;
            framebuffer_auto_down();
        }

        size_t loc = (row * 80 + col) * 2; // Menghitung posisi karakter c
        uint8_t color = (bg << 4) | (fg & 0X0F); // Memberi warna dan warna background pada karakter c

        FRAMEBUFFER_MEMORY_OFFSET[loc] = c;
        FRAMEBUFFER_MEMORY_OFFSET[loc + 1] = color;
    
        if (cursor_col >= TERMINAL_WIDTH) {
            cursor_col = 0;
            cursor_row++;
            if (cursor_row >= TERMINAL_HEIGHT) {
                framebuffer_auto_down();
                cursor_row = TERMINAL_HEIGHT - 1;
            }
        }

        framebuffer_set_cursor(cursor_row, cursor_col);
    }
}

void write_exact_loc(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg){
    size_t loc = (row * 80 + col) * 2; // Menghitung posisi karakter c
    uint8_t color = (bg << 4) | (fg & 0X0F); // Memberi warna dan warna background pada karakter c
    FRAMEBUFFER_MEMORY_OFFSET[loc] = c;
    FRAMEBUFFER_MEMORY_OFFSET[loc + 1] = color;
}


void framebuffer_set_cursor(uint8_t r, uint8_t c) {
    /*
    I.S. Menerima r sebagai baris cursor dan c sebagai kolom cursor
    F.S. Mengubah posisi cursor ke baris r dan kolom c
    */
    uint16_t loc = r * 80 + c; // Menghitung posisi cursor
    // Mengirimkan posisi cursor ke port CURSOR_PORT_CMD dan CURSOR_PORT_DATA
    // Atur byte untuk low cursor
    out(CURSOR_PORT_CMD, 0x0F);
    out(CURSOR_PORT_DATA, (uint8_t) (loc & 0xFF));
    // Atur byte untuk high cursor
    out(CURSOR_PORT_CMD, 0x0E);
    out(CURSOR_PORT_DATA, (uint8_t) ((loc >> 8) & 0xFF));
    cursor_row = r;
    cursor_col = c;
}

uint16_t framebuffer_get_cursor(void) {
    /*
    I.S. -
    F.S. Mengembalikan posisi cursor
    */
    uint16_t pos = 0;
    // Mengambil posisi cursor dari port CURSOR_PORT_CMD dan CURSOR_PORT_DATA
    // Mengambil byte low cursor
    out(CURSOR_PORT_CMD, 0x0F);
    pos |= in(CURSOR_PORT_DATA);
    // Mengambil byte high cursor
    out(CURSOR_PORT_CMD, 0x0E);
    pos = in(CURSOR_PORT_DATA) << 8;
    return pos;
}

void framebuffer_clear(void) {
    /*
    I.S. -
    F.S. Menghapus semua karakter yang ada di framebuffer
    */
    memset(FRAMEBUFFER_MEMORY_OFFSET, 0X00, 80 * 25 * 2);
    cursor_row = 0;
    cursor_col = 0;
    framebuffer_set_cursor(cursor_row, cursor_col);
}

void framebuffer_auto_down(void) {
    memmove(FRAMEBUFFER_MEMORY_OFFSET, FRAMEBUFFER_MEMORY_OFFSET + TERMINAL_WIDTH * 2, sizeof(uint16_t) * TERMINAL_WIDTH * (TERMINAL_HEIGHT - 1));

    for (uint8_t col = 0; col < TERMINAL_WIDTH; col++) {
        framebuffer_write(TERMINAL_HEIGHT - 1, col, ' ', 0x07, 0x00);
    }

    framebuffer_set_cursor(TERMINAL_HEIGHT - 1, 0);
}