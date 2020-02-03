#include "ush.h"
/*
*  convert delimeter in int-value
*/
static int get_type(char *delim) {
    if (!delim)
        return NUL;
    if (IS_SEP(delim))
        return SEP;
    else if (IS_FON(delim))
        return FON;
    else if (IS_AND(delim))
        return AND;
    else if (IS_OR(delim))
        return OR;
    else if (IS_PIPE(delim))
        return PIPE;
    else if (IS_R_INPUT(delim))
        return R_INPUT;
    else if (IS_R_INPUT_DBL(delim))
        return R_INPUT_DBL;
    else if (IS_R_OUTPUT(delim))
        return R_OUTPUT;
    else if (IS_R_OUTPUT_DBL(delim))
        return R_OUTPUT_DBL;
    return NUL;
}
/*
*  get delimeter like a string and convert in int-value
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
*  get one command and delimeter after it
*/
static char *get_token_and_delim(char *line, int *i, int *type) {
    int pos = 0;
    char *tmp = NULL;

    if ((pos = mx_get_char_index_quote(&line[pos], PARSE_DELIM)) > 0) {
        tmp = mx_strndup(line, pos);
        *type = get_delim(line + pos, &pos);
        *i += pos;
    }
    else if (pos == 0) {  // in case >> << < >
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
*  get list of all commands and delimeters (operators)
*/
t_ast *mx_ush_parsed_line(char *line) {
    t_ast *res = NULL;
    int type = 0;
    char *tmp = NULL;

    if (mx_check_parce_errors(line))
        return NULL;
    int i = 0;
    while (line[i]) {
        if ((tmp = get_token_and_delim(&line[i], &i, &type)))
            mx_ast_push_back(&res, tmp, type);
        mx_strdel(&tmp);
    }
    return res;
}
