#include "ush.h"

static int count_len(char *s) {
    int res = 0;
    int len = mx_strlen(s);

    for (int i = 0; i < len; i++) {
        if (s[i] && s[i] == '\'' && (i == 0 || s[i - 1] != '\\'))
            while (s[i] != '\'')
                ++i;
        if (s[i] && s[i] == '\\') {
            res++;
            i++;
        }
    }
    return (len - res);
}
/*
 * Substitutiont backslashes.
 */
char *mx_substr_backslash(char *s) {
    int newlen;
    char *n;

    if (!s || !*s || (newlen = count_len(s)) == mx_strlen(s))
        return s;
    else if (newlen > 0) {
        n = mx_strnew(newlen);
        for (int j = 0, i = 0; s[i] && j < newlen; j++, i++) {
            if (s[i] && s[i] == '\'' && (i == 0 || s[i - 1] != '\\'))
                while (s[i] && s[++i] != '\'' && (j++) < newlen)
                    n[j] = s[i];
            if (s[i] && s[i] == '\\')
                i++;
            n[j] = s[i];
        }
    } else
        n = mx_strnew(0);
    mx_strdel(&s);
    return n;
}
