#include "ush.h"
/*
 * Print parse error.
 */
static bool parse_er(char *c, int k) {
    mx_printerr("u$h: parse error near `");
    write(2, c, k);
    mx_printerr("\'\n");
    return true;
}
/*
 * Print unmached error.
 */
static bool unmached_er(char c) {
    mx_printerr("Unmatched ");
    write(2, &c, 1);
    mx_printerr(".\n");
    return true;
}
/*
 *  check_quote split by auditor ))))
 */
static char check_quote_auditor(char *s, int *i) {
    int j = *i;
    char tmp;

    if (s[j] == ')')
        return ')';
    (s[j] == '(') ? (tmp = ')') : (tmp = s[j]);
    j++;
    while (s[j] && s[j] != tmp)
        (s[j] == '\\') ? (j += 2) : (j++);
    *i = j;
    (tmp == ')') ? (tmp = '(') : (tmp);
    return tmp;
}
/*
 *  Check if '', "", ``, () even
 */
static bool check_quote(char *s) {
    char tmp;

    if (!s || !*s)
        return false;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '\\')
            i++;
        else if (mx_isdelim(s[i], QUOTE)) {
            tmp = check_quote_auditor(s, &i);
            if (!s[i] || tmp == ')')
                return unmached_er(tmp);
        }
        else if (s[i] == '\'') {
            i++;
            while (s[i] && s[i] != '\'')
                i++;
            if (!s[i])
                return unmached_er('\'');
        }
    }
    return false;
}
/*
 * Check wrong combinations of operators (like |> and |<>).
 * Check third delim like <<< >>> &&& ||| <<| ets.
 */
static bool check_parse_auditor(char *line, int i) {
    int i2;
    int i3;

    i2 = mx_get_char_index_quote(&line[i + 1], PARSE_DELIM);
    if (i2 == 0) {
        if (line[i] != line[i + 1] || line[i + 1] == ';')
            return parse_er(&line[i + 1], 1);
        else if (line[i + 2]) {
            i3 = mx_get_char_index_quote(&line[i + 2], PARSE_DELIM);
            if (i3 == 0)
                return parse_er(&line[i + 2], 1);
        }
    }
    return false;
}
/*
 * Check operator at the end (in there no cmd after operator) .\n ..\n
 */
static bool check_parse(char *line) {
    int i = 0;

    while (line) {
        if ((i = mx_get_char_index_quote(line, PARSE_DELIM)) >= 0) {
            if ((line[i + 1] == '\0' && line[i] != ';' && line[i] != '&')
            || mx_strcmp(&line[i], "&&") == 0)
                return parse_er("\\n", 2);
            if (line[i + 1])
                if (check_parse_auditor(line, i))
                    return true;
            line += i + 1;
        }
        else
            break;
    }
    return false;
}
/*
 * Check if line begins of delimeters "|&><".
 */
static bool check_first_delim(char *line) {
    if (line[0] && mx_isdelim(line[0], "|&><")) {
        if (line[1] && mx_isdelim(line[1], "|&><"))
            return parse_er(&line[0], 2);
        else
            return parse_er(&line[0], 1);
    }
    return false;
}
/*
 * Check all possible errors of parsing.
 */
bool mx_check_parce_errors(char *line) {
    if (!line
        || check_quote(line)
        || check_parse(line)
        || check_first_delim(line))
        return true;
    return false;
}
