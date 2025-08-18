#include <gui/ltk/window.h>
#include <gui/theme.h>
#include <libc/string.h>
#include <libc/memory.h>
#include <libc/stdio.h>

static uint32_t next_window_id = 1;

static void default_window_proc(window_t *window, int event_type, void *data) {
    return;
}

static void draw_window_frame(window_t *window) {
    if (!window || !window->visible) return;
    
    const theme_t *theme = get_current_theme();
    
    window_draw_rect(window, 0, 0, window->width, window->height, theme->window_bg);
    
    if (window->flags & WINDOW_TITLED) {
        const int titlebar_height = 24;
        
        window_draw_rect(window, 0, 0, window->width, titlebar_height, theme->window_titlebar);
        
        if (window->title) {
            int text_x = 8;
            int text_y = (titlebar_height - 8) / 2;
            window_draw_text(window, window->title, text_x, text_y, theme->text_primary);
        }
    }
    
    if (window->flags & WINDOW_BORDERED) {
        window_draw_rect(window, 0, 0, window->width, 1, theme->window_border);
        window_draw_rect(window, 0, window->height - 1, window->width, 1, theme->window_border);
        window_draw_rect(window, 0, 0, 1, window->height, theme->window_border);
        window_draw_rect(window, window->width - 1, 0, 1, window->height, theme->window_border);
    }
}

window_t *init_window(const char *title, uint32_t width, uint32_t height, uint32_t flags) {
    window_t *window = (window_t*)kmalloc(sizeof(window_t));
    if (!window) return NULL;
    
    window->id = next_window_id++;
    window->x = 100;
    window->y = 100;
    window->width = width;
    window->height = height;
    window->flags = flags | WINDOW_VISIBLE;
    window->visible = true;
    window->bg_color = 0xFF000000;
    
    if (title) {
        window->title = (char*)kmalloc(strlen(title) + 1);
        if (window->title) {
            strcpy(window->title, title);
        }
    } else {
        window->title = NULL;
    }
    
    window->framebuffer = (uint32_t*)kmalloc(width * height * sizeof(uint32_t));
    if (!window->framebuffer) {
        kfree(window->title);
        kfree(window);
        return NULL;
    }
    
    window->on_close = NULL;
    window->on_resize = NULL;
    window->on_mouse_move = NULL;
    window->on_mouse_button = NULL;
    window->on_key_press = NULL;
    window->on_draw = NULL;
    
    window_clear(window);
    
    return window;
}

void window_close(window_t *window) {
    if (!window) return;
    
    if (window->on_close) {
        window->on_close(window);
    }
    
    if (window->title) {
        kfree(window->title);
    }
    if (window->framebuffer) {
        kfree(window->framebuffer);
    }
    kfree(window);
}

void window_draw(window_t *window) {
    if (!window || !window->visible) return;
    
    draw_window_frame(window);
    
    if (window->on_draw) {
        window->on_draw(window);
    }
}

void window_render(window_t *window) {
    if (!window || !window->visible) return;
    
}

void window_set_position(window_t *window, uint32_t x, uint32_t y) {
    if (!window) return;
    window->x = x;
    window->y = y;
}

void window_set_size(window_t *window, uint32_t width, uint32_t height) {
    if (!window || width == 0 || height == 0) return;
    
    // save old size for callback
    uint32_t old_width = window->width;
    uint32_t old_height = window->height;
    
    // update window size
    window->width = width;
    window->height = height;
    
    // reallocate framebuffer
    if (window->framebuffer) {
        kfree(window->framebuffer);
    }
    
    window->framebuffer = (uint32_t*)kmalloc(width * height * sizeof(uint32_t));
    if (!window->framebuffer) {
        // handle error - out of memory
        window->width = old_width;
        window->height = old_height;
        return;
    }
    
    // clear the new framebuffer
    window_clear(window);
    
    // call resize callback if set
    if (window->on_resize) {
        window->on_resize(window, width, height);
    }
}

// set window title
void window_set_title(window_t *window, const char *title) {
    if (!window) return;
    
    // free old title if it exists
    if (window->title) {
        kfree(window->title);
        window->title = NULL;
    }
    
    if (title) {
        window->title = (char*)kmalloc(strlen(title) + 1);
        if (window->title) {
            strcpy(window->title, title);
        }
    }
}

void window_show(window_t *window) {
    if (!window) return;
    window->visible = true;
    window->flags |= WINDOW_VISIBLE;
}

void window_hide(window_t *window) {
    if (!window) return;
    window->visible = false;
    window->flags &= ~WINDOW_VISIBLE;
}

void window_set_background(window_t *window, uint32_t color) {
    if (!window) return;
    window->bg_color = color;
    window_clear(window);
}

void window_draw_pixel(window_t *window, int x, int y, uint32_t color) {
    if (!window || !window->framebuffer || x < 0 || y < 0 || 
        x >= (int)window->width || y >= (int)window->height) {
        return;
    }
    
    window->framebuffer[y * window->width + x] = color;
}

// draw a filled rectangle
void window_draw_rect(window_t *window, int x, int y, int w, int h, uint32_t color) {
    if (!window || !window->framebuffer || w <= 0 || h <= 0) return;
    
    // clip rectangle to window bounds
    int x1 = (x < 0) ? 0 : x;
    int y1 = (y < 0) ? 0 : y;
    int x2 = (x + w > (int)window->width) ? window->width : x + w;
    int y2 = (y + h > (int)window->height) ? window->height : y + h;
    
    // draw the rectangle
    for (int dy = y1; dy < y2; dy++) {
        for (int dx = x1; dx < x2; dx++) {
            window->framebuffer[dy * window->width + dx] = color;
        }
    }
}

// draw text at the specified position (simplified)
void window_draw_text(window_t *window, const char *text, int x, int y, uint32_t color) {
    if (!window || !text) return;
    
    // this is a simplified text drawing function
    // in a real implementation, you would use a font rendering system
    
    // for now, we'll just draw a placeholder
    const char *p = text;
    int cx = x;
    int cy = y;
    
    while (*p) {
        if (*p == '\n') {
            cx = x;
            cy += 8; // line height
        } else {
            // draw character (simplified as a rectangle for now)
            window_draw_rect(window, cx, cy, 8, 8, color);
            cx += 8; // character width
        }
        p++;
    }
}

// clear window with background color
void window_clear(window_t *window) {
    if (!window || !window->framebuffer) return;
    
    uint32_t *pixel = window->framebuffer;
    for (uint32_t i = 0; i < window->width * window->height; i++) {
        *pixel++ = window->bg_color;
    }
}

// present the window (swap buffers if double buffered)
void window_present(window_t *window) {
    if (!window) return;
    
    // in a real implementation, this would:
    // 1. Apply any window decorations (borders, titlebar, etc.)
    // 2. Blit the window's framebuffer to the screen at the window's position
    // 3. Handle transparency and clipping
    
    // for now, we'll just mark the window as needing a redraw
    // the window manager will handle the actual rendering
}

// check if coordinates are inside window
bool window_contains_point(window_t *window, int x, int y) {
    if (!window || !window->visible) return false;
    return (x >= (int)window->x && x < (int)(window->x + window->width) &&
            y >= (int)window->y && y < (int)(window->y + window->height));
}

// get window client area (excluding borders/titlebar)
void window_get_client_rect(window_t *window, int *x, int *y, int *w, int *h) {
    if (!window) return;
    
    int border_size = (window->flags & WINDOW_BORDERED) ? 1 : 0;
    int titlebar_size = (window->flags & WINDOW_TITLED) ? 24 : 0;
    
    if (x) *x = border_size;
    if (y) *y = titlebar_size + border_size;
    if (w) *w = window->width - 2 * border_size;
    if (h) *h = window->height - titlebar_size - 2 * border_size;
}

// convert screen coordinates to window coordinates
void window_screen_to_client(window_t *window, int screen_x, int screen_y, int *window_x, int *window_y) {
    if (!window) return;
    
    if (window_x) *window_x = screen_x - window->x;
    if (window_y) *window_y = screen_y - window->y;
}

// convert window coordinates to screen coordinates
void window_client_to_screen(window_t *window, int window_x, int window_y, int *screen_x, int *screen_y) {
    if (!window) return;
    
    if (screen_x) *screen_x = window->x + window_x;
    if (screen_y) *screen_y = window->y + window_y;
}
