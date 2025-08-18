#ifndef LUMAOS_WINDOW_H_
#define LUMAOS_WINDOW_H_

#include <stdint.h>
#include <stdbool.h>
#include <gui/theme.h>

#define WINDOW_BORDERED     (1 << 0)
#define WINDOW_RESIZABLE    (1 << 1)
#define WINDOW_VISIBLE      (1 << 2)
#define WINDOW_TITLED       (1 << 3)

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_RIGHT_BUTTON  0x02
#define MOUSE_MIDDLE_BUTTON 0x04

typedef struct Window {
    char *title;
    uint32_t x, y;
    uint32_t width;
    uint32_t height;
    uint32_t id;
    uint32_t flags;
    bool visible;
    
    uint32_t *framebuffer;
    uint32_t bg_color;
    
    void (*on_close)(struct Window*);
    void (*on_resize)(struct Window*, uint32_t, uint32_t);
    void (*on_mouse_move)(struct Window*, int, int);
    void (*on_mouse_button)(struct Window*, int, int, uint8_t);
    void (*on_key_press)(struct Window*, uint8_t);
    
    void *user_data;
} window_t;

window_t *init_window(const char *title, uint32_t width, uint32_t height, uint32_t flags);
void window_close(window_t *window);
void window_draw(window_t *window);
void window_render(window_t *window);
void window_set_position(window_t *window, uint32_t x, uint32_t y);
void window_set_size(window_t *window, uint32_t width, uint32_t height);
void window_set_title(window_t *window, const char *title);
void window_show(window_t *window);
void window_hide(window_t *window);
void window_set_background(window_t *window, uint32_t color);
void window_draw_pixel(window_t *window, int x, int y, uint32_t color);
void window_draw_rect(window_t *window, int x, int y, int w, int h, uint32_t color);
void window_draw_text(window_t *window, const char *text, int x, int y, uint32_t color);
void window_clear(window_t *window);
void window_present(window_t *window);
bool window_contains_point(window_t *window, int x, int y);
void window_get_client_rect(window_t *window, int *x, int *y, int *w, int *h);
void window_screen_to_client(window_t *window, int screen_x, int screen_y, int *window_x, int *window_y);
void window_client_to_screen(window_t *window, int window_x, int window_y, int *screen_x, int *screen_y);

#endif // LUMAOS_WINDOW_H_
