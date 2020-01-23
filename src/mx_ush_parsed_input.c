#include "ush.h"

static bool mx_isdelim(char c, char *delim) {
    if (!c || !delim)
        return false;
    for (int i = 0; delim[i]; i++) {
        if (delim[i] == c)
            return true;
    }
    return false;
}

static char *get_delim(char *line) {
    if (mx_isdelim(line[0], PARSE_DELIM)) {
        if (mx_isdelim(line[1], PARSE_DELIM))
            return mx_strndup(line, 2);
        else
            return mx_strndup(line, 1);
    }
    return NULL;
}

static void push_back_zero(t_input **list, char *line, int *i_del) {
    char *delim = get_delim(line);
    char **tmp = mx_parce_tokens(" ");

    mx_ush_push_back(list, tmp, delim);
    *i_del = *i_del + mx_strlen(delim);
    mx_strdel(&delim);
}

static void push_back_all(t_input **list, char *line, char **args, int i_del) {
    char **tmp = NULL;
    char *delim;

    for (int i = 0; args[i]; i++) {
        i_del += mx_strlen(args[i]);
        tmp = mx_parce_tokens(args[i]);

        if (mx_strcmp(args[i], "\n") != 0) {
            if (i_del < mx_strlen(line))
                delim = get_delim(&line[i_del]);
            else
                delim = mx_strdup(";");
            mx_ush_push_back(list, tmp, delim);
            i_del += mx_strlen(delim);
            mx_strdel(&delim);
        }
        free(tmp);
        tmp = NULL;
    }
}

t_input *mx_ush_parsed_line(char *line) {
    char **args;
    t_input *res = NULL;
    int i_del = 0;

    if (mx_check_parce_errors(line))
        return NULL;
    args = mx_strsplit_quote(line, PARSE_DELIM);
    if (mx_isdelim(line[0], PARSE_DELIM))
        push_back_zero(&res, line, &i_del);
    push_back_all(&res, line, args, i_del);
    mx_del_strarr(&args);
    return res;
}
