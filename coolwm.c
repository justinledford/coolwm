#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "coolwm.h"

void launch_term()  { system("xterm &"); }
void launch_clock() { system("xclock &"); }

void move_window(int x, int y)
{
    if (ev.xkey.subwindow != None) {
        XGetWindowAttributes(dpy, ev.xkey.subwindow, &attr);
        XMoveWindow(dpy, ev.xkey.subwindow, attr.x + x, attr.y + y);
        XWarpPointer(dpy, None, ev.xkey.subwindow, 0, 0, 0, 0,
                     attr.width/2 + x, attr.height/2 + y);
    }
}

void move_left()    { move_window(-MOVE_AMOUNT, 0); }
void move_right()   { move_window(MOVE_AMOUNT, 0); }
void move_up()      { move_window(0, -MOVE_AMOUNT); }
void move_down()    { move_window(0, MOVE_AMOUNT); }
void big_move_left()    { move_window(-MOVE_AMOUNT*10, 0); }
void big_move_right()   { move_window(MOVE_AMOUNT*10, 0); }
void big_move_up()      { move_window(0, -MOVE_AMOUNT*10); }
void big_move_down()    { move_window(0, MOVE_AMOUNT*10); }

void resize_window(int w, int h)
{
    if (ev.xkey.subwindow != None) {
        XGetWindowAttributes(dpy, ev.xkey.subwindow, &attr);
        XResizeWindow(dpy, ev.xkey.subwindow, attr.width + w, attr.height + h);
    }
}

void resize_left()  { resize_window(-MOVE_AMOUNT, 0); }
void resize_right() { resize_window(MOVE_AMOUNT, 0); }
void resize_up()    { resize_window(0, -MOVE_AMOUNT); }
void resize_down()  { resize_window(0, MOVE_AMOUNT); }
void big_resize_left()  { resize_window(-MOVE_AMOUNT*10, 0); }
void big_resize_right() { resize_window(MOVE_AMOUNT*10, 0); }
void big_resize_up()    { resize_window(0, -MOVE_AMOUNT*10); }
void big_resize_down()  { resize_window(0, MOVE_AMOUNT*10); }

void cycle()
{
    XCirculateSubwindows(dpy, root, RaiseLowest);
    warp_pointer();
}

void quit()
{
    unsigned int i;

    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++)
        free(keybindings[i].keystring);

    XCloseDisplay(dpy);
    exit(0);
}

void warp_pointer()
{
    XWarpPointer(dpy, None, *highest_window(), 0, 0, 0, 0, 0, 0);
}

Window * highest_window()
{
    Window _, *children, *highest;
    unsigned int nchildren;

    XQueryTree(dpy, root, &_, &_, &children, &nchildren);
    highest = &children[nchildren-1];
    XFree(children);
    return highest;
}

void keycode_callback(KeyCode keycode, unsigned int state)
{
    unsigned int i;

    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++) {
        if ((keybindings[i].keycode == keycode) &&
            (keybindings[i].modmasks == state))
            return keybindings[i].callback();
    }
}

void (*name_to_func(char *name))()
{
    unsigned int i;

    for (i = 0; i < sizeof name_to_funcs; i++)
        if (strcmp(name, name_to_funcs[i].name) == 0)
            return name_to_funcs[i].func;
}

void parse_conf(char *conf_file)
{
    FILE *fp;
    char *k, *tok, *line;
    size_t len = 0;
    int i;
    unsigned int j;

    fp = fopen(conf_file, "r");
    line = NULL;
    i = 0;

    if (fp == NULL) {
        printf("Error opening config file\n");
        exit(1);
    }

    while (getline(&line, &len, fp) != -1) {
        // Parse command name
        tok = strtok(line, " ");
        keybindings[i].callback = name_to_func(tok);

        // Parse keybinding
        tok = strtok(NULL, " \n");
        k = strtok(tok, "-");

        // Parse modifiers
        for (j = 0; j < strlen(k); ++j) {
            strncpy(keybindings[i].modstrings[j], k+j, 1);
            keybindings[i].modstrings[j][1] = '\0';
        }

        // Parse key
        k = strtok(NULL, " ");
        keybindings[i].keystring = malloc(sizeof(char) * strlen(k)+1);
        strcpy(keybindings[i].keystring, k);
        ++i;
    }

    free(line);
    fclose(fp);
}

unsigned int modstring_to_sym(char *s)
{
    if (strcmp(s, "A") == 0)
        return Mod1Mask;
    if (strcmp(s, "S") == 0)
        return ShiftMask;
    if (strcmp(s, "C") == 0)
        return ControlMask;
    return 0;
}

void init_keybindings()
{
    unsigned int i, mask, j;

    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++) {

        keybindings[i].keycode = XKeysymToKeycode(dpy,
                XStringToKeysym(keybindings[i].keystring));

        mask = 0;
        for (j = 0; j < 3; ++j)
            mask |= modstring_to_sym(keybindings[i].modstrings[j]);
        keybindings[i].modmasks = mask;

        XGrabKey(dpy, keybindings[i].keycode, mask, root, True,
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
        if (ev.type == KeyPress) {
            keycode_callback(ev.xkey.keycode, ev.xkey.state);
        }
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
