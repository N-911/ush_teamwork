#include "ush.h"

void mx_ush_clear_list(t_input **list) {
    t_input *q = *list;
    t_input *tmp = NULL;

    if (!(*list) || !list)
        return;
    while (q) {
        mx_del_strarr(&q->args);
        mx_strdel(&q->delim);
        tmp = q->next;
        free(q);
        q = tmp;
    }
    *list = NULL;
}
