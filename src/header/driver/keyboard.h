#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/cpu/interrupt.h"

#define EXT_SCANCODE_UP        0x48
#define EXT_SCANCODE_DOWN      0x50
#define EXT_SCANCODE_LEFT      0x4B
#define EXT_SCANCODE_RIGHT     0x4D

#define KEYBOARD_DATA_PORT     0x60
#define EXTENDED_SCANCODE_BYTE 0xE0

#define CAPSLOCK_SCANCODE      0x3A
#define LEFT_SHIFT_SCANCODE    0x2A
#define RIGHT_SHIFT_SCANCODE   0x36
#define LEFT_SHIFT_RELEASE_SCANCODE 0xAA
#define RIGHT_SHIFT_RELEASE_SCANCODE 0xB6
#define CTRL_SCANCODE          0x1D
#define CTRL_RELEASE_SCANCODE   0x9D


/**
 * keyboard_scancode_1_to_ascii_map[256], Convert scancode values that correspond to ASCII printables
 * How to use this array: ascii_char = k[scancode]
 * 
 * By default, QEMU using scancode set 1 (from empirical testing)
 */
extern const char keyboard_scancode_1_to_ascii_map[256];
extern const char keyboard_scancode_caps_to_ascii_map[256];
extern const char keyboard_scancode_shift_to_ascii_map[256];

#define MAX_COMMAND_LENGTH 100
extern char display_command_buffer;
extern bool display_command_pending;

#define MAX_HISTORY 10
extern char command_history; // Array to store history
extern int current_history_index;         // Index of the current command in history
extern int total_history_count;            // Total commands in history
extern int viewing_history_index;         // Index for navigating history


/**
 * KeyboardDriverState - Contain all driver states
 * 
 * @param read_extended_mode Optional, can be used for signaling next read is extended scancode (ex. arrow keys)
 * @param keyboard_input_on  Indicate whether keyboard ISR is activated or not
 * @param keyboard_buffer    Storing keyboard input values in ASCII
 */
struct KeyboardDriverState {
    bool read_extended_mode;
    bool keyboard_input_on;
    char keyboard_buffer;
    bool capslock_on;
    bool shift_on;
    bool ctrl_on;    
} __attribute((packed));


void add_command(char c);

void get_command(char commandShell[100],int *idxShell);

void clear_command();

void delete_command();

/* -- Driver Interfaces -- */

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void);

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void);

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf);


/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void);

#endif