#include "ush.h"

static int get_job_type(t_ast **ast, int i) {
    t_ast *tmp = NULL;

    if (i != 0) {
        tmp = ast[i - 1];
        while (tmp->next)
            tmp = tmp->next;
        if (tmp->type == AND || tmp->type == OR)
            return tmp->type;
    }
    return 0;
}

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
//    int status = 1;
    while (1) {
        printf("\r%s%s%s", GRN, "u$h> ", RESET);
        line = mx_ush_read_line();
        if (line[0] == '\0') {
            mx_check_jobs(m_s);
            continue;
        } else {
            if ((ast = mx_ast_creation(line, m_s))) {
                // ast_print(ast);  // печать дерева
                for (int i = 0; ast[i]; i++) {
                    t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
                    new_job = mx_create_job(m_s, ast[i]);
                    new_job->job_type = get_job_type(ast, i);
                    mx_launch_job(m_s, new_job);
                    //m_s->exit_code = status;
                    // mx_destroy_jobs(m_s, 0);
                    m_s->history_count = 0;
                    //  mx_add_history(m_s, new_job);
        //        termios_restore(m_s);
                }
                mx_ast_clear_all(&ast);  // clear leeks
                // system ("leaks -q ush");
            }
        }
        mx_strdel(&line);
    }
}
