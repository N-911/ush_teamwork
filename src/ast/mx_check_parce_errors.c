#include "ush.h"
/*
 *  Check if ' or " even
 */
static bool check_quote(char *line) {
    char *quote = QUOTE;

    if (!line)
        return false;
    for (int i = 0; i < mx_strlen(quote); i++)
        if (mx_count_chr_ush(line, quote[i]) % 2 != 0) {
            mx_printerr("Unmatched ");
            mx_printerr(&quote[i]);
            mx_printerr(".\n");
            return true;
        }
    return false;
}
/*
 * Print parse error.
 */
static bool print_parse_error(char *c, int k) {
    mx_printerr("u$h: parse error near `");
    write(2, c, k);
    mx_printerr("\'\n");
    return true;
}
/*
 * Check wrong combinations of operators (like |> and |<>).
 * Check third delim like <<< >>> &&& ||| <<| ets.
 */
static bool check_parse_auditor(char *line, int i) {
    int i2;
    int i3;

    i2 = mx_get_char_index_quote(&line[i + 1], PARSE_DELIM, QUOTE);
    if (i2 == 0) {
        if (line[i] != line[i + 1] || line[i + 1] == ';'
        || (i == 0 && line[i] == line[i + 1]))
            return print_parse_error(&line[i + 1], 1);
        else if (line[i + 2]) {
            i3 = mx_get_char_index_quote(&line[i + 2], PARSE_DELIM, QUOTE);
            if (i3 == 0)
                return print_parse_error(&line[i + 2], 1);
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
        if ((i = mx_get_char_index_quote(line, PARSE_DELIM, QUOTE)) >= 0) {
            if ((line[i + 1] == '\0' && line[i] != ';' && line[i] != '&')
            || mx_strcmp(&line[i], "&&") == 0)
                return print_parse_error("\\n", 2);
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
 * Check all possible errors of parsing,
 * Check if line begins of delimeters "|&><"
 */
bool mx_check_parce_errors(char *line) {
    if (!line || check_quote(line) || check_parse(line))
        return true;

    if (line[0] && mx_isdelim(line[0], "|&><")) {
        if (line[1]) {
            if (!mx_isdelim(line[1], "|&><"))
                return print_parse_error(&line[0], 1);
            else
                return print_parse_error(&line[0], 2);
        }
    }
    return false;
}
