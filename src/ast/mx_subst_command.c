#include "ush.h"
/*
 *  Command substitutiont.
 */
char *mx_subst_command(char *s) {
    char *res = NULL;
    int pos = 0;

    if (!s || !*s)
        return s;
    // while ((pos = mx_get_char_index_quote(res, "`$", QUOTE2)) == 0)
    //     if (!(res = expantion(res, variables, pos))) {
    //         mx_printerr("u$h: bad substitution");
    //         return NULL;
    //     }
    // return s;
}
