#include "ush.h"

void mx_set_r_infile(t_shell *m_s, t_job *job, t_process *p) {
    t_redir *r;
    int j = 0;

    p->r_infile = (int *) malloc(sizeof(int) * (p->c_input));
    p->r_infile[0] = job->infile;
    if (p->redirect) {
        for (r = p->redirect; r; r = r->next) {
            if (r->input_path) {
                if (r->redir_delim == R_INPUT)
                    m_s->redir = mx_red_in(job, p, r, j);
                if (r->redir_delim == R_INPUT_DBL) {
                    //m_s->redir = mx_red_in_d(job, p, r, j);
                }
                j++;
            }
        }
//        if (m_s->redir == 1)
//            mx_set_variable(m_s->variables, "?", "1");
        job->infile = p->r_infile[0];
    }
}


int mx_red_in(t_job *job, t_process *p, t_redir *r, int j) {
    int status_redir = 0;
    int fd;

    if ((fd = open(r->input_path, O_RDONLY, 0666)) < 0) {
        mx_printerr("ush :");
        perror(r->input_path);
//        mx_set_variable(m_s->variables, "?", "1");
        status_redir = 1;
        job->exit_code = 1;
    }
    p->r_infile[j] = fd;
    return status_redir;
}

int mx_red_in_d(t_job *job, t_process *p, t_redir *r, int j) {
    int status_redir = 0;
    int fd;
    char *line;
    int count;

    if ((fd = open(r->input_path, O_RDWR | O_CREAT | O_TRUNC, 0666)) < 0 ) {
        mx_printerr("ush :");
        perror(r->input_path);
//        mx_set_variable(m_s->variables, "?", "1");
        status_redir = 1;
        job->exit_code = 1;
    }
    line = strdup("");
    count = 0;
    while (strcmp(line, r->input_path) != 0) {
        p->pipe ? printf("pipe heredoc> ") : printf("heredoc> ");
        write(fd, line, mx_strlen(line));
        free(line);  ////////////////////
        if (count)
            write(fd, "\n", 1);
//        line = mx_ush_read_line(m_s);
        count++;
    }
    free(line);
    close(fd);
    p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
    remove(r->input_path);
    return status_redir;
}



