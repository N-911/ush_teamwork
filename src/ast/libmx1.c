#include "ush.h"

char *mx_strjoin_free(char *s1, char const *s2) {
     char *p = NULL;

    if (!s1 && !s2)
        return NULL;
    else if (!s1)
        p = mx_strdup(s2);
    else if (!s2)
        p = mx_strdup(s1);
    else {
        p = mx_strnew(mx_strlen(s1) + mx_strlen(s2));
        if (!p)
            return NULL;
        p = mx_strcpy(p, s1);
        p = mx_strcat(p, s2);
    }
    mx_strdel(&s1);
    return p;
}

int mx_strlen_arr(char **s) {
    int i = 0;

    if (!s)
        return 0;
    while (s[i])
        i++;
    return i;
}

char **mx_strdup_arr(char **strarr) {
    char **res = NULL;
    int len;
    int i = 0;

    if (!strarr)
        return NULL;
    len = mx_strlen_arr(strarr);
    res = (char **)malloc((len + 1) * sizeof(char *));
    for (; i < len; i++) {
        res[i] = mx_strdup(strarr[i]);
    }
    res[i] = NULL;
    return res;
}

void mx_print_strarr_in_line(char **res, const char *delim) {
    if (!res || !delim)
        return;
    if (res[0] == NULL)
        mx_printstr("NULL\n");
    for (int i = 0; res[i]; i++) {
        mx_printstr(res[i]);
        mx_printstr(delim);
        if (res[i + 1] == NULL){
            mx_printstr("NULL\n");
        }
    }
}

void mx_set_buff_zero(void *s, size_t n) {
    size_t i = 0;
    char *p = (void*)s;

    if (!s)
        return;
    while (i < n) {
        p[i] = 0;
        i++;
    }
}
