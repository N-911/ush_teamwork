#include "ush.h"

char *mx_strtrim_quote(const char *s) {
    int i = 0;
    int j;
    int length;
    char *n;

    if (!s)
        return NULL;
    j = mx_strlen(s) - 1;
    length = j + 1 - mx_count_substr(s, "\'") - mx_count_substr(s, "\"");
    for (; s[i] == '\'' || s[i] == '\"'; i++)  // check chars from start
        for (; s[j] == '\'' || s[j] == '\"'; j--)
            if (i <= j && length != 0) {
                n = mx_strnew(length);
                for (j = 0; j < length; j++, i++)
                    for (; s[i] == '\'' || s[i] == '\"'; i++)
                        n[j] = s[i];
                return n;
            }
    return malloc(0);
}
