#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>

#include "coolwm.h"

#include "eventnames.h"

void launch_term()  { system("xterm &"); }
void launch_clock() { system("xclock &"); }

void move_window(int x, int y)
{
    if (ev.xkey.subwindow != None) {
        XGetWindowAttributes(dpy, ev.xkey.subwindow, &attr);
        XMoveWindow(dpy, ev.xkey.subwindow, attr.x + x, attr.y + y);
        XWarpPointer(dpy, None, ev.xkey.subwindow, 0, 0, 0, 0,
                     attr.width/2, attr.height/2);
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

        if ((attr.width + w < 1) || (attr.height + h < 1))
            return;

        XResizeWindow(dpy, ev.xkey.subwindow, attr.width + w, attr.height + h);
        XWarpPointer(dpy, None, ev.xkey.subwindow, 0, 0, 0, 0,
                     (attr.width + w)/2, (attr.height + h)/2);
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
    Window w;

    w = wl_next(context.current_group->windows, ev.xkey.subwindow);

    if (!w)
        return;

    XRaiseWindow(dpy, w);
    XGetWindowAttributes(dpy, w, &attr);
    XWarpPointer(dpy, None, w, 0, 0, 0, 0,
                 attr.width/2, attr.height/2);
    set_active(w);
}


void add_to_group()
{
    unsigned int add_group;

    add_group = ev.xkey.keycode - 10;
    if (add_group > 3)
        return;

    /*
     * Delete from current group, add to new group, then send to
     * new group
     */
    wl_delete(context.current_group->windows, ev.xkey.subwindow);
    wl_add(context.groups[add_group].windows, ev.xkey.subwindow);

    if (&context.groups[add_group] != context.current_group)
        XUnmapWindow(dpy, ev.xkey.subwindow);
}

void hide_group(group *grp)
{
    window_list_node *node;

    node = grp->windows->root;
    while (node) {
        if (node->w)
            XUnmapWindow(dpy, node->w);
        node = node->next;
    }
}

void show_group(group *grp)
{
    window_list_node* node;

    node = grp->windows->root;
    while (node) {
        XMapWindow(dpy, node->w);
        node = node->next;
    }
}

void switch_group()
{
    unsigned int show;

    /* why is this 10, 11, ... ? */
    show = ev.xkey.keycode - 10;
    if (show > 10)
        return;

    hide_group(context.current_group);
    show_group(&context.groups[show]);

    context.current_group = &context.groups[show];
}

void remove_from_groups(Window w)
{
    int i;

    for (i = 0; i < 3; i++)
        wl_delete(context.groups[i].windows, w);
}

void draw_border(Window w, unsigned int color)
{
    XSetWindowBorderWidth(dpy, w, BORDER_WIDTH);
    XSetWindowBorder(dpy, w, color);
}

void draw_borders(window_list *wl, unsigned int color)
{
    window_list_node *node;

    node = wl->root;

    while(node) {
        XSetWindowBorder(dpy, node->w, color);
        node = node->next;
    }
}

void quit()
{
    unsigned int i;

    for (i = 0; i < (sizeof(keybindings) / sizeof(keybinding)); i++)
        free(keybindings[i].keystring);

    XCloseDisplay(dpy);
    exit(0);
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

void init_groups()
{
    int i;

    for (i = 0; i < 3; i++) {
        context.groups[i].windows = wl_init();
        context.groups[i].active = 0;
    }
}

void wm_init()
{
    XSetWindowAttributes rootattr;

    if (!(dpy = XOpenDisplay(0x0))) return;
    root = DefaultRootWindow(dpy);

    XSetErrorHandler(error_handler);

    /* Set event mask on root window to receive events */
    rootattr.event_mask = SubstructureNotifyMask | FocusChangeMask |
        EnterWindowMask | LeaveWindowMask;
    XChangeWindowAttributes(dpy, root, 0, &rootattr);
    XSelectInput(dpy, root, rootattr.event_mask);

    init_keybindings();
    init_groups();

    context.current_group = &context.groups[0];
}

void wm_event_loop()
{
    for (;;) {
        XNextEvent(dpy, &ev);
        if (ev.type == KeyPress)
            keycode_callback(ev.xkey.keycode, ev.xkey.state);
        if (ev.type == CreateNotify)
            create_handler(ev.xcreatewindow.window);
        if (ev.type == DestroyNotify)
            destroy_handler(ev.xdestroywindow.window);
        if (ev.type == EnterNotify)
            enter_handler(ev.xcrossing.window);
    }
}

void test()
{
    XSetWindowBorder(dpy, ev.xkey.subwindow, FOCUSED_COLOR);
}

void create_handler(Window w)
{
    XSetWindowAttributes set_attr;

    XGetWindowAttributes(dpy, w, &attr);

    /* ignore "hidden" windows */
    if (!attr.bit_gravity || attr.override_redirect)
        return;

    /* Set event mask to receive events */
    set_attr.event_mask = FocusChangeMask |
        EnterWindowMask | LeaveWindowMask;
    XChangeWindowAttributes(dpy, w, 0, &set_attr);
    XSelectInput(dpy, w, set_attr.event_mask);

    wl_add(context.current_group->windows, w);
    set_active(w);

    /* focus on created window */
    XWarpPointer(dpy, None, w, 0, 0, 0, 0,
                 attr.width/2, attr.height/2);

}

void destroy_handler(Window w)
{
    Window next;

    next = wl_next(context.current_group->windows, w);
    remove_from_groups(w);

    if (w != next)
        set_active(next);
}

void enter_handler(Window w)
{
    if (wl_find(context.current_group->windows, w))
        set_active(w);
}

void set_active(Window w)
{
    Window prev;

    prev = context.current_group->active;
    if (prev && wl_find(context.current_group->windows, prev))
        draw_border(context.current_group->active, UNFOCUSED_COLOR);

    context.current_group->active = w;
    if (w) {
        XSetInputFocus(dpy, w, RevertToPointerRoot, CurrentTime);
        draw_border(w, FOCUSED_COLOR);
    }
}

/* cwm's error_handler */
static int error_handler(Display *dpy, XErrorEvent *e)
{
#ifdef DEBUG
    char msg[80], number[80], req[80];

    XGetErrorText(dpy, e->error_code, msg, sizeof(msg));
    snprintf(number, sizeof(number), "%d", e->request_code);
    XGetErrorDatabaseText(dpy, "XRequest", number,
            "<unknown>", req, sizeof(req));

    warnx("%s(0x%x): %s", req, (unsigned int)e->resourceid, msg);
#endif

    return(0);
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
