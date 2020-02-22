#include "ush.h"
/*
 * check if result of subst is correct (NULL or '\0')
 */
static bool check_subsut_result(char **res, char **args, int *i) {
    if (!res[*i]) {
        mx_del_strarr(&res);
        free(args);
        return true;
    }
    else if (res[*i][0] == '\0') {
        mx_strdel(&res[*i]);
        (*i)--;
    }
    return false;
}

/*
 * Parse by USH_TOK_DELIM, subst ~, $, trim'' "" , \
 *
 * need scans for word splitting results of parameter expansion
 * and command substitution, that did not double quoted.
 *
 * reterns:
 * subst_tilde      result or res[i] if bad subst
 * substr_dollar    result, '\0' or NULL if bad subst
 * subst_command    '\0' or NULL if bad subst
 */
char **mx_filters(char *arg, t_export *variables) {
    int i;
    int j;
    char **args = mx_parce_tokens(arg);
    char **res = (char **)malloc((mx_strlen_arr(args) + 1) * sizeof(char *));

    for (i = 0, j = 0; args[j] && args[j][0]; i++, j++) {
        res[i] = mx_strdup(args[j]);
        res[i] = mx_subst_tilde(res[i], variables);
        res[i] = mx_substr_dollar(res[i], variables);
        res[i] = mx_subst_command(res[i]);
        if (check_subsut_result(res, args, &i))
            return NULL;
    }
    res[i] = NULL;
    mx_strtrim_quote(res);
    free(args);
    return res;
}
