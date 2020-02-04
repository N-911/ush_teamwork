#include "ush.h"

static char *get_var(char *s, int *v_len) {
    char *var = NULL;
    int i = 0;

    if (!s)
        return NULL;
    if (s[0] == '{') {
        var = mx_strndup(&s[1], mx_get_char_index(s, '}') - 1);
        *v_len = mx_strlen(var) + 2;
    }
    else {
        while (s[i] && !mx_isspace(s[i]))  // mx_isdelim
            i++;
        if (i != 0)
            var = mx_strndup(s, i);
        *v_len = i;
    }
    return var;
}

static char *get_value(char *var, t_export *variables) {

    for (t_export *q = variables; q; q = q->next)
        if (mx_strcmp(var, q->name) == 0)
            return q->value;
    return NULL;
}

static char *expantion(char *s, t_export *variables, int pos) {
    char *res = NULL;
    int v_len = 0;
    char *var;
    char *value;

    res = mx_strndup(s, pos);
    if ((var = get_var(&s[pos + 1], &v_len)))
        if ((value = get_value(var, variables)))
            res = mx_strjoin_free(res, value);
    if (s[pos + v_len])
        res = mx_strjoin_free(res, &s[pos + 1 + v_len]);
    mx_strdel(&s);
    return res;
}

char *mx_substr_dollar(char *s, t_export *variables) {
    char *res = s;
    int pos = 0;

    if (!s || !variables)
        return NULL;
    if (mx_strcmp(s, "$") == 0)
        return s;
    while ((pos = mx_get_char_index_quote(res, "$")) >= 0) {
        res = expantion(res, variables, pos);
    return res;
}
