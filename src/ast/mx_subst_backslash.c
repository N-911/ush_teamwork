#include "ush.h"
/*
 * Substitutiont backslashes.
 */
char *mx_substr_backslash(char *s) {
    char *n;
    int j = 0;
    int i = 0;

    if (!s || !*s || mx_get_char_index_quote(s, "\\", "\'") < 0)
        return s;
    n = mx_strnew(mx_strlen(s));
    while (s[i]) {
        if (s[i] && s[i] == '\\')
            n[j++] = s[++i];
        else if (s[i] && s[i] == '\'') {
            n[j++] = s[i++];
            while (s[i] != '\'')
                n[j++] = s[i++];
            n[j++] = s[i++];
        }
        else if (s[i] && s[i] == '\"') {
            n[j++] = s[i++];
            while (s[i] != '\"') {
                if (s[i] && s[i] == '\\' && s[i + 1]
                    && (s[i + 1] == '$' || s[i + 1] == '\"'
                    || s[i + 1] == '`' || s[i + 1] == '\\'))
                    n[j++] = s[++i];
                n[j++] = s[i++];
            }
            n[j++] = s[i++];
        }
        else
            n[j++] = s[i++];
    }
    while (j < mx_strlen(s))
        n[j++] = '\0';
    mx_strdel(&s);
    return n;
}
