#include "ush.h"
/*
 * Parse by USH_TOK_DELIM, subst ~, $, trim'' "" , \
 */
char **mx_filters(char *arg, t_export *variables) {
    int i;
    char **args = mx_parce_tokens(arg);
    char **res = (char **)malloc((mx_strlen_arr(args) + 1) * sizeof(char *));

    if (mx_check_allocation_error((char **)&res))
        return NULL;
    for (i = 0; args[i] && args[i][0]; i++) {
        res[i] = mx_strdup(args[i]);
        // res[i] = mx_subst_tilde(res[i]);
        // res[i] = mx_substr_dollar(res[i], variables);
        if (variables) {}
        // res[i] = mx_substr_backslash(res[i]);
        // res[i] = mx_strtrim_quote(res[i], '\"', "\'"); // проверить, изолирует ли все кроме $ ' '\'
        // res[i] = mx_strtrim_quote(res[i], '\'', NULL);
        // if (!res[i] || res[i][0] == '\0') {
        //     mx_strdel(&res[i]);
        //     i--;
        // }
    }
    res[i] = NULL;
    free(args);
    return res;
}
