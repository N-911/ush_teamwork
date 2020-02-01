#include "ush.h"

static t_ast *ast_create_node(char *line, int type) {
    t_ast *q;

    if (!line)
        return NULL;
    q = (t_ast *)malloc(sizeof(t_ast));
    if (!q)
        return NULL;
    q->line = mx_strdup(line);
    q->args = NULL;
    q->type = type;
    q->next = NULL;
    q->left = NULL;
    q->parent = NULL;
    return (q);
}

void mx_ast_push_back(t_ast **head, char *line, int type) {
    t_ast *q;
    t_ast *p;

    if (!head || !line)
        return;
    q = ast_create_node(line, type);
    if (!q)
        return;
    p = *head;
    if (*head == NULL) { // if head = NULL
        *head = q;
        return;
    }
    else {
        while (p->next != NULL) // find last node
            p = p->next;
        p->next = q;
    }
}
