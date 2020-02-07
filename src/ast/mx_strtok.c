#include "ush.h"
/*
*  get end of function
*/
// static char *get_end_func(char *end) {
//     int tmp = mx_get_char_index_ush(end, '}');
//     mx_printstr(end);
//     mx_printint(tmp);

//     if (mx_strncmp(end, " () { ", 6) == 0 && tmp > 0) {
//         end += tmp + 2;
//         if (end && *end == ';')
//             end++;
//         return end;
//     }
//     else
//         mx_printerr_red("usage: function_name () { func_body;}\n");
//     return NULL;
// }
/*
*  get end of simple token, or quote, or function
*/
static char *get_end_usual_quote_func(char *s, const char *delim, char *end) {
    char tmp;

    while (*s) {
        if (mx_isdelim(*s, QUOTE)) {
            tmp = *s;
            s += mx_get_char_index_ush(s + 1, tmp) + 2;
        }
        else if (mx_isdelim(*s, (char *)delim))
            break;
        s++;
    }
    end = s;
    // if (mx_strncmp(end, " ()", 3) == 0)  // Find the end of the function.
    //     end = get_end_func(end);
    return end;
}
/*
*  get one token (cut it with '\0' in the end)
*/
static char *strtok_tmp (char *s, const char *delim, char **save_ptr) {
    char *end = NULL;

    if (s == NULL)
        s = *save_ptr;
    s += strspn(s, delim);  // Scan leading delimiters.
    if (*s == '\0') {
        *save_ptr = s;
        return NULL;
    }
    if (!(end = get_end_usual_quote_func(s, delim, end)))
        return NULL;
    if (*end == '\0') {  // If it's last token, int tne end of the str.
        *save_ptr = end;
        return s;
    }
    *end = '\0';  // Terminate the token.
    *save_ptr = end + 1;
    return s;
}
/*
*  return one token, but save rest of the line in static
*/
char *mx_strtok (char *s, const char *delim) {
        static char *olds;

        return strtok_tmp (s, delim, &olds);
    }
