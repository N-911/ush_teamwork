#include "ush.h"
/*
*  print one ast-list (one job)
*/
static void print_list(t_ast *parsed_line) {
    for (t_ast *q = parsed_line; q; q = q->next) {
        mx_printstr("proc  == ");

        if (q->args)
            mx_print_strarr_in_line(q->args, " ");
        else {
            mx_printstr(q->line);
            mx_printstr("\n");
        }

        if (q->left) {
            for (t_ast *r = q->left; r; r = r->next) {
                mx_printstr("redir == ");

                if (r->type == R_INPUT)
                    mx_printstr("> ");
                else if (r->type == R_INPUT_DBL)
                    mx_printstr(">> ");
                else if (r->type == R_OUTPUT)
                    mx_printstr("< ");
                else if (r->type == R_OUTPUT_DBL)
                    mx_printstr("<< ");

                if (r->args)
                    mx_print_strarr_in_line(r->args, " ");
                else {
                    mx_printstr(r->line);
                    mx_printstr("\n");
                }
            }
        }
        mx_printstr("delim == ");
        mx_printint(q->type);
        mx_printstr("\n");
    }
}
/*
*  print array of ast-lists (all jobs)
*/
void ast_print(t_ast **ast) {
    char *j = NULL;
    for (int i = 0; ast[i]; i++) {
        mx_print_color(YEL, "job-");
        j = mx_itoa(i + 1);
        mx_print_color(YEL, j);
        mx_strdel(&j);
        mx_printstr("\n");
        print_list(ast[i]);
    }
    mx_print_color(YEL, "-----\n");
}
/*
*  create ast from parsed_line
*/
t_ast **mx_ast_creation(char *line) {
    t_ast **ast = NULL;
    t_ast *parsed_line = NULL;

    if (!(parsed_line = mx_ush_parsed_line(line))) {  // parse and print
        // mx_printerr("parsed_line is NULL\n");
        return NULL;
    }
    if (!(ast = mx_ast_parse(parsed_line)) || !(*ast)) {
        // mx_printerr("ast is NULL\n");
        return NULL;
    }
    mx_filters(ast);
    mx_ast_clear_list(&parsed_line);  // clear leeks
    return ast;
}
