#include "ush.h"

/*
*  check if error of ' or " % 2 == 0
*/
static bool check_quote(char *line) {
    if (!line)
        return false;
    else if (mx_count_substr(line, "\'") % 2 != 0) {
        mx_printerr("Unmatched '.\n");
        return true;
    } else if (mx_count_substr(line, "\"") % 2 != 0) {
        mx_printerr("Unmatched \".\n");
        return true;
    } else if (mx_count_chr_quote(line, "&") % 2 != 0) {
        mx_printerr("Unmatched &. Usage: &&\n");
        return true;
    }
    return false;
}

/*
*  check for error:
*  - of third <<< >>> &&& ||| <<| ets
*  - of \n - .\n ..\n
*  - of diff delim <> >< >| etc
*/

static bool print_parse_error(char c) {
    if (c == '\n')
        mx_printerr("u$h: parse error near `\\n\'\n");
    else {
        mx_printerr("u$h: parse error near `");
        write(2, &c, 1);
        mx_printerr("\'\n");
    }
    return true;
}

static bool check_parse_auditor(char *line, int i) {
    int i2;
    int i3;

    i2 = mx_get_char_index_quote(&line[i + 1], PARSE_DELIM);
    if (i2 == 0) {
        if (line[i] != line[i + 1] || line[i + 1] == ';')
            return print_parse_error(line[i + 1]);
        else if (line[i + 2]) {
            if (line[i + 2] == '\n')
                return print_parse_error('\n');
            i3 = mx_get_char_index_quote(&line[i + 2], PARSE_DELIM);
            if (i3 == 0)
                return print_parse_error(line[i + 2]);
        }
    }
    return false;
}

static bool check_parse(char *line) {
    int i = 0;

    while (line) {
        if ((i = mx_get_char_index_quote(line, PARSE_DELIM)) >= 0) {
            if (line[i + 1]) {
                if (line[i + 1] == '\n' && line[i] != ';')
                    return print_parse_error('\n');
                else if (check_parse_auditor(line, i))
                    return true;
            }
            line += i + 1;
        } else
            break;
    }
    return false;
}

bool mx_check_parce_errors(char *line) {
    if (!line || check_quote(line) || check_parse(line))
        return true;
    return false;
}
