#ifndef WM_H
#define WM_H

#include <stdlib.h>
#include <X11/Xlib.h>

#define MOVE_AMOUNT 20

Display * dpy;
Window root;
XWindowAttributes attr;
XEvent ev;

typedef struct {
    KeyCode keycode;
    char *string;
    void (*callback)();
} keybinding;

void launch_term();
void launch_clock();
void move_window();
void move_left();
void move_right();
void move_up();
void move_down();
void cycle();
void quit();

void wm_init();
void init_keybindings();
void wm_event_loop();

void keycode_callback(KeyCode);

void warp_pointer();
Window highest_window();

keybinding keybindings[8] = {
    {0, "Return", &launch_term},
    {0, "c", &launch_clock},
    {0, "j", &move_down},
    {0, "k", &move_up},
    {0, "h", &move_left},
    {0, "l", &move_right},
    {0, "Tab", &cycle},
    {0, "q", &quit}
};

#endif
