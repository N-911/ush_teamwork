#include "ush.h"
/*
 *  Create ast from parsed_line
 */
// t_ast **mx_ast_creation(char *line, t_shell *m_s) {
//     t_ast **ast = NULL;
//     t_ast *parsed_line = NULL;

//     if (!(parsed_line = mx_ush_parsed_line(line, m_s->variables))) {
//         // mx_printerr("parsed_line is NULL\n");
//         return NULL;
//     }
//     if (!(ast = mx_ast_parse(parsed_line)) || !(*ast)) {
//         // mx_printerr("ast is NULL\n");
//         return NULL;
//     }
//     // mx_ast_clear_list(&parsed_line);
//     print_list(parsed_line);
//     return ast;
// }
/*
 * Print redirections (one process)
 */
static void print_left(t_ast *q) {
    for (t_ast *r = q->left; r; r = r->next) {
        mx_printstr("redir == ");
        if (r->type == R_INPUT)
            mx_printstr("< ");
        else if (r->type == R_INPUT_DBL)
            mx_printstr("<< ");
        else if (r->type == R_OUTPUT)
            mx_printstr("> ");
        else if (r->type == R_OUTPUT_DBL)
            mx_printstr(">> ");
        if (r->args)
            mx_print_strarr_in_line(r->args, " ");
    }
}
/*
 * Print one ast-list (one job)
 */
static void print_list(t_ast *parsed_line) {
    for (t_ast *q = parsed_line; q; q = q->next) {
        mx_printstr("proc  == ");
        if (q->args)
            mx_print_strarr_in_line(q->args, " ");
        if (q->left)
            print_left(q);
        mx_printstr("delim == ");
        mx_printint(q->type);
        mx_printstr("\n");
    }
}
/*
 * Print array of ast-lists (all jobs)
 */
void mx_ast_print(t_ast **ast) {
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

void mx_del_strarr(char ***arr) {
    if (arr == NULL)
        return;

    char ***p = arr;
    char **pp = *arr;
    while (**p != NULL) {
        mx_strdel(*p);
        (*p)++;
    }
    free(pp);
    *arr = NULL;
}

// void clear_list(t_ast **list) {
//     t_ast *q = *list;
//     t_ast *tmp = NULL;

//     if (!(*list) || !list)
//         return;
//     while (q) {
//         if (q->args) {
//             // char **arr = q->args;
//             // for (int i = 0; arr[i]; i++) {
//             //     if (malloc_size(arr[i]) > 0)
//             //         mx_strdel(&arr[i]);
//             // }
//             free(q->args);
//         }
//         if (q->left)
//             mx_ast_clear_list(&q->left);
//         tmp = q->next;
//         free(q);
//         q = tmp;
//     }
//     *list = NULL;
// }


t_ast **mx_ast_creation(char *line, t_shell *m_s) {
    t_ast **ast = NULL;
    t_ast *parsed_line = NULL;

    if (!(parsed_line = mx_ush_parsed_line(line, m_s->variables))) {
        // mx_printerr("parsed_line is NULL\n");
        return NULL;
    }
    if (!(ast = mx_ast_parse(parsed_line)) || !(*ast)) {
        // mx_ast_clear_list(&parsed_line);
        // mx_printerr("ast is NULL\n");
        return NULL;
    }
    // print_list(parsed_line);
    mx_ast_clear_list(&parsed_line);
    // clear_list(&parsed_line);
    return ast;
}
