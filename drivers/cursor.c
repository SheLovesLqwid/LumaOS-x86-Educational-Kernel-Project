#include <drivers/display.h>
#include <drivers/mouse.h>
#include <stdint.h>

#define CURSOR_CHAR 0xDB

static uint16_t cursor_backup = 0;
static uint8_t cursor_visible = 0;

void draw_cursor(int32_t x, int32_t y) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }

    uint16_t position = y * SCREEN_WIDTH + x;
    
    if (!cursor_visible) {
        cursor_backup = video_memory[position];
    }
    
    video_memory[position] = (CURSOR_CHAR) | (0x1F << 8);
    cursor_visible = 1;
}

void clear_cursor(int32_t x, int32_t y) {
    if (!cursor_visible) return;
    
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) {
        return;
    }
    
    uint16_t position = y * SCREEN_WIDTH + x;
    video_memory[position] = cursor_backup;
    cursor_visible = 0;
}

void update_cursor(int32_t x, int32_t y) {
    if (cursor_visible) {
        clear_cursor(mouse_cursor.old_x, mouse_cursor.old_y);
    }
    
    draw_cursor(x, y);
}

/*
 * ============================================================
 * DEV NOTES: cursor.c - Text Mode Cursor Management
 * ============================================================
 * 
 * OVERVIEW:
 * This module handles the visual representation of the cursor in text mode.
 * It provides functions to draw, clear, and update the cursor position.
 *
 * KEY FEATURES:
 * - Saves and restores the character under the cursor
 * - Handles cursor visibility toggling
 * - Prevents cursor from being drawn outside screen bounds
 * - Uses a simple block character for the cursor
 *
 * LIMITATIONS:
 * - Single cursor support only
 * - No support for different cursor styles
 * - Assumes VGA text mode
 *
 * USAGE:
 * - Call update_cursor(x, y) to move the cursor
 * - The module handles the rest automatically
 *
 * TODO:
 * - Add support for different cursor styles
 * - Implement cursor blinking
 * - Add support for multiple cursors
 * - Optimize for performance
 */
