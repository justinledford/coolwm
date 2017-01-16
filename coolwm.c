#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "coolwm.h"

void launch_term()
{
    system("xterm &");
}

void launch_clock()
{
    system("xclock &");
}

void move_window(int x, int y)
{
    if (ev.xkey.subwindow != None) {
        XGetWindowAttributes(dpy, ev.xkey.subwindow, &attr);
        XMoveWindow(dpy, ev.xkey.subwindow, attr.x + x, attr.y + y);
    }
}

void move_left()
{
    move_window(-MOVE_AMOUNT, 0);
}

void move_right()
{
    move_window(MOVE_AMOUNT, 0);
}

void move_up()
{
    move_window(0, -MOVE_AMOUNT);
}

void move_down()
{
    move_window(0, MOVE_AMOUNT);
}

void cycle()
{
    XCirculateSubwindows(dpy, root, RaiseLowest);
    warp_pointer();
}

void quit()
{
    exit(0);
}

void warp_pointer()
{
    XWarpPointer(dpy, None, highest_window(),
            0, 0, 0, 0, 0, 0);
}

Window highest_window()
{
    Window _, *children;
    unsigned int nchildren;

    XQueryTree(dpy, root, &_, &_, &children, &nchildren);
    return children[nchildren-1];
}

void keycode_callback(KeyCode keycode)
{
    unsigned int i;

    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++) {
        if (keybindings[i].keycode == keycode)
            return keybindings[i].callback();
    }
}

void (*name_to_func(char *name))()
{
    unsigned int i;

    for (i = 0; i < 8; i++)
        if (strcmp(name, name_to_funcs[i].name) == 0)
            return name_to_funcs[i].func;
}

void parse_conf(char *conf_file)
{
    FILE *fp;
    char *tok, *line;
    size_t len = 0;
    int i;

    fp = fopen(conf_file, "r");
    line = NULL;
    i = 0;

    if (fp == NULL) {
        printf("Error opening config file\n");
        exit(1);
    }

    while (getline(&line, &len, fp) != -1) {
        tok = strtok(line, " ");
        keybindings[i].callback = name_to_func(tok);
        tok = strtok(NULL, " \n");
        keybindings[i].string = malloc(sizeof(char) * strlen(tok)+1);
        strcpy(keybindings[i].string, tok);
        ++i;
    }
}

void init_keybindings()
{
    unsigned int i;
    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++) {
        keybindings[i].keycode = XKeysymToKeycode(dpy,
                XStringToKeysym(keybindings[i].string));
        XGrabKey(dpy, keybindings[i].keycode, Mod1Mask, root, True,
                 GrabModeAsync, GrabModeAsync);
    }
}

void wm_init()
{
    if (!(dpy = XOpenDisplay(0x0))) return;
    root = DefaultRootWindow(dpy);

    init_keybindings();
}

void wm_event_loop()
{
    for (;;) {
        XNextEvent(dpy, &ev);
        if (ev.type == KeyPress)
            keycode_callback(ev.xkey.keycode);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Error: no config file supplied\n");
        exit(1);
    }
    parse_conf(argv[1]);
    wm_init();
    wm_event_loop();
}
