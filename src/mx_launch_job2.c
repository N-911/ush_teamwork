#include "ush.h"

void mx_dup_fd(t_process *p) {
    if (p->infile != STDIN_FILENO) {
        dup2(p->infile, STDIN_FILENO);
        close(p->infile);
    }
    if (p->outfile != STDOUT_FILENO) {
        dup2(p->outfile, STDOUT_FILENO);
        close(p->outfile);
    }
    if (p->errfile != STDERR_FILENO) {
        dup2(p->errfile, STDERR_FILENO);
        close(p->errfile);
    }
}

void mx_set_redirec(t_shell  *m_s, t_job * job, t_process *p, int job_id) {
    if (p->input_path) {
        mx_set_redir_input(m_s, job, p, job_id);
        mx_set_redir_inp_d(job, p);
    }
    mx_set_redir_output(job, p);
}

void mx_set_redir_input(t_shell *m_s, t_job *job, t_process *p, int job_id) {
    if (p->redir_delim == R_INPUT) {  // <
        job->infile = open(p->input_path, O_RDONLY, 0666);
        if (job->infile < 0) {
            mx_printerr("ush :");
            perror(p->input_path);
            mx_set_variable(m_s->variables, "?", "1");
            mx_remove_job(m_s, job_id);
            m_s->redir = 1;
            job->exit_code = 1; // ?
//                continue;
        }
    }
}

void mx_set_redir_inp_d(t_job *job, t_process *p) {
    if (p->redir_delim == R_INPUT_DBL) {
        int fd = open(p->input_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
        char *line = strdup("");
        int count = 0;

        while (strcmp(line, p->input_path) != 0) {
            p->pipe ? printf("pipe heredoc> ") : printf("heredoc> ");
            write(fd, line, mx_strlen(line));
            free(line);
            if (count)
                write(fd, "\n", 1);
            line = mx_ush_read_line();
            count++;
        }
        free(line);
        close(fd);
        job->infile = open(p->input_path, O_RDONLY, 0666);
        remove(p->input_path);
    }
}


void mx_set_redir_output(t_job * job, t_process *p) {
    int flags;

    if (p->output_path) {  // redirection > >>
        if (p->redir_delim == R_OUTPUT)
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        if (p->redir_delim == R_OUTPUT_DBL)
            flags = O_WRONLY | O_CREAT;
        job->outfile = open(p->output_path, flags, 0666);
        lseek(job->outfile, 0, SEEK_END);
    }
}

