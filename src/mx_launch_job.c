#include "ush.h"

static int get_flag(char **args);
static int execute_job(t_shell *m_s, t_job * job, int job_id);
static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status);

static void print_info(t_shell *m_s, t_job *job, t_process *p, int job_id);

void mx_launch_job(t_shell *m_s, t_job *job) {
    setbuf(stdout, NULL); /* установить небуферизованный режим */
    int status;
    int job_id;  // for job contoll
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);

    mx_check_jobs(m_s);  // job control
    job_id = mx_insert_job(m_s, job);
//    printf("job_type  %d\n", job->job_type);
    if (!job->job_type)
        status = execute_job(m_s, job, job_id);
    else if (job->job_type == AND && m_s->exit_code == 0)
        status = execute_job(m_s, job, job_id);
    else if (job->job_type == OR && m_s->exit_code != 0)
        status = execute_job(m_s, job, job_id);
    else
        mx_remove_job(m_s, job_id);
// m_s->exit_code == 0 ? m_s->exit_code = status : 0;
    mx_print_color(RED, "m_s->exit_code  ");
    mx_print_color(RED, mx_itoa(m_s->exit_code));
    mx_set_variable(m_s->variables, "?", mx_itoa(m_s->exit_code));
    mx_printstr("\n");
}

static int execute_job(t_shell *m_s, t_job * job, int job_id) {
    extern char **environ;
    char **env = environ;
    char *path = getenv("PATH");
    int status;
    t_process *p;
    int mypipe[2];
    int infile = job->stdin;
    int outfile = 1;
    int errfile = 2;

    for (p = m_s->jobs[job_id]->first_process; p; p = p->next) {  // list of process in job
        print_info(m_s, job, p, job_id);
        if (m_s->exit_flag == 1 && !(p->type == 10))
            m_s->exit_flag = 0;

        mx_set_redirections(p, infile, outfile);
        if (p->pipe) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            outfile = mypipe[1];
            p->r_outfile[0] = outfile;
        }
        else
            outfile = job->stdout;
        //*******
        p->infile = infile;
        p->outfile = outfile;
        p->errfile = errfile;
        //********
        int flag = get_flag(p->argv);
        if (flag) {
            status = mx_set_parametr(p->argv, m_s);
        }
        else if (p->type != -1) {
                status = mx_launch_builtin(m_s, p, job_id);  // fork own buildins
        }
        else {
            mx_print_fd(p);
            status = mx_launch_process(m_s, p, job_id, path, env );
        }
        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];

//        if (p->c_input > 1) {
//            for(int i = 0; i < p->c_input; i ++) {
//                if (p->r_infile[i] != job->stdin)
//                    close(p->r_infile[i]);
//            }
////        }
////        if (p->c_output > 1) {
//            for(int i = 0; i < p->c_output; i ++) {
//                if (p->r_outfile[i] != job->stdout)
//                    close(p->r_outfile[i]);
//            }
        m_s->exit_code = status;
    }
    mx_set_last_job(m_s);  //!!!!!!!! test
    launch_job_help(m_s, job, job_id, status);
    return status;
}

static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status) {
    int shell_terminal = STDIN_FILENO;

    if (job->foreground) {
//        printf ("foreground help\n");
        tcsetpgrp(STDIN_FILENO, job->pgid);
        status = mx_wait_job(m_s, job_id);
        if (mx_job_completed(m_s, job_id)) {
            mx_remove_job(m_s, job_id);
        }
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpid());
//        signal(SIGTTOU, SIG_DFL);
        tcgetattr(shell_terminal, &job->tmodes);
        tcsetattr(shell_terminal, TCSADRAIN, &m_s->tmodes);
    }
    else {
//        printf ("background help\n");
        mx_print_pid_process_in_job(m_s, job->job_id);
//        tcsetpgrp(STDIN_FILENO, getpid());
//        mx_check_jobs(m_s);  // job control
    }
    m_s->exit_code = status;
//    printf ("help end \n");
}

void mx_set_redirections(t_process *p, int intfile, int outfile) {
    mx_count_redir(p, intfile);
    set_r_infile(p, intfile);
    set_r_outfile(p, outfile);
}

void mx_count_redir(t_process *p, int intfile) {
    t_redir *r;

    for (r = p->redirect; r; r = r->next) {
        if (r->redir_delim == R_INPUT || r->redir_delim == R_INPUT_DBL)
            p->c_input +=1;
        if (r->redir_delim == R_OUTPUT || r->redir_delim == R_OUTPUT_DBL)
            p->c_output +=1;
    }
//    if (p->pipe)
    if (intfile != STDOUT_FILENO)
        p->c_input++;
    if (p->c_output == 0)
        p->c_output++;
    printf("\x1B[32m p->redirect->c_input = %d \x1B[0m  \n", p->c_input);
    printf("\x1B[32m p->redirect->c_output = %d \x1B[0m  \n", p->c_output);
}
void set_r_infile(t_process *p, int infile) {
    t_redir *r;
    int j;

    p->r_infile = (int *) malloc(sizeof(int) * (p->c_input));
    p->r_infile[0] = infile;
    if (p->redirect) {
        for (r = p->redirect, j = 1; r; r = r->next, j++) {
            if (r->redir_delim == R_INPUT) {
                printf(" <\n ");
                p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
//                    infile = open(p->input_path, O_RDONLY, 0666);
                if (p->r_infile[j] < 0) {
                    mx_printerr("ush :");
                    perror(r->input_path);
//                    mx_set_variable(m_s->variables, "?", "1");
//                    mx_remove_job(m_s, job_id);
                    continue;
                }
            }
            if (r->redir_delim == R_INPUT_DBL) {
                int fd = open(r->input_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
                char *line = "";
                int count = 0;
                while (strcmp(line, r->input_path) != 0) {
                    printf("heredoc> ");
                    write(fd, line, mx_strlen(line));
                    if (count)
                        write(fd, "\n", 1);
                    line = mx_ush_read_line();
                    count++;
                }
                close(fd);
                p->r_infile[j] = open(r->input_path, O_RDONLY, 0666);
                remove(r->input_path);
            }
        }
    }
}

void set_r_outfile(t_process *p, int outfile) {
    int flags;
    t_redir *r;
    int j;

    p->r_outfile = (int *)malloc(sizeof(int) * (p->c_output));
    p->r_outfile[0] = outfile;
    if (p->redirect) {
        for (r = p->redirect, j = 0; r; r = r->next, j++) {
//            printf("count out redir\n");
            if (r->redir_delim == R_OUTPUT) {
                flags = O_WRONLY | O_CREAT | O_TRUNC;
                if ((p->r_outfile[j] = open(r->output_path, flags, 0666)) == -1) {
                    mx_printerr("ush :");
                    perror(r->output_path);
//                return 255;  // do!!
                }
            }
            if (r->redir_delim == R_OUTPUT_DBL) {
                flags = O_WRONLY | O_CREAT;
                if ((p->r_outfile[j] = open(r->output_path, flags, 0666)) == -1) {
                    mx_printerr("ush :");
                    perror(r->output_path);
//                return 255;  // do!!
                }
            }
//            printf ("out redir end %d\n", p->r_outfile[j]);
        }
    }
}



static int get_flag(char **args) {
    int flag = 1;

    for (int i = 0; args[i] != NULL; i++) {
        if (mx_get_char_index(args[i],'=') <= 0) {
            flag--;
            break;
        }
    }
    return flag;
}

static void print_info(t_shell *m_s, t_job *job, t_process *p, int job_id) {
    //------------- print info
    mx_print_color(RED, "job [");
    mx_print_color(RED, mx_itoa(job_id));
    mx_print_color(RED, "]\t\t");
    mx_print_color(RED, mx_itoa(p->type));
    mx_print_color(RED, "\t");
    mx_print_color(RED, p->argv[0]);
    mx_print_color(RED, "\tjob->foreground\t");
    mx_print_color(RED, mx_itoa(job->foreground));
    mx_print_color(RED, "\tp->foreground\t");
    mx_print_color(RED, mx_itoa(p->foreground));
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

