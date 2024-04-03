#include "header/driver/keyboard.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"


static bool capslock_on = false;

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

const char keyboard_scancode_caps_to_ascii_map[256] = {
      0, 0x1B, '1', '2', '3', '4', '5', '6',  '7', '8', '9',  '0',  '-', '=', '\b', '\t',
    'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',  'O', 'P', '[',  ']', '\n',   0,  'A',  'S',
    'D',  'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`',   0, '\\',  'Z', 'X',  'C',  'V',
    'B',  'N', 'M', ',', '.', '/',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
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


/**
 * KeyboardDriverState - Contain all driver states
 * 
 * @param read_extended_mode Optional, can be used for signaling next read is extended scancode (ex. arrow keys)
 * @param keyboard_input_on  Indicate whether keyboard ISR is activated or not
 * @param keyboard_buffer    Storing keyboard input values in ASCII
 */
struct KeyboardDriverState keyboard_state = {
    .read_extended_mode = false,
    .keyboard_input_on = false,
    .keyboard_buffer = 0,
};




/* -- Driver Interfaces -- */

// Activate keyboard ISR / start listen keyboard & save to buffer
void keyboard_state_activate(void) {
    keyboard_state.keyboard_input_on = true;
}

// Deactivate keyboard ISR / stop listening keyboard interrupt
void keyboard_state_deactivate(void) {
    keyboard_state.keyboard_input_on = false;

}

// Get keyboard buffer value and flush the buffer - @param buf Pointer to char buffer
void get_keyboard_buffer(char *buf) {
    *buf = keyboard_state.keyboard_buffer;
    keyboard_state.keyboard_buffer = '\0';
}


// Define cursor position variables
static uint8_t cursor_col = 0;
static uint8_t cursor_row = 0;


/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void) {
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    if(keyboard_state.keyboard_input_on){
        bool is_break_code = (scancode & 0x80) != 0;
        if (is_break_code) {
            return;
        }

        if (scancode == CAPSLOCK_SCANCODE) {
            capslock_on = !capslock_on;
            return;
        }

        // if(keyboard_state.read_extended_mode){
        if(scancode == EXT_SCANCODE_UP && cursor_row > 0){
            cursor_row--;
            framebuffer_set_cursor(cursor_row, cursor_col);
            return;
        }
        else if(scancode == EXT_SCANCODE_RIGHT && !(cursor_row == 24 && cursor_col == 79)){
            if(cursor_col == 79){
                cursor_row++;
                cursor_col=0;
            }
            else{
                cursor_col++;
            }
            framebuffer_set_cursor(cursor_row, cursor_col);
            return;
        }
        else if(scancode == EXT_SCANCODE_LEFT && !(cursor_row == 0 && cursor_col == 0)){
            if(cursor_col == 0){
                cursor_row--;
                cursor_col = 79;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
            else{
                cursor_col--;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
            return;
        }
        else if(scancode == EXT_SCANCODE_DOWN && cursor_row < 25){
            cursor_row++;
            framebuffer_set_cursor(cursor_row, cursor_col);
            return;
        }
            // keyboard_state.read_extended_mode = false;
            // return;
        // }

        char ascii_char;
        if (capslock_on) {
            ascii_char = keyboard_scancode_caps_to_ascii_map[scancode];
        } 
        else {
            ascii_char = keyboard_scancode_1_to_ascii_map[scancode];
        }

        if (capslock_on && ascii_char >= 'a' && ascii_char <= 'z') {
            ascii_char -= ('a' - 'A');
        }

        if (ascii_char == '\n') { // Enter
            if (cursor_row < 25) {
                cursor_row++;
                cursor_col = 0;
            }
            framebuffer_set_cursor(cursor_row, cursor_col);
        }
        else if (ascii_char == '\t') { // Tab
            for(int i = 0; i < 4; i++) {
                if (cursor_col > 79) {
                    cursor_col = 0;
                    cursor_row++;
                }
                framebuffer_write(cursor_row, cursor_col, ' ', 0XF, 0);
                cursor_col++;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
        }
        else if (ascii_char == '\b') { // Backspace
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
        else { // Regular char
            if (cursor_col > 79) {
                cursor_col = 0;
                cursor_row++;
            }
            framebuffer_write(cursor_row, cursor_col, ascii_char, 0XF, 0);
            cursor_col++;
            framebuffer_set_cursor(cursor_row, cursor_col);
        }
        framebuffer_write(cursor_row,cursor_col + 1,' ', 0xF, 0x0);
    } 
    else {
        keyboard_state.keyboard_buffer = '\0';   
    }
    pic_ack(IRQ_KEYBOARD);
}
