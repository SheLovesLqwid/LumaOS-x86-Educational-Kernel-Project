#include <gui/desktop.h>
#include <gui/theme.h>
#include <libc/memory.h>
#include <libc/stdio.h>
#include <libc/string.h>

// Global desktop instance
static desktop_environment_t *desktop_instance = NULL;

// Default window procedure for desktop windows
static void default_window_proc(window_t *window, int event_type, void *data) {
    // Default window procedure
    switch (event_type) {
        case WINDOW_EVENT_CLOSE:
            // Handle window close
            break;
        case WINDOW_EVENT_RESIZE:
            // Handle window resize
            break;
        case WINDOW_EVENT_MOUSE_MOVE:
            // Handle mouse move
            break;
        case WINDOW_EVENT_MOUSE_BUTTON:
            // Handle mouse button
            break;
        case WINDOW_EVENT_KEY_PRESS:
            // Handle key press
            break;
    }
}

// Initialize the desktop environment
desktop_environment_t* desktop_init(uint32_t width, uint32_t height) {
    if (desktop_instance) {
        return desktop_instance;
    }
    
    // Allocate memory for desktop
    desktop_environment_t *desktop = (desktop_environment_t*)kmalloc(sizeof(desktop_environment_t));
    if (!desktop) {
        return NULL;
    }
    
    // Initialize window manager
    desktop->wm = wm_get_instance();
    wm_init();
    
    // Set desktop properties
    desktop->width = width;
    desktop->height = height;
    desktop->bg_color = 0xFF1A1A2E; // Dark blue background
    desktop->is_running = false;
    
    // Initialize window list
    desktop->windows = list_create();
    if (!desktop->windows) {
        kfree(desktop);
        return NULL;
    }
    
    // Initialize theme
    theme_init();
    
    // Set global instance
    desktop_instance = desktop;
    
    printf("Desktop environment initialized (%dx%d)\n", width, height);
    
    return desktop;
}

// Run the desktop main loop
void desktop_run(desktop_environment_t *desktop) {
    if (!desktop || !desktop->is_running) return;
    
    printf("Starting desktop main loop...\n");
    
    // Main desktop loop
    while (desktop->is_running) {
        // In a real implementation, this would:
        // 1. Process input events (mouse, keyboard)
        // 2. Update window states
        // 3. Render the desktop and all windows
        
        // For now, we'll just render the desktop
        desktop_draw(desktop);
        
        // In a real implementation, we would have some kind of vsync or delay here
    }
}

// Shut down the desktop environment
void desktop_shutdown(desktop_environment_t *desktop) {
    if (!desktop) return;
    
    printf("Shutting down desktop environment...\n");
    
    // Close all windows
    for (int i = 0; i < desktop->windows->size; i++) {
        window_t *window = (window_t*)list_get(desktop->windows, i);
        if (window) {
            window_close(window);
        }
    }
    
    // Free window list
    list_destroy(desktop->windows);
    
    // Free desktop instance
    kfree(desktop);
    desktop_instance = NULL;
    
    printf("Desktop environment shut down\n");
}

// Create a new window on the desktop
window_t* desktop_create_window(desktop_environment_t *desktop, 
                              const char *title, 
                              uint32_t width, 
                              uint32_t height, 
                              uint32_t flags) {
    if (!desktop) return NULL;
    
    // Create window using window manager
    window_t *window = wm_create_window(title, 100, 100, width, height, 
                                      flags | WINDOW_BORDERED | WINDOW_TITLED);
    if (!window) {
        return NULL;
    }
    
    // Set default background color from theme
    const theme_t *theme = get_current_theme();
    window_set_background(window, theme->window_bg);
    
    // Set default callbacks
    window->on_close = default_window_proc;
    window->on_resize = default_window_proc;
    window->on_mouse_move = default_window_proc;
    window->on_mouse_button = default_window_proc;
    window->on_key_press = default_window_proc;
    
    // Add to window list
    list_append(desktop->windows, window);
    
    return window;
}

// Close a window
void desktop_close_window(desktop_environment_t *desktop, window_t *window) {
    if (!desktop || !window) return;
    
    // Remove from window list
    list_remove(desktop->windows, window);
    
    // Close the window
    wm_close_window(window);
}

// Get the desktop instance
desktop_environment_t* desktop_get_instance() {
    return desktop_instance;
}

// Draw the desktop (background, taskbar, etc.)
void desktop_draw(desktop_environment_t *desktop) {
    if (!desktop) return;
    
    // In a real implementation, this would:
    // 1. Clear the screen with the background color
    // 2. Draw the desktop background (wallpaper, icons, etc.)
    // 3. Draw the taskbar/panel
    // 4. Let the window manager render all windows
    
    // For now, we'll just clear the screen with the background color
    // and let the window manager handle the rest
    
    // Clear screen with desktop background color
    // (In a real implementation, this would use the display driver)
    
    // Let the window manager render all windows
    wm_render();
}

// Handle mouse events
void desktop_handle_mouse(desktop_environment_t *desktop, int x, int y, uint8_t buttons) {
    if (!desktop) return;
    
    // Forward mouse events to window manager
    wm_handle_mouse(x, y, buttons);
    
    // In a real implementation, we would also handle:
    // - Desktop interactions (clicking on desktop icons, etc.)
    // - Taskbar interactions
    // - Window dragging/resizing
    
    // For now, we'll just print the mouse position
    // printf("Mouse: %d, %d (buttons: 0x%02X)\n", x, y, buttons);
}

// Handle keyboard events
void desktop_handle_keyboard(desktop_environment_t *desktop, uint8_t key) {
    if (!desktop) return;
    
    // In a real implementation, we would:
    // 1. Handle global shortcuts (e.g., Alt+Tab, Alt+F4)
    // 2. Forward the key event to the active window
    
    // For now, we'll just print the key press
    printf("Key press: 0x%02X\n", key);
}
