#include "ush.h"

static t_export *create_node(void *name, void *value);

void mx_push_export(t_export **list, void *name, void *value) {
    t_export *tmp;
    t_export *p;

    if (!list)
        return;
    tmp = create_node(name, value); // create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) { // find Null-node
        *list = tmp;
        return;
    }
    else {
        while (p->next != NULL) // find Null-node
            p = p->next;
        p->next = tmp;
    }
}

static t_export *create_node(void *name, void *value) {
    t_export *node =  (t_export *)malloc(sizeof(t_export));

    node->name = strdup(name);
    if(!value)
        node->value = strdup("");
    else
        node->value = strdup(value);
    node->next = NULL;
    return node;
}

