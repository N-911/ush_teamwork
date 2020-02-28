#include "ush.h"
/*
 * Substitution functions.
 */
static void func_or_push(t_ast **res, char **args, int type, t_shell *m_s) {
    t_export *q;

    for (q = m_s->functions; q; q = q->next)
        if (mx_strcmp(args[0], q->name) == 0) {
            *res = mx_ush_parsed_line(*res, q->value, m_s, type);
            return;
        }
    for (q = m_s->aliases; q; q = q->next)
        if (mx_strcmp(args[0], q->name) == 0) {
            *res = mx_ush_parsed_line(*res, q->value, m_s, type);
            return;
        }
    mx_ast_push_back(res, args, type);
}

static void if_recurcion_func_or_alias(t_ast **res, int old_t) {
    if (old_t) {
        t_ast *q = *res;
        while (q->next)
            q = q->next;
        q->type = old_t;
    }
}
/*
 * Get list of all commands and delimeters (operators) -> use filters.
 */
t_ast *mx_ush_parsed_line(t_ast *res, char *line1, t_shell *m_s, int old_t) {
    int type = 0;
    int i = 0;
    char *tmp = NULL;
    char *line;
    char **args = NULL;

    if (mx_check_parce_errors(line1))
        return NULL;
    line = mx_strdup(line1);
    while (line[i])
        if ((tmp = mx_get_token_and_delim(&line[i], &i, &type))) {
            // tmp = mx_subst_command(tmp);
            if ((args = mx_filters(tmp, m_s)) && *args)
                func_or_push(&res, args, type, m_s);
            else if (!args || type != SEP)
                return mx_parse_error_ush(type, res, line);
            mx_del_strarr(&args);
        }
    mx_strdel(&line);
    if_recurcion_func_or_alias(&res, old_t);
    return res;
}
