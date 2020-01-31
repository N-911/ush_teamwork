#include "ush.h"

static int count_sep_first_lwl(t_ast *q) {
    int i = 1;

    for (; q; q = q->next)
        if (IS_SEP_FIRST_LWL(q->type))
            i++;
    return i;
}

t_ast **mx_ast_parse(t_ast *parsed_line) {
    t_ast *q = parsed_line;
    int k = count_sep_first_lwl(q);
    t_ast **ast = (t_ast **)malloc((k + 1) * sizeof(t_ast *));
    int i = 0;

    ast[i] = NULL;
    for (; q; q = q->next) {
        mx_ast_push_back(&ast[i], q->line, q->type);
        if (IS_SEP_FIRST_LWL(q->type) || q->type == NUL)
            ast[++i] = NULL;
    }
    return ast;
}
