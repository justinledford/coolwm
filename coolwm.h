#ifndef WM_H
#define WM_H

#include <stdlib.h>
#include <X11/Xlib.h>

#include "window_list.h"

#define MOVE_AMOUNT         20
#define FOCUSED_COLOR       0xcfe0fc
#define UNFOCUSED_COLOR     0x103268
#define BORDER_WIDTH        5

Display * dpy;
Window root;
XWindowAttributes attr;
XEvent ev;

typedef struct {
    KeyCode keycode;
    char *keystring;
    char modstrings[3][2];
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
void big_move_left();
void big_move_right();
void big_move_up();
void big_move_down();
void resize_window();
void resize_left();
void resize_right();
void resize_up();
void resize_down();
void big_resize_left();
void big_resize_right();
void big_resize_up();
void big_resize_down();
void cycle();
void quit();

void add_to_group();
void hide_group(window_list *);
void show_group(window_list *);
void switch_group();
void remove_from_groups(Window);

void draw_border(Window, unsigned int);
void draw_borders(window_list *, unsigned int);

void create_handler(Window);
void destroy_handler(Window);

void parse_conf(char *);
void wm_init();
void init_keybindings();
void init_groups();
void wm_event_loop();

void test();

void keycode_callback(KeyCode, unsigned int);
void (*name_to_func(char *))();

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
    {"big_move_down", &big_move_down},
    {"big_move_up", &big_move_up},
    {"big_move_left", &big_move_left},
    {"big_move_right", &big_move_right},
    {"resize_down", &resize_down},
    {"resize_up", &resize_up},
    {"resize_left", &resize_left},
    {"resize_right", &resize_right},
    {"big_resize_down", &big_resize_down},
    {"big_resize_up", &big_resize_up},
    {"big_resize_left", &big_resize_left},
    {"big_resize_right", &big_resize_right},
    {"cycle", &cycle},
    {"add_to_group", &add_to_group},
    {"switch_group", &switch_group},
    {"quit", &quit},
    {"test", &test}
};

keybinding keybindings[27];

window_list* groups[3];

struct {
    window_list* current_group;
} context;

#endif
