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
 * Convert int-value in delimeter.
 */
static char *get_delim_from_type(int type) {
    if (type == SEP)
        return mx_strdup(";");
    else if (type == FON)
        return mx_strdup("&");
    else if (type == AND)
        return mx_strdup("&&");
    else if (type == OR)
        return mx_strdup("||");
    else if (type == PIPE)
        return mx_strdup("|");
    else if (type == R_INPUT)
        return mx_strdup(">");
    else if (type == R_INPUT_DBL)
        return mx_strdup(">>");
    else if (type == R_OUTPUT)
        return mx_strdup("<");
    else if (type == R_OUTPUT_DBL)
        return mx_strdup("<<");
    return NULL;
}
/*
 * Get delimeter like a string and convert in int-value.
 */
static int get_delim(char *line, int *pos) {
    char *delim = NULL;
    int type = 0;

    if (line[0] && mx_isdelim(line[0], PARSE_DELIM)) {
        if (line[1] && mx_isdelim(line[1], PARSE_DELIM))
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

    if ((pos = mx_get_char_index_quote(&line[pos], PARSE_DELIM)) > 0) {
        tmp = mx_strndup(line, pos);
        *type = get_delim(line + pos, &pos);
        *i += pos;
    }
    else if (pos == 0) {        // in case >> << < >
        // tmp = mx_strdup("first_arg_is_empty");
        // *type = get_delim(line, &pos);
        // *i += pos;
        (*i)++;
    }
    else {
        tmp = mx_strdup(line);
        *type = SEP;
        *i += mx_strlen(line);
    }
    return tmp;
}
/*
 * Get list of all commands and delimeters (operators) -> use filters
 */
t_ast *mx_ush_parsed_line(char *line, t_export *variables) {
    t_ast *res = NULL;
    int type = 0;
    char *delim;
    char *tmp = NULL;
    char **args = NULL;

    if (mx_check_parce_errors(line))
        return NULL;
    int i = 0;
    while (line[i]) {
        if ((tmp = get_token_and_delim(&line[i], &i, &type))) {
            if ((args = mx_filters(tmp, variables)) && *args)
                mx_ast_push_back(&res, args, type);
            else if (type != SEP) {  // works "; ; ;", block "; | ;"
                mx_printerr("u$h: parse error near `");
                delim = get_delim_from_type(type);
                write(2, delim, mx_strlen(delim));
                mx_strdel(&delim);
                mx_printerr("\'\n");
                mx_ast_clear_list(&res);
                return NULL;
            }
            mx_del_strarr(&args);
        }
        mx_strdel(&tmp);
    }
    return res;
}
