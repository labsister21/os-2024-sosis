#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define FRAMEBUFFER_MEMORY_OFFSET ((uint8_t*) 0xC00B8000)
#define CURSOR_PORT_CMD    0x03D4
#define CURSOR_PORT_DATA   0x03D5

#define TERMINAL_HEIGHT 25
#define TERMINAL_WIDTH 80


/**
 * Terminal framebuffer
 * Resolution: 80x25
 * Starting at FRAMEBUFFER_MEMORY_OFFSET,
 * - Even number memory: Character, 8-bit
 * - Odd number memory:  Character color lower 4-bit, Background color upper 4-bit
*/

extern uint8_t cursor_col;
extern uint8_t cursor_row;


/**
 * Set framebuffer character and color with corresponding parameter values.
 * More details: https://en.wikipedia.org/wiki/BIOS_color_attributes
 *
 * @param row Vertical location (index start 0)
 * @param col Horizontal location (index start 0)
 * @param c   Character
 * @param fg  Foreground / Character color
 * @param bg  Background color
 */
void framebuffer_write(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg);

/**
 * Set cursor to specified location. Row and column starts from 0
 * 
 * @param r row
 * @param c column
*/
void framebuffer_set_cursor(uint8_t r, uint8_t c);

void write_exact_loc(uint8_t row, uint8_t col, char c, uint8_t fg, uint8_t bg);

/**
 * Set all cell in framebuffer character to 0x00 (empty character)
 * and color to 0x07 (gray character & black background)
 * Extra note: It's allowed to use different color palette for this
 *
 */
void framebuffer_clear(void);

void framebuffer_auto_down(void);

uint16_t framebuffer_get_cursor(void);

#endif