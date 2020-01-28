#include "ush.h"

static char **res_words(const char *s, char *c, int size) {
    int i = 0;
    int i_del = 0;
    char **res = (char **) malloc((size + 1) * sizeof(char *));

    for (int j = 0; s[j]; i++) {
        if ((i_del = mx_get_char_index_quote(&s[j], c)) > 0) {
            res[i] = mx_strndup(&s[j], i_del);
            j += i_del + 1;
        } else if (i_del == -1) {
            res[i] = mx_strdup(&s[j]);
            j += mx_strlen(&s[j]);
        } else if (i_del == 0) {
            j++;
            i--;
        }
    }
    res[i] = NULL;
    return res;
}

char **mx_strsplit_quote(const char *s, char *c) {
    int size;
    char **res;

    if (!s || !c)
        return NULL;
    size = mx_count_chr_quote(s, c) + 1;
    res = res_words(s, c, size);
    return res;
}
