#include "ush.h"

static char *usage_err(void) {
    mx_printerr("u$h: function usage: func() { ...; }\n");
    return NULL;
}

static char *get_var(char *line, int i) {
    char *tmp = mx_strndup(line, i);
    char *var = mx_strtrim(tmp);

    mx_strdel(&tmp);
    if (mx_get_char_index_quote(var, MX_USH_TOK_DELIM, NULL) >= 0) {
        mx_strdel(&var);
        return mx_syntax_error("(");
    }
    return var;
}

static char *get_value(char *line, int end) {
    int i = end;
    char *value = NULL;

    if (!line[i] || mx_strncmp(&line[i], " { ", 3) != 0)
        return usage_err();
    i += 3;
    end = mx_get_char_index_quote(&line[i], "}", MX_QUOTE);
    if (end <= 0)
        return mx_syntax_error("{");
    value = mx_strndup(&line[i], end);
    return value;
}

/*
 * Get functions if any.
 */
bool mx_get_functions(char *line, t_shell *m_s) {
    char *var;
    char *value;
    int i = mx_get_char_index_quote(line, "(", MX_QUOTE);
    int end = mx_get_char_index_quote(&line[i], ")", MX_QUOTE);

    if (end == 1) {
        if (!(var = get_var(line, i)))
            return true;
        if (!(value = get_value(line, i + 2))) {
            mx_strdel(&var);
            return true;
        }
        mx_push_export(&m_s->functions, var, value);
        mx_strdel(&var);
        mx_strdel(&value);
        return true;
    }
    return false;
}
