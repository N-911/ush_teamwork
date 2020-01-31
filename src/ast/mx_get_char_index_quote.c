#include "ush.h"

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
