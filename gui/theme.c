#include <gui/theme.h>
#include <libc/string.h>

// global theme instance
static theme_t current_theme;

// initialize the dark midnight theme
static void init_dark_midnight_theme() {
    current_theme.background = THEME_BG_DARKER;
    current_theme.window_bg = THEME_BG_DARKEST;
    current_theme.window_titlebar = THEME_BG_DARKEST;
    current_theme.window_border = THEME_BORDER;
    current_theme.text_primary = THEME_TEXT;
    current_theme.text_secondary = THEME_TEXT_SECONDARY;
    current_theme.accent = THEME_ACCENT;
    current_theme.highlight = THEME_HIGHLIGHT;
    current_theme.button_normal = THEME_BG_DARKER;
    current_theme.button_hover = THEME_ACCENT_DARK;
    current_theme.button_pressed = THEME_ACCENT_LIGHT;
    current_theme.scrollbar = THEME_BORDER;
}

// get the current theme
const theme_t* get_current_theme() {
    return &current_theme;
}

// initialize theme system
void theme_init() {
    // for now, we only have one theme
    init_dark_midnight_theme();
}
