#include "ush.h"
/*
*  count separators of first lewel (;, && , ||)
*/
static int count_sep_first_lwl(t_ast *q) {
    int i = 1;

    for (; q; q = q->next)
        if (IS_SEP_FIRST_LWL(q->type))
            i++;
    return i;
}
/*
*  create ast (array of lists) from parsed_line (list)
*/
t_ast **mx_ast_parse(t_ast *parsed_line) {
    t_ast *q = parsed_line;
    int k = count_sep_first_lwl(q);
    t_ast **ast = (t_ast **)malloc((k + 1) * sizeof(t_ast *));
    int i = 0;

    ast[i] = NULL;
    for (; q; q = q->next) {
        if (IS_REDIRECTION(q->type)) {
            mx_ast_push_back_redirection(&ast[i], &q);
            for (; IS_REDIRECTION(q->type); q = q->next) {}
        }
        else
            mx_ast_push_back(&ast[i], q->args, q->type);
        if (IS_SEP_FIRST_LWL(q->type) || q->type == NUL)
            ast[++i] = NULL;
    }
    return ast;
}
