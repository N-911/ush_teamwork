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

static void launch_blow_job(t_shell *m_s, t_ast **ast, t_job *new_job) {
    for (int i = 0; ast[i]; i++) {
        new_job = mx_create_job(m_s, ast[i]);
        new_job->job_type = get_job_type(ast, i);
        mx_launch_job(m_s, new_job);
    }
    mx_ast_clear_all(&ast);
}

static void check_eof(t_shell *m_s) {
    if (!isatty(0)) {
        mx_clear_all(m_s);
        //system("leaks -q ush");
        exit(0);
    }
}

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
    t_job *new_job = NULL;

    getenv("HOME") ? m_s->git = mx_get_git_info() : 0;
    while (1) {
        isatty(0) ? (line = mx_get_line(m_s)) : (line = mx_ush_read_line());
        if (line[0] == '\0') {
            free(line);
            mx_check_jobs(m_s);
            check_eof(m_s);
            continue;
        }
        else {
            if ((ast = mx_ast_creation(line, m_s)))
                launch_blow_job(m_s, ast, new_job);
        }
    }
}
