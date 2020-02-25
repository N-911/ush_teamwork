#include "ush.h"
/*
 * Convert delimeter in int-value.
 */
static int get_type(char *delim) {
    if (MX_IS_SEP(delim))
        return SEP;
    else if (MX_IS_FON(delim))
        return FON;
    else if (MX_IS_AND(delim))
        return AND;
    else if (MX_IS_OR(delim))
        return OR;
    else if (MX_IS_PIPE(delim))
        return PIPE;
    else if (MX_IS_R_INPUT(delim))
        return R_INPUT;
    else if (MX_IS_R_INPUT_DBL(delim))
        return R_INPUT_DBL;
    else if (MX_IS_R_OUTPUT(delim))
        return R_OUTPUT;
    else if (MX_IS_R_OUTPUT_DBL(delim))
        return R_OUTPUT_DBL;
    return NUL;
}

/*
 * Get delimeter like a string and convert in int-value.
 */
static int get_delim(char *line, int *pos) {
    char *delim = NULL;
    int type = 0;

    if (line[0] && mx_isdelim(line[0], MX_PARSE_DELIM)) {
        if (line[1] && mx_isdelim(line[1], MX_PARSE_DELIM))
            delim = mx_strndup(line, 2);
        else
            delim = mx_strndup(line, 1);
    }
    type = get_type(delim);
    if (delim)
        *pos += mx_strlen(delim);
    mx_strdel(&delim);
    return type;
}
/*
 * Get one command and delimeter after it.
 */
static char *get_token_and_delim(char *line, int *i, int *type) {
    int pos = 0;
    char *tmp = NULL;

    if ((pos = mx_get_char_index_quote(&line[pos],
                                       MX_PARSE_DELIM, MX_QUOTE)) > 0) {
        tmp = mx_strndup(line, pos);
        *type = get_delim(line + pos, &pos);
        *i += pos;
    }
    else if (pos == 0)        // in case >> << < >
        (*i)++;
    else {
        tmp = mx_strdup(line);
        *type = SEP;
        *i += mx_strlen(line);
    }
    return tmp;
}
/*
 * Get list of all commands and delimeters (operators) -> use filters.
 */
t_ast *mx_ush_parsed_line(char *line, t_shell *m_s) {
    t_ast *res = NULL;
    int type = 0;
    int i = 0;
    char *tmp = NULL;
    char **args = NULL;

    if (mx_check_parce_errors(line))
        return NULL;
    while (line[i])
        if ((tmp = get_token_and_delim(&line[i], &i, &type))) {
            if ((args = mx_filters(tmp, m_s)) && *args)
                mx_ast_push_back(&res, args, type);
            else if (!args || type != SEP)
                return mx_parse_error_ush(type, res);
            mx_del_strarr(&args);
        }
    return res;
}
