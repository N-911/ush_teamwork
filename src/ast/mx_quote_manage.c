#include "ush.h"
/*
 *  Get char index outside of the quote
 */
int mx_get_char_index_quote(const char *str, char *c) {
    char *s = (char *)str;
    int i = 0;
    char tmp;

    if (!str || !*str)
        return -2;
    while (s[i]) {
        if ((s[i] == '\'' || s[i] == '"' || s[i] == '`')
            && (i == 0 || s[i - 1] != '\\')) {
            tmp = s[i];
            i++;
            while (s[i] && s[i] != tmp)
                i++;
        }
        for (int j = 0; j < mx_strlen(c); j++) {
            if (s[i] == c[j])
                return i;
        }
        i++;
    }
    return -1;
}
/*
 * Count chars (outside of the quote)
 */
int mx_count_chr_quote(const char *str, char *c) {
    int res = 0;
    int tmp = 0;
    const char *s = str;

    while (s && (tmp = mx_get_char_index_quote(s, c)) >= 0) {
        res++;
        s += tmp + 1;
    }
    return res;
}
/*
 * Trim first in quote
 */





char *mx_strtrim_quote(char *s, char q_char) {
    int i = 0;
    int j = mx_strlen(s) - 1;
    char *n;

    while ((i = mx_get_char_index(s)) > 0 )
        if
            i++;
    while (s[j] == q_char)
        j--;

    if (i < j) {
        n = mx_strnew(mx_strlen(s));
        for (j = 0; j < length; j++, i++) {
            while (s[i] && s[i] == q_char)
                i++;
            n[j] = s[i];
        }
        mx_strdel(&s);
        return n;
    }
    mx_strdel(&s);
    return malloc(0);  //or return NULL;
}
