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
    char *keystring;
    char modstrings[2][2];
    unsigned int modmasks;
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

void parse_conf(char *);
void wm_init();
void init_keybindings();
void wm_event_loop();

void keycode_callback(KeyCode);
void (*name_to_func(char *))();

void warp_pointer();
Window highest_window();

struct {
    char *name;
    void (*func)();
} name_to_funcs[] = {
    {"launch_term", &launch_term},
    {"launch_clock", &launch_clock},
    {"move_down", &move_down},
    {"move_up", &move_up},
    {"move_left", &move_left},
    {"move_right", &move_right},
    {"cycle", &cycle},
    {"quit", &quit}
};

keybinding keybindings[8];

#endif
