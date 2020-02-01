#include "ush.h"
/*
*  clear one lists (parsed_line)
*/
void mx_ast_clear_list(t_ast **list) {
    t_ast *q = *list;
    t_ast *tmp = NULL;

    if (!(*list) || !list)
        return;
    while (q) {
        mx_strdel(&q->line);
        if (q->args)
            mx_del_strarr(&q->args);
        if (q->left)
            mx_printstr("left exists\n");
        tmp = q->next;
        free(q);
        q = tmp;
    }
    *list = NULL;
}
/*
*  clear array of lists (Abstract Syntax Tree)
*/
void mx_ast_clear_all(t_ast ***list) {
    t_ast **q = *list;

    for (int i = 0; q[i]; i++)  // clear leeks
        mx_ast_clear_list(&q[i]);
    free(q);
    q = NULL;
}
