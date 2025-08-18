#include <gui/desktop.h>
#include <gui/theme.h>
#include <libc/stdio.h>
#include <libc/stdlib.h>

// button state structure
typedef struct {
    uint32_t color;
    int x, y, w, h;
    bool is_pressed;
} button_t;

// demo window state
typedef struct {
    window_t *window;
    button_t button;
    bool running;
} demo_state_t;

// draw a button
static void draw_button(button_t *button, window_t *window) {
    const theme_t *theme = get_current_theme();
    
    // draw button background
    uint32_t bg_color = button->is_pressed ? theme->button_pressed : 
                       (button->color ? button->color : theme->button_normal);
    
    window_draw_rect(window, button->x, button->y, button->w, button->h, bg_color);
    
    // draw button border
    window_draw_rect(window, button->x, button->y, button->w, 1, theme->window_border);
    window_draw_rect(window, button->x, button->y + button->h - 1, button->w, 1, theme->window_border);
    window_draw_rect(window, button->x, button->y, 1, button->h, theme->window_border);
    window_draw_rect(window, button->x + button->w - 1, button->y, 1, button->h, theme->window_border);
    
    // draw button label (centered)
    const char *label = "Click Me!";
    int text_x = button->x + (button->w - 8 * 8) / 2; // approximate width of text
    int text_y = button->y + (button->h - 8) / 2;     // center vertically
    window_draw_text(window, label, text_x, text_y, theme->text_primary);
}

// check if point is inside button
static bool is_point_in_button(button_t *button, int x, int y) {
    return (x >= button->x && x < button->x + button->w &&
            y >= button->y && y < button->y + button->h);
}

// window draw callback
static void demo_draw(window_t *window) {
    demo_state_t *state = (demo_state_t*)window->user_data;
    
    // clear window
    window_clear(window);
    
    // draw window title
    window_draw_text(window, "LumaOS GUI Demo", 10, 30, 0xFFFFFFFF);
    
    // draw button
    draw_button(&state->button, window);
    
    // draw instructions
    window_draw_text(window, "Click the button to change its color!", 10, 100, 0xFFFFFFFF);
}

// window mouse button callback
static void demo_mouse_button(window_t *window, int x, int y, uint8_t button) {
    demo_state_t *state = (demo_state_t*)window->user_data;
    
    // convert screen coordinates to window coordinates
    int wx, wy;
    window_screen_to_client(window, x, y, &wx, &wy);
    
    if (button & MOUSE_LEFT_BUTTON) {
        if (is_point_in_button(&state->button, wx, wy)) {
            // button was clicked
            state->button.is_pressed = true;
            
            // change button color to a random color
            state->button.color = 0xFF000000 | 
                                 (rand() % 0x7F + 0x80) |         // R
                                ((rand() % 0x7F + 0x80) << 8) |   // G
                                ((rand() % 0x7F + 0x80) << 16);   // B
            
            // request a redraw
            window_draw(window);
        }
    } else {
        state->button.is_pressed = false;
        window_draw(window);
    }
}

// main demo function
void gui_demo() {
    printf("Starting GUI demo...\n");
    
    // initialize desktop environment
    desktop_environment_t *desktop = desktop_init(800, 600);
    if (!desktop) {
        printf("Failed to initialize desktop environment!\n");
        return;
    }
    
    // create demo window
    window_t *window = desktop_create_window(desktop, "LumaOS Demo", 400, 300, 
                                           WINDOW_BORDERED | WINDOW_TITLED);
    if (!window) {
        printf("Failed to create demo window!\n");
        desktop_shutdown(desktop);
        return;
    }
    
    // set window position
    window_set_position(window, 200, 150);
    
    // set window background color
    const theme_t *theme = get_current_theme();
    window_set_background(window, theme->window_bg);
    
    // set up demo state
    demo_state_t *state = (demo_state_t*)kmalloc(sizeof(demo_state_t));
    if (!state) {
        printf("Out of memory!\n");
        desktop_close_window(desktop, window);
        desktop_shutdown(desktop);
        return;
    }
    
    state->window = window;
    state->running = true;
    
    // initialize button
    state->button.x = 100;
    state->button.y = 100;
    state->button.w = 200;
    state->button.h = 40;
    state->button.color = 0; // use default color
    state->button.is_pressed = false;
    
    // set window callbacks
    window->user_data = state;
    window->on_close = [](window_t *w) { 
        demo_state_t *s = (demo_state_t*)w->user_data; 
        s->running = false; 
    };
    window->on_mouse_button = demo_mouse_button;
    
    // set draw callback
    window_draw(window);
    
    printf("GUI demo running. Close the window to exit.\n");
    
    // main loop (simplified for demo)
    while (state->running) {
        // in a real implementation, this would process events from the system
        // for now, we just let the window manager handle events
        desktop_run(desktop);
    }
    
    // clean up
    kfree(state);
    desktop_close_window(desktop, window);
    desktop_shutdown(desktop);
    
    printf("GUI demo finished.\n");
}

int main(int argc, char **argv) {
    gui_demo();
    return 0;
}
