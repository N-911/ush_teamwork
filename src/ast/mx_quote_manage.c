#include "ush.h"
/*
 *  Get char index outside of the quote
 */
int mx_get_char_index_quote(char *s, char *c, char *quote) {
    char tmp;

    if (!s || !*s)
        return -2;
    for (int i = 0; s[i]; i++) {
        if (quote)
            for (int k = 0; quote && k < mx_strlen(quote); k++)     // check for segf
                if (s[i] == quote[k] && (i == 0 || s[i - 1] != '\\')) {
                    tmp = s[i];
                    i++;
                    while (s[i] && s[i] != tmp)
                        i++;
                }
        for (int j = 0; j < mx_strlen(c); j++)
            if (s[i] == c[j] && (i == 0 || s[i - 1] != '\\'))
                return i;
    }
    return -1;
}
/*
 *  Get char index outside of the quote reverse
 */
int mx_get_char_index_quote_reverse(char *s, char *c, char *q) {
    char tmp;

    if (!s || !*s)
        return -2;
    for (int i = mx_strlen(s) - 1; s[i]; i--) {
        if (q)
            for (int k = 0; k < mx_strlen(q); k++)
                if (s[i] == q[k] && q[k] && (i == 0 || s[i - 1] != '\\')) {
                    tmp = s[i];
                    i--;
                    while (s[i] && s[i] != tmp)
                        i--;
                }
        for (int j = 0; j < mx_strlen(c); j++)
            if (s[i] == c[j] && (i == 0 || s[i - 1] != '\\'))
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
