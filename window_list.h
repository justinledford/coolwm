#include <stdlib.h>
#include <X11/Xlib.h>

struct window_list_node {
    Window w;
    struct window_list_node* next;
};

struct window_list {
    struct window_list_node* root;
};

typedef struct window_list window_list;
typedef struct window_list_node window_list_node;


window_list* wl_init()
{
    window_list* wl = malloc(sizeof(window_list));
    wl->root = NULL;
    return wl;
}

void wl_add(window_list *wl, Window w)
{
    window_list_node* node;

    node = malloc(sizeof(window_list_node));
    node->w = w;
    node->next = wl->root;
    wl->root = node;
}

void wl_delete(window_list *wl, Window w)
{
    window_list_node **node, *tmp;

    node = &wl->root;

    while (*node) {
        if ((*node)->w == w) {
            tmp = *node;
            *node = ((*node)->next);
            free(tmp);
        } else {
            node = &((*node)->next);
        }
    }
}
