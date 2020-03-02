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

//    if (p->input_path)
    mx_set_r_infile(m_s, job, p);
    mx_set_r_outfile(m_s, job, p);
    return m_s->redir;
}


void mx_count_redir(t_job *job, t_process *p) {
    t_redir *r;
    p->c_input = 0;
    p->c_output = 0;
    for (r = p->redirect; r; r = r->next) {
        if (r->redir_delim == R_INPUT || r->redir_delim == R_INPUT_DBL)
            p->c_input += 1;
        if (r->redir_delim == R_OUTPUT || r->redir_delim == R_OUTPUT_DBL)
            p->c_output += 1;
    }
//    if (p->pipe)
    if (job->infile != STDOUT_FILENO)
        p->c_input++;
    if (p->c_output == 0)
        p->c_output++;
    if (!p->pipe && p->outfile != STDOUT_FILENO)
        p->c_output++;

    printf("\x1B[32m p->redirect->c_input = %d \x1B[0m  \n", p->c_input);
    printf("\x1B[32m p->redirect->c_output = %d \x1B[0m  \n", p->c_output);
}


void mx_set_r_infile(t_shell *m_s, t_job  *job, t_process *p) {
    t_redir *r;
    int j;

    p->r_infile = (int *) malloc(sizeof(int) * (p->c_input));
    p->r_infile[0] = job->infile;
//    printf("set_infile\n");

    if (p->redirect) {
        for (r = p->redirect, j = 1; r; r = r->next, j++) {
            if (r->redir_delim == R_INPUT) {
                printf(" <\n ");
                p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
//                    infile = open(p->input_path, O_RDONLY, 0666);
                if (p->r_infile[j] < 0) {
                    mx_printerr("ush :");
                    perror(r->input_path);
                    mx_set_variable(m_s->variables, "?", "1");
                    m_s->redir = 1;
                    job->exit_code = 1;
                    continue;
                }
            }
            if (r->redir_delim == R_INPUT_DBL) {
                int fd = open(r->input_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
                char *line = strdup("");
                int count = 0;

                while (strcmp(line, r->input_path) != 0) {
                    p->pipe ? printf("pipe heredoc> ") : printf("heredoc> ");
                    write(fd, line, mx_strlen(line));
                    if (count)
                        write(fd, "\n", 1);
                    line = mx_ush_read_line();
                    count++;
                }
                free(line);
                close(fd);
                p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
                remove(r->input_path);
            }
        }
//        job->init_file = p->r_infile[0];
    }
}


void mx_set_r_outfile(t_shell *m_s, t_job *job, t_process *p) {
    int flags;
    t_redir *r;
    int j;

//    printf("set_r_output \n");
    p->r_outfile = (int *) malloc(sizeof(int) * (p->c_output));
    p->r_outfile[0] = job->outfile;
    if (p->redirect) {
        for (r = p->redirect, j = 1; r; r = r->next, j++) {
//            printf("out redir =  %s\n",r->output_path);
            if (r->redir_delim == R_OUTPUT) {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
            }
            if (r->redir_delim == R_OUTPUT_DBL) {
                flags = O_WRONLY | O_CREAT;
            }
            if ((p->r_outfile[j] = open(r->output_path, flags, 0666)) < 0) {
                mx_printerr("ush :");
                perror(r->output_path);
                mx_set_variable(m_s->variables, "?", "1");
                m_s->redir = 1;
                job->exit_code = 1;
            }
            lseek(job->outfile, 0, SEEK_END);
        }
//        printf("out redir end %d\n", p->r_outfile[j]);
//        job->outfile = p->r_outfile[0];
    }
}




void mx_print_info(t_shell *m_s, t_job *job, t_process *p, int job_id) {
    //------------- print info
    mx_print_color(RED, "job [");
    mx_print_color(RED, mx_itoa(job_id));
    mx_print_color(RED, "]\t\t");
    mx_print_color(RED, mx_itoa(p->type));
    mx_print_color(RED, "\t");
    mx_print_color(RED, p->argv[0]);
    mx_print_color(RED, "\tjob->foreground\t");
    mx_print_color(RED, mx_itoa(job->foregrd));
    mx_print_color(RED, "\tp->foreground\t");
    mx_print_color(RED, mx_itoa(p->foregrd));
    mx_print_color(RED, "\t job->pgid\t");
    mx_print_color(RED,mx_itoa(m_s->jobs[job_id]->pgid));
    mx_printstr("\n");
    //------------

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
int mx_set_redirec(t_shell  *m_s, t_job * job, t_process *p, int job_id) {
    m_s->redir = 0;

    if (p->input_path) {
        mx_set_redir_input(m_s, job, p, job_id);
//        mx_set_redir_inp_d(job, p);
    }
    mx_set_redir_output(m_s, job, p);
    return m_s->redir;
}

*/


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
            line = mx_ush_read_line();
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
