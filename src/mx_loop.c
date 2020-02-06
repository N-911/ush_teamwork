#include "ush.h"

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
//    int status = 1;
    while (1) {
        mx_check_jobs(m_s);
        printf("\r%s%s%s", GRN, "u$h> ", RESET);
        line = mx_ush_read_line();
        if (line[0] == '\0') {
//            mx_check_jobs(m_s);
            continue;
        } else {
            if ((ast = mx_ast_creation(line, m_s))) {
                // ast_print(ast);  // печать дерева
                for (int i = 0; ast[i]; i++) {
                    t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
                    new_job = mx_create_job(m_s, ast[i]);
                    mx_launch_job(m_s, new_job);
                    //m_s->exit_code = status;
                    // mx_destroy_jobs(m_s, 0);
                    m_s->history_count = 0;
                    //  mx_add_history(m_s, new_job);
        //        termios_restore(m_s);
                }
                mx_ast_clear_all(&ast);  // clear leeks
            }
        }
        mx_strdel(&line);
    }
}
