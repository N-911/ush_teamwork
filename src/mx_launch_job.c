#include "ush.h"

static int get_flag(char **args);
static int execute_job(t_shell *m_s, t_job * job, int job_id);
static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status);

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
        // //------------- print info
        // mx_print_color(RED, "job [");
        // mx_print_color(RED, mx_itoa(job_id));
        // mx_print_color(RED, "]\t\t");
        // mx_print_color(RED, mx_itoa(p->type));
        // mx_print_color(RED, "\tjob->foreground\t");
        // mx_print_color(RED, mx_itoa(job->foreground));
        // mx_print_color(RED, "\tp->foreground\t");
        // mx_print_color(RED, mx_itoa(p->foreground));
        // mx_print_color(RED, "\t job->pgid\t");
        // mx_print_color(RED,mx_itoa(m_s->jobs[job_id]->pgid));
        // mx_printstr("\n");
        // //------------
        if (m_s->exit_flag == 1 && !(p->type == 10))
            m_s->exit_flag = 0;

        if (p->input_path) { // redirection > >>
            int flags;
            if (p->redir_delim == R_INPUT)
                flags = O_WRONLY | O_CREAT | O_TRUNC;
            if (p->redir_delim == R_INPUT_DBL)
                flags = O_WRONLY | O_CREAT;
            outfile = open(p->input_path, flags, 0666);
        }
        if (p->output_path) { // redirection < <<
            if (p->redir_delim == R_OUTPUT) {
                infile = open(p->output_path, O_RDONLY, 0666);
                if (infile < 0) {
                    mx_printerr("ush :");
                    perror(p->output_path);
                    mx_set_variable(m_s->variables, "?", "1");
                    mx_remove_job(m_s, job_id);
                    continue;
                }
            }
            if (p->redir_delim == R_OUTPUT_DBL) {
                int fd = open(p->output_path, O_RDWR | O_CREAT | O_TRUNC, 0666);
                char *line = "";
                int count = 0;
                while (strcmp(line, p->output_path) != 0) {
                    printf("heredoc> ");
                    write(fd, line, mx_strlen(line));
                    if (count)
                        write(fd, "\n", 1);
                    line = mx_ush_read_line();
                    count++;
                }
                close(fd);
                infile = open(p->output_path, O_RDONLY, 0666);
                remove(p->output_path);
            }
        }
        if (p->pipe) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            outfile = mypipe[1];
        }
        // else
        //     outfile = job->stdout;
        p->infile = infile;
        p->outfile = outfile;
        p->errfile = errfile;
        int flag = get_flag(p->argv);
        if (flag) {
            status = mx_set_parametr(p->argv, m_s);
        } else if (p->type != -1) {
            status = mx_launch_builtin(m_s, p, job_id);  // fork own buildins
            m_s->exit_code = status;
        } else
            status = mx_launch_process(m_s, p, job_id, path, env, infile, outfile, errfile);
        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];
    }
    launch_job_help(m_s, job, job_id, status);
    return status;
}

static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status) {
    int shell_terminal = STDIN_FILENO;

    if (job->foreground) {
    //else if (status >= 0 && job->foreground == FOREGROUND) {
        tcsetpgrp(STDIN_FILENO, job->pgid);
        status = mx_wait_job(m_s, job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
        //signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpid());
    //signal(SIGTTOU, SIG_DFL);
        tcgetattr(shell_terminal, &job->tmodes);
        tcsetattr(shell_terminal, TCSADRAIN, &m_s->tmodes);
    }
    else
        mx_print_pid_process_in_job(m_s, job->job_id);
    m_s->exit_code = status;
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
