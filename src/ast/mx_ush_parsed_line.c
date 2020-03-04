#include "ush.h"
/*
 * Substitution functions.
 */
// static void func_or_push(t_ast **res, char **args, int type, t_shell *m_s) {
//     t_export *q;

//     for (q = m_s->functions; q; q = q->next)
//         if (mx_strcmp(args[0], q->name) == 0) {
//             *res = mx_ush_parsed_line(*res, q->value, m_s, type);
//             return;
//         }
//     for (q = m_s->aliases; q; q = q->next)
//         if (mx_strcmp(args[0], q->name) == 0) {
//             *res = mx_ush_parsed_line(*res, q->value, m_s, type);
//             return;
//         }
//     mx_ast_push_back(res, args, type);
// }

// static void if_recurcion_func_or_alias(t_ast **res, int old_t) {
//     if (old_t) {
//         t_ast *q = *res;
//         while (q->next)
//             q = q->next;
//         q->type = old_t;
//     }
// }

static bool isempty(char *s, char *delim) {
    if (!s || !delim)
        return true;
    for (int i = 0; s[i]; i++) {
        if (!mx_isdelim(s[i], delim))
            return false;
    }
    return true;
}
/*
 * Get list of all commands and delimeters (operators) -> use filters.
 */
t_ast *mx_ush_parsed_line(t_ast *res, char *line1, t_shell *m_s, int old_t) {
    int type = 0;
    int i = 0;
    char *tmp = NULL;
    char *line;
    // char **args = NULL;

    if (m_s && old_t) {}  /// можливо не потрібно

    if (mx_check_parce_errors(line1))
        return NULL;
    line = mx_strdup(line1);  /// можливо не потрібно
    while (line[i])
        if ((tmp = mx_get_token_and_delim(&line[i], &i, &type))) {
            if (!isempty(tmp, MX_USH_TOK_DELIM))
                mx_ast_push_back(&res, tmp, type);
            else if (type != SEP)
                return mx_parse_error_ush(type, res, line);

            //     return mx_parse_error_ush(type, res, line);
            // if ((args = mx_filters(tmp, m_s)) && *args)
            //     func_or_push(&res, args, type, m_s);
            // else if (!args || type != SEP)
            //     return mx_parse_error_ush(type, res, line);
            // mx_del_strarr(&args);
        }
    mx_strdel(&line);
    // if_recurcion_func_or_alias(&res, old_t);
    return res;
}
