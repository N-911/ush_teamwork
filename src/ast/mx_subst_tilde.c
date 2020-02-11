#include "ush.h"
/*
*  check if prefix is correct login or not
*/
static char *add_login(char *s, char *home, char *prefix) {
    char *path = NULL;
    int i = mx_get_char_index_reverse(home, '/');
    struct stat buff;

    path = mx_strndup(home, i + 1);
    path = mx_strjoin_free(path, prefix);
    if (lstat(path, &buff) != 0) {
        // mx_printerr("u$h: no such user or named directory: ");
        // mx_printerr(prefix);
        // mx_printerr("\n");
        mx_strdel(&path);
        return NULL;
    }
    else {
        if ((i = mx_get_char_index(s, '/')) > 0)
            path = mx_strjoin_free(path, &s[i]);
        return path;
    }
}
/*
*  prefix - all after ~ and before char '/' or '\0' (example: ~mboiko/)
*/
static char *get_prefix(char *s) {
    char *prefix = NULL;
    int sleshpos = mx_get_char_index(&s[1], '/');

    if (s[1]) {
        if (sleshpos <= 0)
            prefix = mx_strdup(&s[1]);
        else
            prefix = mx_strndup(&s[1], sleshpos);
    }
    return prefix;
}
/*
*  substitutiont tilde in different ways
*/
static char *expantion(char *s) {
    char *res = NULL;
    char *prefix = get_prefix(s);
    char *home = getenv("HOME");

    if (prefix == NULL)
        res = mx_strdup(home);
    else if (prefix[0] == '/') {
        res = mx_strdup(home);
        res = mx_strjoin_free(res, prefix);
    }
    else if (mx_strcmp(prefix, "+") == 0)
        res = mx_strdup(getenv("PWD"));
    else if (mx_strcmp(prefix, "-") == 0)
        res = mx_strdup(getenv("OLDPWD"));
    else {
        res = add_login(s, home, prefix);
    }
    mx_strdel(&prefix);
    return res;
}
/*
*  substitutiont tilde
*/
char *mx_subst_tilde(char *s) {
    char *res = NULL;

    if (!s || !*s)
        return s;
    if (s[0] == '~') {
        res = expantion(s);
        if (res != NULL) {
            mx_strdel(&s);
            return res;
        }
    }
    return s;
}
