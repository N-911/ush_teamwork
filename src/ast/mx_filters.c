#include "ush.h"
/*
 * Parse by USH_TOK_DELIM, subst ~, $, trim'' "" , \
 */
char **mx_filters(char *arg, t_export *variables) {
    int i = 0;
    // int ind = 0;
    char *tmp = mx_strdup(arg);
    char **args = mx_parce_tokens(arg);
    char **res = (char **)malloc((mx_strlen_arr(args) + 1) * sizeof(char *));

    if (mx_check_allocation_error((char **)&res))
        return NULL;
    for (; args[i]; i++) {
        res[i] = mx_strdup(args[i]);
        res[i] = mx_subst_tilde(res[i]);

        // if ((ind = mx_get_char_index(res[i], '\"')) >= 0)

             // проверить, изолирует ли все кроме $ ' '\'
            res[i] = mx_strtrim_quote(res[i], '\"');

        res[i] = mx_substr_dollar(res[i], variables);

        // if (ind = mx_get_char_index(res[i], '\'') >= 0)
            res[i] = mx_strtrim_quote(res[i], '\'');
    }
    res[i] = NULL;
    mx_strdel(&tmp);
    free(args);
    args = NULL;
    return res;
}
