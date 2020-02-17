#include "ush.h"
/*
 * Get substitution.
 */
static char *get_subst(char *s, int *len) {
    char *subst = NULL;
    int i = 0;

    if (s[0] == '$' && s[1] == '(') {
        i = mx_get_char_index_quote(s, ")", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[2], i - 1);
        *len = mx_strlen(subst) + 3;
    }
    else if (s[0] == '`') {
        i = mx_get_char_index_quote(&s[1], "`", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[1], i - 1);
        *len = mx_strlen(subst) + 2;
    }
    return subst;
}
/*
 * Combine new string.
 */
static char *expantion(char *s, int pos) {
    char *res = NULL;
    int len = 0;
    char *subst;

    res = mx_strndup(s, pos);
    if ((subst = get_subst(&s[pos], &len))) {
        mx_printerr("u$h: command substitushion doesn't work.\n");
        // res = mx_strjoin_free(res, subst);
    }
    if (s[pos + len])
        res = mx_strjoin_free(res, &s[pos + 1 + len]);
    mx_strdel(&s);
    return res;
}
/*
 *  Command substitutiont.
 */
char *mx_subst_command(char *s) {
    char *res = s;
    int pos = 0;

    if (!s || !*s)
        return s;
    while ((pos = mx_get_char_index_quote(res, "`$", "\'")) >= 0)
        res = expantion(res, pos);
    return res;
}
