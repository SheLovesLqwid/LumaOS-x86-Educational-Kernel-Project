#include <drivers/mouse.h>
#include <drivers/display.h>
#include <drivers/ports.h>
#include <cpu/isr.h>
#include <libc/function.h>
#include <drivers/cursor.h>
#include <gui/wm.h>
#include <libc/stdio.h>
#include <stdbool.h>

mouse_state_t mouse_state = {0};
mouse_cursor_t mouse_cursor = {0};

typedef struct {
    uint8_t buttons;
    int8_t x_movement;
    int8_t y_movement;
    int8_t z_movement;
} mouse_packet_t;

static mouse_packet_t mouse_packet;
static int8_t mouse_cycle = 0;
static uint8_t mouse_data[4];
static int mouse_x = 40, mouse_y = 12;
static uint8_t mouse_buttons = 0;
static bool mouse_initialized = false;

void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (--timeout) {
            if ((port_byte_in(PS2_STATUS_PORT) & PS2_OUTPUT_FULL) == 0) {
                return;
            }
        }
    } else {
        while (--timeout) {
            if ((port_byte_in(PS2_STATUS_PORT) & PS2_INPUT_FULL) != 0) {
                return;
            }
        }
    }
}

void mouse_write(uint8_t data) {
    mouse_wait(0);
    port_byte_out(PS2_CMD_PORT, 0xD4);
    mouse_wait(0);
    port_byte_out(PS2_DATA_PORT, data);
}

uint8_t mouse_read() {
    mouse_wait(1);
    return port_byte_in(PS2_DATA_PORT);
}

void mouse_handler(registers_t *regs) {
    (void)regs;
    
    switch(mouse_cycle) {
        case 0:
            mouse_data[0] = port_byte_in(PS2_DATA_PORT);
            if (!(mouse_data[0] & 0x08)) {
                mouse_cycle = 0;
                return;
            }
            mouse_cycle++;
            break;
            
        case 1:
            mouse_data[1] = port_byte_in(PS2_DATA_PORT);
            mouse_cycle++;
            break;
            
        case 2:
            mouse_data[2] = port_byte_in(PS2_DATA_PORT);
            
            mouse_x += (mouse_data[1] - ((mouse_data[0] << 4) & 0x100));
            mouse_y -= (mouse_data[2] - ((mouse_data[0] << 3) & 0x100));
            
            mouse_buttons = mouse_data[0] & 0x07;
            
            mouse_cursor.old_x = mouse_cursor.x;
            mouse_cursor.old_y = mouse_cursor.y;
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
            if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;
            
            mouse_cursor.x = mouse_x;
            mouse_cursor.y = mouse_y;
            
            update_cursor(mouse_cursor.x, mouse_cursor.y);
            
            if (mouse_callback) {
                mouse_callback(mouse_cursor.x, mouse_cursor.y, mouse_buttons);
            }
            
            mouse_cycle = 0;
            break;
    }
}

void register_mouse_callback(mouse_callback_t callback) {
    mouse_callback = callback;
}

void init_mouse() {
    uint8_t status;
    
    mouse_wait(0);
    port_byte_out(PS2_CMD_PORT, 0xA8);
    
    mouse_wait(0);
    port_byte_out(PS2_CMD_PORT, 0x20);
    mouse_wait(1);
    status = (port_byte_in(PS2_DATA_PORT) | 2);
    mouse_wait(0);
    port_byte_out(PS2_CMD_PORT, 0x60);
    mouse_wait(0);
    port_byte_out(PS2_DATA_PORT, status);
    
    mouse_write(0xF6);
    mouse_read();
    
    mouse_write(0xF4);
    mouse_read();
    
    register_interrupt_handler(IRQ12, mouse_handler);
    
    update_cursor(mouse_x, mouse_y);
    
    mouse_initialized = true;
    printf("PS/2 Mouse initialized\n");
}
