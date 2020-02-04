#include "ush.h"

static t_ast *ast_create_node(char **args, int type) {
    t_ast *q;

    if (!args)
        return NULL;
    q = (t_ast *)malloc(sizeof(t_ast));
    if (!q)
        return NULL;
    q->args = mx_strdup_arr(args);
    q->type = type;
    q->next = NULL;
    q->left = NULL;
    q->parent = NULL;
    return (q);
}

void mx_ast_push_back(t_ast **head, char **args, int type) {
    t_ast *q;
    t_ast *p;

    if (!head || !args)
        return;
    q = ast_create_node(args, type);
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

void mx_ast_push_back_redirection(t_ast **head, t_ast **list) {
    t_ast *q;
    t_ast *p = *list;
    int sep = p->next->type;

    if (!head || !list)
        return;
    q = ast_create_node(p->args, p->next->type);
    mx_ast_push_back(&q->left, p->next->args, p->type);
    // if (IS_REDIRECTION(p->next->type)) {
    //     t_ast *tmp = p->next;
    //     mx_ast_push_back_redirection(&q->left, &tmp);
    //     sep = tmp->next->type;
    // }
    q->type = sep;
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
