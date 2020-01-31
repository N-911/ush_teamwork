#include "ush.h"

char **filter_one(char *arg) {
    int i = 0;
    char *tmp = mx_strdup(arg);
    char **args = mx_parce_tokens(arg);
    char **res = (char **)malloc((mx_strlen_arr(args) + 1) * sizeof(char *));

    for (; args[i]; i++) {
        res[i] = mx_strdup(args[i]);
        res[i] = mx_subst_tilde(res[i]);  // exp ~
        // substr $
        if (mx_get_char_index(res[i], '\'') >= 0
            || mx_get_char_index(res[i], '\"') >= 0)
            res[i] = mx_strtrim_quote(res[i]);
    }
    res[i] = NULL;
    mx_strdel(&tmp);
    free(args);
    args = NULL;
    return res;
}

void mx_filters(t_ast **ast) {
    for (int i = 0; ast[i]; i++) {
        for (t_ast *q = ast[i]; q; q = q->next) {
            q->args = filter_one(q->line);
        }
    }
}
