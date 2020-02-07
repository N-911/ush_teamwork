#include "ush.h"
/*
 *  Get char index outside of the quote
 */
int mx_get_char_index_quote(const char *str, char *c, char *quote) {
    char *s = (char *)str;
    int i = 0;
    char tmp;

    if (!str || !*str)
        return -2;
    while (s[i]) {
        for (int k = 0; k < mx_strlen(quote); k++) {
            if (s[i] == quote[k] && (i == 0 || s[i - 1] != '\\')) {
                tmp = s[i];
                i++;
                while (s[i] && s[i] != tmp)
                    i++;
            }
        }
        for (int j = 0; j < mx_strlen(c); j++)
            if (s[i] == c[j] && (i == 0 || s[i - 1] != '\\'))
                return i;
        i++;
    }
    return -1;
}
/*
 * Get char index except isolated with '\'
 */
int mx_get_char_index_ush(const char *str, char c) {
    char *s = (char *)str;
    int i = 0;

    if (!str || !*str)
        return -2;
    while (s[i]) {
        if (s[i] == c && (i == 0 || s[i - 1] != '\\'))
            return i;
        i++;
    }
    return -1;
}
/*
 * Count chars (outside of the quote)
 */
int mx_count_chr_quote(const char *str, char *c, char *q) {
    int res = 0;
    int tmp = 0;
    const char *s = str;

    while (s && (tmp = mx_get_char_index_quote(s, c, q)) >= 0) {
        res++;
        s += tmp + 1;
    }
    return res;
}
/*
 * Count chars, except isolated with \
 */
int mx_count_chr_ush(const char *str, char c) {
    int res = 0;
    int tmp = 0;
    const char *s = str;

    while (s && (tmp = mx_get_char_index_ush(s, c)) >= 0) {
        res++;
        s += tmp + 1;
    }
    return res;
}

/*
 * Trim first in quote
 */
// char *mx_strtrim_quote(char *s, char q_char) {
//     int i = 0;
//     int j = mx_strlen(s) - 1;
//     char *n;

//     while ((i = mx_get_char_index(s)) > 0 )
//         if
//             i++;
//     while (s[j] == q_char)
//         j--;

//     if (i < j) {
//         n = mx_strnew(mx_strlen(s));
//         for (j = 0; j < length; j++, i++) {
//             while (s[i] && s[i] == q_char)
//                 i++;
//             n[j] = s[i];
//         }
//         mx_strdel(&s);
//         return n;
//     }
//     mx_strdel(&s);
//     return malloc(0);  //or return NULL;
// }

static int get_new_len_and_indexes(const char *s, int *i, int *j, char q_ch){
    int length;
    int gi = *i;
    int gj = *j;

    gj = mx_strlen(s) - 1;
    length = gj + 1 - mx_count_chr_ush(s, q_ch);
    while (s[gi] == q_ch)
        (gi)++;
    while (s[gj] == q_ch)
        gj--;
    *i = gi;
    *j = gj;
    return length;
}
char *mx_strtrim_quote(char *s, char q_char) {
    int i = 0;
    int j;
    int length;
    char *n;

    length = get_new_len_and_indexes(s, &i, &j, q_char);
    if (i <= j && length > 0) {
        n = mx_strnew(length);
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
