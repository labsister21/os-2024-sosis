#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/driver/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"


void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg) {
    /*
    I.S. Menerima :
            row = baris dari karakter c
            col = kolom dari karakter c
            c = karakter yang akan ditulis
            fg = warna karakter
            bg = warna background
    F.S. Menulis karakter c ke framebuffer dengan warna fg dan warna background bg
    */
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
}


void framebuffer_clear(void) {
    /*
    I.S. -
    F.S. Menghapus semua karakter yang ada di framebuffer
    */
    memset(FRAMEBUFFER_MEMORY_OFFSET, 0X00, 80 * 25 * 2);
}
//     uint16_t *fb = (uint16_t *) FRAMEBUFFER_MEMORY_OFFSET; // Mengambil alamat memory framebuffer
//     // // Looping untuk menghapus semua karakter yang ada di framebuffer
//     for (uint16_t i = 0; i < 80 * 25; i++) {
//         fb[i + 1] = (0x07 << 8) | 0x00;
//         // fb[i + 1] = 0x07;
//     }
// }