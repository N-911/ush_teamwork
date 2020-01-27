#include "ush.h"

static t_input *ush_create_node(char **arg, char *d) {
    t_input *temp;

    if (!arg || !d)
        return NULL;
    temp = malloc(sizeof(t_input));
    if (!temp)
        return NULL;
    temp->args = mx_strdup_arr(arg);
    temp->delim = mx_strdup(d);
    temp->next = NULL;
    return temp;
}

void mx_ush_push_back(t_input **list, char **arg, char *d) {
    t_input *tmp;
    t_input *p;

    if (!list || !arg || !d)
        return;
    tmp = ush_create_node(arg, d); // create new
    if (!tmp)
        return;
    p = *list;
    if (*list == NULL) { // find Null-node
        *list = tmp;
        return;
    } else {
        while (p->next != NULL) // find Null-node
            p = p->next;
        p->next = tmp;
    }
}
