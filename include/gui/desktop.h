#ifndef LUMAOS_DESKTOP_H_
#define LUMAOS_DESKTOP_H_

#include <gui/wm.h>
#include <libc/list.h>
#include <stdint.h>

typedef struct {
    window_manager_t *wm;
    uint32_t width;
    uint32_t height;
    uint32_t bg_color;
    list_t *windows;
    bool is_running;
} desktop_environment_t;

desktop_environment_t* desktop_init(uint32_t width, uint32_t height);

void desktop_run(desktop_environment_t *desktop);

void desktop_shutdown(desktop_environment_t *desktop);

window_t* desktop_create_window(desktop_environment_t *desktop, 
                              const char *title, 
                              uint32_t width, 
                              uint32_t height, 
                              uint32_t flags);

void desktop_close_window(desktop_environment_t *desktop, window_t *window);

desktop_environment_t* desktop_get_instance();

void desktop_draw(desktop_environment_t *desktop);

void desktop_handle_mouse(desktop_environment_t *desktop, int x, int y, uint8_t buttons);

void desktop_handle_keyboard(desktop_environment_t *desktop, uint8_t key);

#endif // LUMAOS_DESKTOP_H_
