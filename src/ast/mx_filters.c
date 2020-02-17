#include "ush.h"
/*
 * Parse by USH_TOK_DELIM, subst ~, $, trim'' "" , \
 */
char **mx_filters(char *arg, t_export *variables) {
    int i;
    int j;
    char **args = mx_parce_tokens(arg);
    char **res = (char **)malloc((mx_strlen_arr(args) + 1) * sizeof(char *));

    for (i = 0, j = 0; args[j] && args[j][0]; i++, j++) {
        res[i] = mx_strdup(args[j]);
        res[i] = mx_subst_tilde(res[i]);                // res[i] if bad subst
        res[i] = mx_substr_dollar(res[i], variables);   // NULL if bad subst
        res[i] = mx_subst_command(res[i]);              // '\0' or NULL if bad subst

        if (!res[i]) {
            mx_del_strarr(&res);
            free(args);
            return NULL;
        }
        else if (res[i][0] == '\0') {
            mx_strdel(&res[i]);
            i--;
        }
    }
    res[i] = NULL;
    /*  scans the results of parameter expansion, command substitution,
     *  that did not double quoted for word splitting.
     */
    // mx_strtrim_quote(res);
    free(args);
    return res;
}
