#include <gui/wm.h>
#include <gui/theme.h>
#include <gui/window.h>
#include <libc/memory.h>
#include <libc/string.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>

static window_manager_t wm_instance;
static bool wm_initialized = false;

void wm_init() {
    if (wm_initialized) return;
    
    wm_instance.windows = list_create();
    wm_instance.active_window = NULL;
    wm_instance.drag_window = NULL;
    wm_instance.is_dragging = false;
    wm_initialized = true;
    
    printf("Window manager initialized\n");
}

window_t* wm_create_window(const char *title, uint32_t x, uint32_t y, 
                          uint32_t width, uint32_t height, uint32_t flags) {
    if (!wm_initialized) wm_init();
    
    window_t *window = init_window(title, width, height, flags);
    if (!window) return NULL;
    
    window->x = x;
    window->y = y;
    
    list_append(wm_instance.windows, window);
    wm_focus_window(window);
    
    return window;
}

void wm_close_window(window_t *window) {
    if (!window || !wm_initialized) return;
    
    list_remove(wm_instance.windows, window);
    
    if (wm_instance.active_window == window) {
        wm_instance.active_window = NULL;
    }
    
    if (wm_instance.drag_window == window) {
        wm_instance.drag_window = NULL;
        wm_instance.is_dragging = false;
    }
    
    window_close(window);
}

void wm_focus_window(window_t *window) {
    if (!window || !wm_initialized) return;
    if (wm_instance.active_window == window) return;
    
    list_remove(wm_instance.windows, window);
    list_append(wm_instance.windows, window);
    
    wm_instance.active_window = window;
}

static uint8_t wm_get_resize_edge(window_t *window, int x, int y) {
    if (!window) return 0;
    
    const int resize_border = 5;
    uint8_t edge = 0;
    
    if (x >= window->x - resize_border && x <= window->x + resize_border) {
        edge |= 1;
    }
    
    if (x >= window->x + window->width - resize_border && x <= window->x + window->width + resize_border) {
        edge |= 2;
    }
    
    if (y >= window->y - resize_border && y <= window->y + resize_border) {
        edge |= 4;
    }
    
    if (y >= window->y + window->height - resize_border && y <= window->y + window->height + resize_border) {
        edge |= 8;
    }
    
    return edge;
}

/*
 * ============================================================
 * DEV NOTES: wm.c - Window Manager Implementation
 * ============================================================
 * 
 * OVERVIEW:
 * This module implements the core window management functionality,
 * including window creation, focus management, and mouse interaction.
 *
 * KEY FEATURES:
 * - Window creation and lifecycle management
 * - Z-order management
 * - Window dragging and resizing
 * - Focus management
 * - Mouse event handling
 *
 * WINDOW MANAGEMENT:
 * - Maintains a list of all windows
 * - Tracks active and focused windows
 * - Handles window z-ordering
 * - Manages window destruction and cleanup
 *
 * MOUSE INTERACTION:
 * - Handles window dragging
 * - Implements window resizing from edges
 * - Manages focus changes on click
 * - Forwards mouse events to windows
 *
 * LIMITATIONS:
 * - No window minimization/maximization
 * - Basic window decoration only
 * - No support for multiple desktops
 * - Limited window management features
 *
 * USAGE:
 * 1. Initialize with wm_init()
 * 2. Create windows with wm_create_window()
 * 3. Handle mouse events with wm_handle_mouse()
 * 4. Render windows with wm_render()
 * 5. Clean up with wm_close_window()
 *
 * TODO:
 * - Add window management features (minimize, maximize, close buttons)
 * - Implement window focus policy (click-to-focus, focus-follows-mouse)
 * - Add window decorations (titlebar buttons, resize handles)
 * - Support for multiple workspaces/desktops
 * - Improve window movement and resize performance
 * - Add support for window snapping
 */

// handle mouse events
void wm_handle_mouse(int x, int y, uint8_t buttons) {
    static uint8_t prev_buttons = 0;
    static bool was_dragging = false;
    
    if (!wm_initialized) return;
    
    // check for mouse button press/release
    bool left_pressed = (buttons & MOUSE_LEFT_BUTTON) && !(prev_buttons & MOUSE_LEFT_BUTTON);
    bool left_released = !(buttons & MOUSE_LEFT_BUTTON) && (prev_buttons & MOUSE_LEFT_BUTTON);
    
    // update previous button state for next time
    prev_buttons = buttons;
    
    // check for window focus change on click
    if (left_pressed) {
        // find the topmost window under the cursor
        window_t *window = wm_get_window_at(x, y);
        if (window && window != wm_instance.active_window) {
            wm_focus_window(window);
        }
    }
    
    // handle window dragging and resizing
    if (buttons & MOUSE_LEFT_BUTTON) {
        if (!wm_instance.is_dragging && !wm_instance.is_resizing && wm_instance.active_window) {
            // check if we should start a drag or resize operation
            if (wm_is_in_titlebar(wm_instance.active_window, x, y)) {
                // start dragging
                wm_instance.drag_window = wm_instance.active_window;
                wm_instance.drag_offset_x = x - wm_instance.active_window->x;
                wm_instance.drag_offset_y = y - wm_instance.active_window->y;
                wm_instance.drag_start_x = wm_instance.active_window->x;
                wm_instance.drag_start_y = wm_instance.active_window->y;
                wm_instance.is_dragging = true;
            } else {
                // check for resize
                uint8_t edge = wm_get_resize_edge(wm_instance.active_window, x, y);
                if (edge) {
                    wm_instance.drag_window = wm_instance.active_window;
                    wm_instance.drag_start_x = x;
                    wm_instance.drag_start_y = y;
                    wm_instance.drag_start_width = wm_instance.active_window->width;
                    wm_instance.drag_start_height = wm_instance.active_window->height;
                    wm_instance.is_resizing = true;
                    wm_instance.resize_edge = edge;
                }
            }
        }
        
        // handle active drag operation
        if (wm_instance.is_dragging && wm_instance.drag_window) {
            // move the window
            wm_instance.drag_window->x = x - wm_instance.drag_offset_x;
            wm_instance.drag_window->y = y - wm_instance.drag_offset_y;
            was_dragging = true;
        }
        // handle active resize operation
        else if (wm_instance.is_resizing && wm_instance.drag_window) {
            int dx = x - wm_instance.drag_start_x;
            int dy = y - wm_instance.drag_start_y;
            
            // resize based on which edges are being dragged
            if (wm_instance.resize_edge & 1) { // Left edge
                int new_width = wm_instance.drag_start_width - dx;
                if (new_width >= wm_instance.drag_window->min_width) {
                    wm_instance.drag_window->x = wm_instance.drag_start_x + dx;
                    wm_instance.drag_window->width = new_width;
                }
            }
            if (wm_instance.resize_edge & 2) { // Right edge
                int new_width = wm_instance.drag_start_width + dx;
                if (new_width >= wm_instance.drag_window->min_width) {
                    wm_instance.drag_window->width = new_width;
                }
            }
            if (wm_instance.resize_edge & 4) { // top edge
                int new_height = wm_instance.drag_start_height - dy;
                if (new_height >= wm_instance.drag_window->min_height) {
                    wm_instance.drag_window->y = wm_instance.drag_start_y + dy;
                    wm_instance.drag_window->height = new_height;
                }
            }
            if (wm_instance.resize_edge & 8) { // bottom edge
                int new_height = wm_instance.drag_start_height + dy;
                if (new_height >= wm_instance.drag_window->min_height) {
                    wm_instance.drag_window->height = new_height;
                }
            }
            
            // notify the window that it was resized
            if (wm_instance.drag_window->on_resize) {
                wm_instance.drag_window->on_resize(
                    wm_instance.drag_window,
                    wm_instance.drag_window->width,
                    wm_instance.drag_window->height
                );
            }
            
            was_dragging = true;
        }
            
        // keep window on screen (optional)
        // TODO: Add screen bounds checking
        }
    } else if (left_released) {
        // if we were dragging and just released, send a click event if we didn't move much
        if ((wm_instance.is_dragging || wm_instance.is_resizing) && !was_dragging) {
            // Convert to window coordinates
            int wx = x - wm_instance.active_window->x;
            int wy = y - wm_instance.active_window->y;
            
            // Send click event to the window
            if (wm_instance.active_window->on_mouse_click) {
                wm_instance.active_window->on_mouse_click(
                    wm_instance.active_window,
                    wx,
                    wy,
                    MOUSE_LEFT_BUTTON
                );
            }
        }
        
        // stop any drag or resize operation
        wm_instance.is_dragging = false;
        wm_instance.is_resizing = false;
        wm_instance.drag_window = NULL;
        was_dragging = false;
    } else if (!(buttons & MOUSE_LEFT_BUTTON)) {
        // handle mouse movement without button press
        if (wm_instance.active_window) {
            // convert to window coordinates
            int wx = x - wm_instance.active_window->x;
            int wy = y - wm_instance.active_window->y;
            
            // send mouse move event to the window
            if (wm_instance.active_window->on_mouse_move) {
                wm_instance.active_window->on_mouse_move(
                    wm_instance.active_window,
                    wx,
                    wy,
                    buttons
                );
            }
            
            // update cursor based on resize edges
            uint8_t edge = wm_get_resize_edge(wm_instance.active_window, x, y);
            // TODO: update system cursor based on edge (e.g., resize arrows)
        }
    }
    
    //  update active window based on mouse position
    window_t *window = wm_get_window_at(x, y);
    if (window && window != wm_instance.active_window) {
        wm_focus_window(window);
    }
}

// render all windows
void wm_render() {
    if (!wm_initialized) return;
    
    // render windows in reverse order (back to front)
    for (int i = 0; i < wm_instance.windows->size; i++) {
        window_t *window = list_get(wm_instance.windows, i);
        if (window && window->visible) {
            window_draw(window);
        }
    }
}

// get the window manager instance
window_manager_t* wm_get_instance() {
    if (!wm_initialized) wm_init();
    return &wm_instance;
}

// get window at screen coordinates
window_t* wm_get_window_at(int x, int y) {
    if (!wm_initialized) return NULL;
    
    // check windows in reverse order (top to bottom)
    for (int i = wm_instance.windows->size - 1; i >= 0; i--) {
        window_t *window = list_get(wm_instance.windows, i);
        if (!window || !window->visible) continue;
        
        if (x >= window->x && x < window->x + window->width &&
            y >= window->y && y < window->y + window->height) {
            return window;
        }
    }
    
    return NULL;
}

// check if coordinates are in window titlebar
bool wm_is_in_titlebar(window_t *window, int x, int y) {
    if (!window) return false;
    
    // define title bar height
    const int titlebar_height = 24; // pixels
    
    return (x >= window->x && x < window->x + window->width &&
            y >= window->y && y < window->y + titlebar_height);
}
