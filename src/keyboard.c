#include "header/driver/keyboard.h"
#include "header/driver/framebuffer.h"
#include "header/cpu/portio.h"
#include "header/stdlib/string.h"

static char command[100] = "";
static int idx = 0;

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

const char keyboard_scancode_shift_to_ascii_map[256] = {
      0, 0x1B, '!', '@', '#', '$', '%', '^',  '&', '*', '(',  ')',  '_', '+', '\b', '\t',
    'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',  'O', 'P', '{',  '}', '\n',   0,  'A',  'S',
    'D',  'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',   0, '|',  'Z', 'X',  'C',  'V',
    'B',  'N', 'M', '<', '>', '?',   0, '*',    0, ' ',   0,    0,    0,   0,    0,    0,
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
    .keyboard_buffer = '\0',
    .capslock_on = false,
    .shift_on = false,
    .ctrl_on = false
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

void add_command(char c){
    command[idx] = c;
    idx++;
}

void get_command(char commandShell[100],int *idxShell){
    for(int i=0;i<idx;i++){
        commandShell[i] = command[i];
    }
    for(int i=idx;i<100;i++){
        commandShell[i]='\0';
    }
    (*idxShell) = idx;
}

void clear_command(){
    for(int i=0;i<idx;i++){
        command[i] = '\0';
    }
    idx = 0;
}

void delete_command(){
    if(idx==0){
        return;
    }
    idx--;
    command[idx] = '\0';
}

/* -- Keyboard Interrupt Service Routine -- */

/**
 * Handling keyboard interrupt & process scancodes into ASCII character.
 * Will start listen and process keyboard scancode if keyboard_input_on.
 */
void keyboard_isr(void) {
    uint8_t scancode = in(KEYBOARD_DATA_PORT);
    char ascii_char;
    if (keyboard_state.keyboard_input_on) {

        if (scancode == CAPSLOCK_SCANCODE) {
            keyboard_state.capslock_on = !keyboard_state.capslock_on;
            return;
        }

        if (scancode == LEFT_SHIFT_SCANCODE || scancode == RIGHT_SHIFT_SCANCODE) {
            keyboard_state.shift_on = true;
        } else if (scancode == LEFT_SHIFT_RELEASE_SCANCODE || scancode == RIGHT_SHIFT_RELEASE_SCANCODE) {
            keyboard_state.shift_on = false;
        } else if (scancode == CTRL_SCANCODE) {
            keyboard_state.ctrl_on = true;
        } else if (scancode == CTRL_RELEASE_SCANCODE) {
            keyboard_state.ctrl_on = false;
        }

        // Arrow key handling
        if (scancode == EXT_SCANCODE_UP) {
            if (cursor_row > 0) {
                cursor_row--;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
            return;
        } else if (scancode == EXT_SCANCODE_DOWN) {
            if (cursor_row < TERMINAL_HEIGHT - 1) {
                cursor_row++;
                framebuffer_set_cursor(cursor_row, cursor_col);
            } 
            return;
        } else if (scancode == EXT_SCANCODE_RIGHT && !(cursor_row == TERMINAL_HEIGHT - 1 && cursor_col == TERMINAL_WIDTH - 1)) {
            if (cursor_col == TERMINAL_WIDTH - 1) {
                cursor_row++;
                cursor_col = 0;
            } else {
                cursor_col++;
            }
            framebuffer_set_cursor(cursor_row, cursor_col);
            return;
        } else if (scancode == EXT_SCANCODE_LEFT && !(cursor_row == 0 && cursor_col == 0)) {
            if (cursor_col == 0) {
                cursor_row--;
                cursor_col = TERMINAL_WIDTH - 1;
                framebuffer_set_cursor(cursor_row, cursor_col);
            } else {
                cursor_col--;
                framebuffer_set_cursor(cursor_row, cursor_col);
            }
            return;
        }

        // Character handling
        if (keyboard_state.capslock_on) {
            ascii_char = keyboard_scancode_caps_to_ascii_map[scancode];
        } else {
            if (keyboard_state.shift_on) {
                ascii_char = keyboard_scancode_shift_to_ascii_map[scancode];
            } else {
                ascii_char = keyboard_scancode_1_to_ascii_map[scancode];
            }
        }

        if (keyboard_state.capslock_on && ascii_char >= 'a' && ascii_char <= 'z') {
            ascii_char += ('a' - 'A');
        }
        keyboard_state.keyboard_buffer = ascii_char;
    }
}
