#ifndef LUMAOS_MOUSE_H_
#define LUMAOS_MOUSE_H_

#include <stdint.h>
#include <stdbool.h>

#define MOUSE_LEFT_BUTTON    0x01
#define MOUSE_RIGHT_BUTTON   0x02
#define MOUSE_MIDDLE_BUTTON  0x04

#define PS2_DATA_PORT        0x60
#define PS2_CMD_PORT         0x64
#define PS2_STATUS_PORT      0x64

#define PS2_OUTPUT_FULL      (1 << 0)
#define PS2_INPUT_FULL       (1 << 1)
#define PS2_SYSTEM_FLAG      (1 << 2)
#define PS2_CMD_DATA         (1 << 3)
#define PS2_TIMEOUT_ERR      (1 << 6)
#define PS2_PARITY_ERR       (1 << 7)

typedef struct {
    int x;
    int y;
    uint8_t buttons;
    bool initialized;
} mouse_state_t;

typedef struct {
    int x;
    int y;
    int old_x;
    int old_y;
    uint8_t visible;
} mouse_cursor_t;

typedef void (*mouse_callback_t)(int x, int y, uint8_t buttons);

extern mouse_state_t mouse_state;
extern mouse_cursor_t mouse_cursor;

void init_mouse();
void mouse_wait(uint8_t type);
void mouse_write(uint8_t data);
uint8_t mouse_read();
void update_cursor(int x, int y);
void show_cursor();
void hide_cursor();
int get_mouse_x();
int get_mouse_y();
uint8_t get_mouse_buttons();
bool is_mouse_initialized();
void set_mouse_callback(mouse_callback_t callback);

/*
 * ============================================================
 * DEV NOTES: mouse.h - PS/2 Mouse Driver Interface
 * ============================================================
 * 
 * OVERVIEW:
 * This header defines the interface for the PS/2 mouse driver, including
 * data structures, function prototypes, and hardware port definitions.
 *
 * KEY FEATURES:
 * - PS/2 mouse protocol implementation
 * - Button state tracking (left, right, middle)
 * - Cursor position management
 * - Event callback system
 *
 * HARDWARE INTERFACE:
 * - Uses PS/2 controller ports (0x60 for data, 0x64 for commands/status)
 * - Implements standard PS/2 mouse packet format
 * - Handles mouse initialization and configuration
 *
 * USAGE:
 * 1. Call init_mouse() during system initialization
 * 2. Use get_mouse_x()/get_mouse_y() to get current position
 * 3. Check button states with get_mouse_buttons()
 * 4. Register callbacks for mouse events with set_mouse_callback()
 *
 * LIMITATIONS:
 * - Basic 3-button mouse support only
 * - No support for scroll wheel or additional buttons
 * - Assumes standard PS/2 mouse protocol
 *
 * TODO:
 * - Add scroll wheel support
 * - Implement acceleration curves
 * - Support for additional mouse buttons
 * - USB HID compatibility layer
 */

#endif // LUMAOS_MOUSE_H_
