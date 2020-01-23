#include "ush.h"

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
