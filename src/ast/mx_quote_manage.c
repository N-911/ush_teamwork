#include "ush.h"
/*
*  get char index (outside of the quote)
*/
int mx_get_char_index_quote(const char *str, char *c) {
    char *s = (char *)str;
    int i = 0;
    char tmp;

    if (!str || !*str)
        return -2;
    while (s[i]) {
        if (s[i] == '\'' || s[i] == '\"') {
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
*  count chars (outside of the quote)
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
*  get new lenght of trimmed str
*  and positions of start and end correct str chars
*/
static int get_new_len_and_indexes(const char *s, int *i, int *j) {
    int length;
    int gi = *i;
    int gj = *j;

    gj = mx_strlen(s) - 1;
    length = gj + 1 - mx_count_substr(s, "\'") - mx_count_substr(s, "\"");
    while (s[gi] == '\'' || s[gi] == '\"')
        (gi)++;
    while (s[gj] == '\'' || s[gj] == '\"')
        gj--;
    *i = gi;
    *j = gj;
    return length;
}
/*
*  trim all ' and " in quote
*/
char *mx_strtrim_quote(char *s) {
    int i = 0;
    int j;
    int length = get_new_len_and_indexes(s, &i, &j);
    char *n;

    if (i <= j && length > 0) {
        n = mx_strnew(length);
        for (j = 0; j < length; j++, i++) {
            while (s[i] && (s[i] == '\'' || s[i] == '\"'))
                i++;
            n[j] = s[i];
        }
        mx_strdel(&s);
        return n;
    }
    mx_strdel(&s);
    return malloc(0);  //or return NULL;
}
