#include "ush.h"
/*
 * Get variable name from string.
 */
static char *get_var(char *s, int *v_len) {
    char *var = NULL;
    int i = 0;

    if (!s)
        return NULL;
    if (s[0] == '{') {
        i = mx_get_char_index_quote(s, "}", NULL);
        if (i < 0)
            return NULL;
        var = mx_strndup(&s[1], i - 1);
        *v_len = mx_strlen(var) + 2;
    }
    else {
        while (s[i] && !mx_isspace(s[i]) && !mx_isdelim(s[i], "\'`\\"))
            i++;
        if (i != 0)
            var = mx_strndup(s, i);
        *v_len = i;
    }
    return var;
}
/*
 * Get value from variables.
 */
static char *get_value(char *var, t_export *variables) {
    for (t_export *q = variables; q; q = q->next)
        if (mx_strcmp(var, q->name) == 0)
            return q->value;
    return NULL;
}
/*
 * Combine new string.
 */
static char *expantion(char *s, t_export *variables, int pos) {
    char *res = NULL;
    int v_len = 0;
    char *var;
    char *value;

    if (!s)
        return NULL;
    res = mx_strndup(s, pos);
    if ((var = get_var(&s[pos + 1], &v_len))) {
        if ((value = get_value(var, variables)))
            res = mx_strjoin_free(res, value);
        mx_strdel(&var);
    }
    else
        mx_strdel(&res);
    if (res && s[pos + v_len + 1])
        res = mx_strjoin_free(res, &s[pos + v_len + 1]);
    mx_strdel(&s);
    return res;
}

static char *exp_inside_dblq(char *s, t_export *var, int *i, int *k) {
    int j = 0;
    int pos = 0;
    char *tmp;
    char *res = s;

    (*i) += *k + 1;
    res = mx_strndup(s, *i);
    j = mx_get_char_index_quote(&s[*i], "\"", "`");
    tmp = mx_strndup(&s[*i], j);
    while (tmp && (pos = mx_get_char_index_quote(tmp, "$", "`")) >= 0)
        tmp = expantion(tmp, var, pos);
    res = mx_strjoin_free(mx_strjoin_free(res, tmp), &s[*i + j]);
    (*i) += mx_strlen(tmp) + 1;
    mx_strdel(&tmp);
    mx_strdel(&s);
    return res;
}
/*
 * Substitutiont dollar from variables.
 */
char *mx_substr_dollar(char *s, t_export *variables) {
    char *res = s;
    int pos = 0;
    int i = 0;
    int k = 0;

    if (!s || !*s || !variables || mx_strcmp(s, "$") == 0)
        return s;
    while (s[i]) {
        if ((k = mx_get_char_index_quote(&s[i], "\"", "\'`$")) >= 0)
            res = exp_inside_dblq(res, variables, &i, &k);
        else
            break;
    }
    if (!res) {
        mx_printerr("u$h: bad substitution\n");
        return NULL;
    }
    while ((pos = mx_get_char_index_quote(res, "$", MX_QUOTE)) >= 0)
        // res = expantion(res, variables, pos);
        if (!(res = expantion(res, variables, pos))) {
            mx_printerr("u$h: bad substitution\n");
            return NULL;
        }
    return res;
}
