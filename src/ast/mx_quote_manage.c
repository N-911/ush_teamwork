#include "ush.h"

static void mx_get_char_auditor(char *s, int *ii, char *q) {
    int i = *ii;
    char tmp;

    if ((mx_isdelim(s[i], q) && s[i] == '`')
        || (mx_isdelim(s[i], q) && s[i] == '\"')) {
        tmp = s[i];
        i++;
        while (s[i] && s[i] != tmp)
            (s[i] == '\\') ? (i += 2) : (i++);
    }
    else if (mx_isdelim(s[i], q) && s[i] == '\'') {
        i++;
        while (s[i] && s[i] != '\'')
            i++;
    }
    *ii = i;
}

/*
 * Get char index outside of the quote
 * search everywhere except ' ', " ", ` `, $( ).
 */
int mx_get_char_index_quote(char *s, char *c, char *q) {  // q = "\"\'`$"
    char tmp;

    if (!s || !c)
        return -2;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '\\')
            i++;
        else if (mx_isdelim(s[i], q) && !mx_strncmp(&s[i], "$(", 2)) {
            tmp = ')';
            i++;
            while (s[i] && s[i] != tmp)  // while (s[++i] && s[i] != tmp)   (s[i] == '\\') ? (i++) : (i);
                (s[i] == '\\') ? (i += 2) : (i++);
        }
        else if (mx_isdelim(s[i], q) && mx_isdelim(s[i], "`\'\""))
            mx_get_char_auditor(s, &i, q);
        else
            for (int j = 0; j < mx_strlen(c); j++)
                if (s[i] == c[j])
                    return i;
    }
    return -1;
}

static void mx_strtrim_quote_auditor(char *s, char *tmp, int *ii, int *jj) {
    int i = *ii;
    int j = *jj;

    if (s[i] == '\"') {
        i++;
        while (s[i] && s[i] != '\"') {
            (s[i] == '\\' && mx_isdelim(s[i + 1], DBLQ_EXCEPTIONS)) ? (i++) : (i);
            tmp[j++] = s[i++];
        }
        i++;
    }
    else if (s[i] == '\'') {
        i++;
        while (s[i] && s[i] != '\'')
            tmp[j++] = s[i++];
        i++;
    }
    *ii = i;
    *jj = j;
}
/*
 * Trim quote characters
 */
void mx_strtrim_quote(char **str) {
    char *tmp = NULL;
    int i = 0;
    int j = 0;

    for (char *s = *str; s; s++) {
        tmp = mx_strnew(mx_strlen(s));
        while (s[i]) {
            if (s[i] && s[i] == '\\') {
                i++;
                tmp[j++] = s[i++];
            }
            else if (s[i] && (s[i] == '\"' || s[i] == '\''))
                mx_strtrim_quote_auditor(s, tmp, &i, &j);
            else
                tmp[j++] = s[i++];
        }
        mx_strdel(&s);
        s = tmp;
    }
}
