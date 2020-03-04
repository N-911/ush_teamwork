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

int mx_set_redirections(t_shell *m_s, t_job *job, t_process *p) {
    mx_count_redir(job, p);
    m_s->redir = 0;

    mx_set_r_infile(m_s, job, p);
    mx_set_r_outfile(m_s, job, p);
    p->errfile = job->errfile;
    return m_s->redir;
}

void mx_count_redir(t_job *job, t_process *p) {
    t_redir *r;
    (void)job;

    p->c_input = 0;
    p->c_output = 0;
    for (r = p->redirect; r; r = r->next) {
        if (r->redir_delim == R_INPUT || r->redir_delim == R_INPUT_DBL)
            p->c_input += 1;
        if (r->redir_delim == R_OUTPUT || r->redir_delim == R_OUTPUT_DBL)
            p->c_output += 1;
    }
//    if (p->pipe)
//        p->c_output++;
//    if (job->infile != STDIN_FILENO)
//        p->c_input++;
    if (p->c_input == 0)
        p->c_input++;
    if (p->c_output == 0)
        p->c_output++;
    printf("\x1B[32m p->redirect->c_input = %d \x1B[0m  \n", p->c_input);
    printf("\x1B[32m p->redirect->c_output = %d \x1B[0m  \n", p->c_output);
}

void mx_set_r_infile(t_shell *m_s, t_job  *job, t_process *p) {
    t_redir *r;
    int j = 0;

    p->r_infile = (int *) malloc(sizeof(int) * (p->c_input));
    p->r_infile[0] = job->infile;
//    printf("set_infile\n");
//    if (job->infile == STDIN_FILENO)
//        j = 1;
    if (p->redirect) {
        for (r = p->redirect; r; r = r->next) {
            if (r->input_path) {
                if (r->redir_delim == R_INPUT)
                    mx_red_in(m_s, job, p, r, j);
                if (r->redir_delim == R_INPUT_DBL) {
                    mx_red_in_d(m_s, job, p, r, j);
                }
                j++;
            }
//        job->init_file = p->r_infile[0];
        }
    }
}

void mx_red_in(t_shell *m_s, t_job *job, t_process *p, t_redir *r, int j) {
        printf(" <\n ");
//        printf("r->input_path   = %s\n", r->input_path);
        printf("p->r_infile[0] start = p->r_infile[%d]\n", p->r_infile[0]);
    (void)job;
    (void)m_s;

        int fd = open(r->input_path, O_RDONLY, 0666);
        if (fd < 0) {
            mx_printerr("ush :");
            perror(r->input_path);
            mx_set_variable(m_s->variables, "?", "1");
            m_s->redir = 1;
            job->exit_code = 1;
        }
        p->r_infile[j] = fd;
    printf("p->r_infile[0] end  = p->r_infile[%d]\n", p->r_infile[0]);
}

void mx_red_in_d(t_shell *m_s, t_job *job, t_process *p, t_redir *r, int j) {
    printf(" << \n ");
    printf("r->input_path = %s\n", r->input_path);
    int fd = open(r->input_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) {
        mx_printerr("ush :");
        perror(r->input_path);
        mx_set_variable(m_s->variables, "?", "1");
        m_s->redir = 1;
        job->exit_code = 1;
//        continue;
    }
    char *line = strdup("");
    int count = 0;

    while (strcmp(line, r->input_path) != 0) {
        p->pipe ? printf("pipe heredoc> ") : printf("heredoc> ");
        write(fd, line, mx_strlen(line));
        free(line);  ////////////////////
        if (count)
            write(fd, "\n", 1);
        line = mx_ush_read_line(m_s);
        count++;
    }
    free(line);
    close(fd);
    p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
    remove(r->input_path);
}


void mx_set_r_outfile(t_shell *m_s, t_job *job, t_process *p) {
    int flags = 0;
    t_redir *r;
    int j = 0;
//    int fd;
    p->r_outfile = (int *) malloc(sizeof(int) * (p->c_output));
    p->r_outfile[0] = job->outfile;


//    printf("out redir start  = p->r_outfile[%d]\n", p->r_outfile[0]);
//    if (p->pipe)
//        j = 1;
    if (p->redirect) {
        for (r = p->redirect; r; r = r->next) {
            if (r->output_path) {
                printf("j =  %d\n", j);
                printf("out redir =  %s\n", r->output_path);
                if (r->redir_delim == R_OUTPUT) {
                    flags = O_WRONLY | O_CREAT | O_TRUNC;
                }
                if (r->redir_delim == R_OUTPUT_DBL) {
                    flags = O_WRONLY | O_CREAT;
                }
//                if (r->redir_delim == R_OUTPUT || r->redir_delim == R_OUTPUT_DBL) {
//                    printf("++\n");
                if ((p->r_outfile[j] = open(r->output_path, flags, 0666 )) < 0) {
//                    if (fd < 0) {
                        mx_printerr("ush :");
                        perror(r->output_path);
                        mx_set_variable(m_s->variables, "?", "1");
                        m_s->redir = 1;
                        job->exit_code = 1;
                    }
                    lseek(p->r_outfile[j], 0, SEEK_END);
//                }
//                p->r_outfile[j] = fd;
                j++;
            }
        }
    }
//    printf("out redir end  = p->r_outfile[%d]\n", p->r_outfile[0]);
//    printf("out redir end  = p->r_outfile[%d]\n", p->r_outfile[1]);
}


void mx_print_fd(t_process  *p) {
    printf("\x1B[32m p->r_input \x1B[0m\t");
    for(int i = 0; i < p->c_input; i ++) {
        printf("\x1B[32m [%d] \x1B[0m  \t", p->r_infile[i]);
    }
    printf("\n");
    printf("\x1B[32m p->r_output \x1B[0m\t");
    for(int i = 0; i < p->c_output; i ++) {
        printf("\x1B[32m [%d] \x1B[0m  \t", p->r_outfile[i]);
    }
    printf("\n");
}




/*
void mx_set_redir_input(t_shell *m_s, t_job *job, t_process *p, int job_id) {
    if (p->redir_delim == R_INPUT) {
        job->infile = open(p->input_path, O_RDONLY, 0666);
        if (job->infile < 0) {
            mx_printerr("ush :");
            perror(p->input_path);
            mx_set_variable(m_s->variables, "?", "1");
            job_id--;
            job_id++;
            m_s->redir = 1;
            job->exit_code = 1;
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
            //line = mx_ush_read_line();
            count++;
        }
        free(line);
        close(fd);
        job->infile = open(p->input_path, O_RDONLY, 0666);
        remove(p->input_path);
    }
}

void mx_set_redir_output(t_shell *m_s, t_job * job, t_process *p) {
    int flags;

    if (p->output_path) {  // redirection > >>
        if (p->redir_delim == R_OUTPUT)
            flags = O_WRONLY | O_CREAT | O_TRUNC;
        if (p->redir_delim == R_OUTPUT_DBL)
            flags = O_WRONLY | O_CREAT;
        if ((job->outfile = open(p->output_path, flags, 0666)) < 0) {
            mx_printerr("ush :");
            perror(p->output_path);
            mx_set_variable(m_s->variables, "?", "1");
            m_s->redir = 1;
            job->exit_code = 1;
        }
        lseek(job->outfile, 0, SEEK_END);
    }
}
*/
