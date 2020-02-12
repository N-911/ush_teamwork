#include "ush.h"
/*
 *  Get char index outside of the quote
 */
int mx_get_char_index_quote(char *s, char *c, char *quote) {
    char tmp;

    if (!s || !*s)
        return -2;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '\\')
            i++;
        else if (quote && mx_isdelim(s[i], quote)) {
            tmp = s[i];
            i++;
            while (s[i] && s[i] != tmp) {
                (s[i] == '\\') ? i += 2 : i++;
                // if (s[i] == '\\')
                //     i++;
                // i++;
            }
        }
        else
            for (int j = 0; j < mx_strlen(c); j++)
                if (s[i] == c[j])
                    return i;
    }
    return -1;
}
/*
 * Count chars (outside of the quote)
 */
int mx_count_chr_quote(char *str, char c, char *q) {
    int res = 0;
    int tmp = 0;
    char *s = str;

    while (s && (tmp = mx_get_char_index_quote(s, &c, q)) >= 0) {
        res++;
        s += tmp + 1;
    }
    return res;
}
/*
 * Trim first in quote
 */
char *mx_strtrim_quote(char *s, char c, char *q) {
    int newlen;
    char *n;

    if (!s || !*s || !c)
        return NULL;
    newlen = mx_strlen(s) - mx_count_chr_quote(s, c, q);
    if (newlen == mx_strlen(s))
        return s;
    else if (newlen > 0) {
        n = mx_strnew(newlen);
        for (int j = 0, i = 0; s[i] && j < newlen; j++, i++) {
            while (s[i] && s[i] == c)
                i++;
            n[j] = s[i];
        }
    } else
        n = malloc(0);
    mx_strdel(&s);
    return n;
}
