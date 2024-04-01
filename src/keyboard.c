#include "header/driver/keyboard.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

const char keyboard_scancode_1_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', '\n',   0,  'a',  's',
    'd',  'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0, '\\',  'z', 'x',  'c',  'v',
    'b',  'n', 'm', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0, '-',    0,    0,   0,  '+',    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,

      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
      0,    0,   0,   0,   0,   0,   0,   0,    0,   0,   0,    0,    0,   0,    0,    0,
};

// #define EXT_SCANCODE_UP        0x48
// #define EXT_SCANCODE_DOWN      0x50
// #define EXT_SCANCODE_LEFT      0x4B
// #define EXT_SCANCODE_RIGHT     0x4D

// #define KEYBOARD_DATA_PORT     0x60
// #define EXTENDED_SCANCODE_BYTE 0xE0

// /**
//  * keyboard_scancode_1_to_ascii_map[256], Convert scancode values that correspond to ASCII printables
//  * How to use this array: ascii_char = k[scancode]
//  * 
//  * By default, QEMU using scancode set 1 (from empirical testing)
//  */
// extern const char keyboard_scancode_1_to_ascii_map[256];

/**
 * KeyboardDriverState - Contain all driver states
 * 
 * @param read_extended_mode Optional, can be used for signaling next read is extended scancode (ex. arrow keys)
 * @param keyboard_input_on  Indicate whether keyboard ISR is activated or not
 * @param keyboard_buffer    Storing keyboard input values in ASCII
 */
// struct KeyboardDriverState {
//     bool read_extended_mode;
//     bool keyboard_input_on;
//     char keyboard_buffer;
// } __attribute((packed));

static struct KeyboardDriverState keyboard_state = {0};

/* -- Driver Interfaces -- */

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void){
    keyboard_state.keyboard_input_on = true;
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void){
    keyboard_state.keyboard_input_on = false;
}

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf){
    *buf = keyboard_state.keyboard_buffer;
    keyboard_state.keyboard_buffer = '\0';
}

/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void){
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    if(keyboard_state.keyboard_input_on == true){
        if(scancode == EXTENDED_SCANCODE_BYTE){
            keyboard_state.read_extended_mode = true;
            return;
        }
        if(keyboard_state.read_extended_mode){
            scancode |= 0xE0;
            keyboard_state.read_extended_mode = false;
        }
        char ascii_char = keyboard_scancode_1_to_ascii_map[scancode];
        keyboard_state.keyboard_buffer = ascii_char;
    }
}