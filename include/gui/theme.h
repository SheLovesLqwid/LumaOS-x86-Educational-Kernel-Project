#ifndef LUMAOS_THEME_H_
#define LUMAOS_THEME_H_

#include <stdint.h>

// Color definitions (VGA color format)
typedef enum {
    // Dark theme colors
    THEME_BG_DARK = 0x000000,        // Pure black
    THEME_BG_DARKER = 0x0A0A1E,      // Very dark blue
    THEME_BG_DARKEST = 0x050510,     // Darker blue
    
    // Accent colors
    THEME_ACCENT = 0x4A90E2,         // Soft blue
    THEME_ACCENT_DARK = 0x2A5A8C,    // Darker blue
    THEME_ACCENT_LIGHT = 0x6AA8FF,   // Lighter blue
    
    // Text colors
    THEME_TEXT = 0xE0E0E0,           // Light gray
    THEME_TEXT_SECONDARY = 0xA0A0A0, // Medium gray
    THEME_TEXT_DISABLED = 0x606060,  // Dark gray
    
    // UI Elements
    THEME_BORDER = 0x1E1E3F,         // Dark blue-gray
    THEME_HIGHLIGHT = 0x2D2D5A,      // Brighter blue-gray
    
    // Status colors
    THEME_SUCCESS = 0x2ECC71,        // Green
    THEME_WARNING = 0xF39C12,        // Orange
    THEME_ERROR = 0xE74C3C           // Red
} theme_color_t;

// Theme structure
typedef struct {
    uint32_t background;
    uint32_t window_bg;
    uint32_t window_titlebar;
    uint32_t window_border;
    uint32_t text_primary;
    uint32_t text_secondary;
    uint32_t accent;
    uint32_t highlight;
    uint32_t button_normal;
    uint32_t button_hover;
    uint32_t button_pressed;
    uint32_t scrollbar;
} theme_t;

// Get the current theme
const theme_t* get_current_theme();

// Initialize theme system
void theme_init();

#endif // LUMAOS_THEME_H_
