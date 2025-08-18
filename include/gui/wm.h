#ifndef LUMAOS_WM_H_
#define LUMAOS_WM_H_

#include <gui/window.h>
#include <libc/list.h>
#include <stdint.h>
#include <stdbool.h>

// resize edge flags (can be combined)
#define WM_RESIZE_NONE    0x00
#define WM_RESIZE_LEFT    0x01
#define WM_RESIZE_RIGHT   0x02
#define WM_RESIZE_TOP     0x04
#define WM_RESIZE_BOTTOM  0x08
#define WM_RESIZE_TOPLEFT (WM_RESIZE_TOP | WM_RESIZE_LEFT)
#define WM_RESIZE_TOPRIGHT (WM_RESIZE_TOP | WM_RESIZE_RIGHT)
#define WM_RESIZE_BOTTOMLEFT (WM_RESIZE_BOTTOM | WM_RESIZE_LEFT)
#define WM_RESIZE_BOTTOMRIGHT (WM_RESIZE_BOTTOM | WM_RESIZE_RIGHT)

// window manager structure
typedef struct {
    list_t *windows;            // list of all windows
    window_t *active_window;    // currently active window
    window_t *drag_window;      // window being dragged/resized
    
    // dragging state
    int drag_offset_x;          // X offset from window corner when dragging
    int drag_offset_y;          // Y offset from window corner when dragging
    int drag_start_x;           // X position when drag started
    int drag_start_y;           // Y position when drag started
    uint32_t drag_start_width;  // window width when drag started
    uint32_t drag_start_height; // window height when drag started
    
    // state flags
    bool is_dragging;           // is a window being dragged?
    bool is_resizing;           // is a window being resized?
    uint8_t resize_edge;        // which edge is being resized (WM_RESIZE_* flags)
} window_manager_t;

// initialize window manager
void wm_init();

// create a new window
window_t* wm_create_window(const char *title, uint32_t x, uint32_t y, 
                          uint32_t width, uint32_t height, uint32_t flags);

// close a window
void wm_close_window(window_t *window);

// bring window to front
void wm_focus_window(window_t *window);

// handle mouse events
void wm_handle_mouse(int x, int y, uint8_t buttons);

// render all windows
void wm_render();

// get the window manager instance
window_manager_t* wm_get_instance();

// get window at screen coordinates
window_t* wm_get_window_at(int x, int y);

// check if coordinates are in window titlebar
bool wm_is_in_titlebar(window_t *window, int x, int y);

// get the resize edge at the given coordinates (if any)
uint8_t wm_get_resize_edge(window_t *window, int x, int y);

// set the cursor type based on the current operation
void wm_update_cursor(int x, int y);

#endif // LUMAOS_WM_H_
