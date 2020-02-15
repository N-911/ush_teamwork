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
        res[i] = mx_subst_tilde(res[i]);                // res[i] if bad subst
        res[i] = mx_substr_dollar(res[i], variables);   // NULL if bad subst
        res[i] = mx_subst_command(res[i]);
        /*  command subst;
         *  $ and ` ` inside " "
         *  scans the results of parameter expansion, command substitution,
         *  that did not double quoted for word splitting.
         */

        // res[i] = mx_strtrim_quote(res[i]);

        // if (res[i][0] == '\0') {
        //     mx_strdel(&res[i]);
        //     i--;
        // }
        if (!res[i]) {
            mx_del_strarr(&res);
            free(args);
            return NULL;
        }
    }
    res[i] = NULL;


    free(args);
    return res;
}
