#include "ush.h"

static bool isdelim(char c) {
    char *delim = USH_TOK_DELIM;

    for (int i = 0; i < mx_strlen(delim); i++)
        if (c == delim[i])
            return true;
    return false;
}

static char *get_end_func(char *end) {
    int tmp = mx_get_char_index(end, ';');

    if (mx_strncmp(end, " () { ", 6) == 0 && tmp > -1 && end[tmp + 1] == '}') {
        end += tmp + 2;
        if (end && *end == ';')
            end++;
        return end;
    }
    mx_printerr_red("usage: func_name () { func_body;}");
    return NULL;
}

static char *get_end_usual_quote_func(char *s, const char *delim, char *end) {
    if (*s == '\'') {  // Find the end of the token.
        end = s + mx_get_char_index(s + 1, '\'') + 2;
        if (!isdelim(*end))
            end += strcspn(end, delim);
    }
    else
        end = s + strcspn(s, delim);
    if (mx_strncmp(end, " ()", 3) == 0)  // Find the end of the function.
        end = get_end_func(end);
    return end;
}

static char *strtok_tmp (char *s, const char *delim, char **save_ptr) {
    char *end = NULL;

    if (s == NULL)
        s = *save_ptr;
    s += strspn(s, delim);  // Scan leading delimiters.
    if (*s == '\0') {
        *save_ptr = s;
        return NULL;
    }
    if ((end = get_end_usual_quote_func(s, delim, end)) == NULL)
        return NULL;
    if (*end == '\0') {  // If it's last token, int tne end of the str.
        *save_ptr = end;
        return s;
    }
    *end = '\0';  // Terminate the token.
    *save_ptr = end + 1;
    return s;
}

char *mx_strtok (char *s, const char *delim) {
        static char *olds;

        return strtok_tmp (s, delim, &olds);
    }
