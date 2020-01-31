#include "ush.h"

static int get_indexes(const char *s, int *i, int *j) {
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

char *mx_strtrim_quote(char *s) {
    int i = 0;
    int j;
    int length = get_indexes(s, &i, &j);
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
