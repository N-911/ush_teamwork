#include "ush.h"

static int get_flag(char **args);
static int execute_job(t_shell *m_s, t_job * job, int job_id);
static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status);

void mx_launch_job(t_shell *m_s, t_job *job) {
    setbuf(stdout, NULL);
    int status;
    int job_id;

    mx_check_jobs(m_s);
    job_id = mx_insert_job(m_s, job);
    if (!job->job_type)
        status = execute_job(m_s, job, job_id);
    else if (job->job_type == AND && m_s->exit_code == 0)
        status = execute_job(m_s, job, job_id);
    else if (job->job_type == OR && m_s->exit_code != 0)
        status = execute_job(m_s, job, job_id);
    else
        mx_remove_job(m_s, job_id);
// m_s->exit_code == 0 ? m_s->exit_code = status : 0;
    char *exit_status = mx_itoa(m_s->exit_code);
    mx_set_variable(m_s->variables, "?", exit_status);
    free(exit_status);
}

static int execute_job(t_shell *m_s, t_job * job, int job_id) {
    extern char **environ;  // ?
    int status;
    t_process *p;
    int mypipe[2];

    job->env = environ;
    job->path = getenv("PATH");
    if (!job->path)
        job->path = "";
    for (p = m_s->jobs[job_id]->first_pr; p; p = p->next) {
        if (m_s->exit_flag == 1 && !(p->type == 10))
            m_s->exit_flag = 0;
        m_s->redir = 0;
        mx_set_redirec(m_s, job, p, job_id);
        if (m_s->redir != 0)
            continue;
        if (p->pipe) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);  // ?
            }
            job->outfile = mypipe[1];
        }
        p->infile = job->infile;
        p->outfile = job->outfile;
        p->errfile = job->errfile;
        int flag = 0;
        if (!p->pipe)
            flag = get_flag(p->argv);
        if (flag) {
            status = mx_set_parametr(p->argv, m_s);
            mx_remove_job(m_s, job_id);
        }
        else if (p->type != -1) {
            status = mx_launch_builtin(m_s, p, job_id);  // fork own buildins
        }
        else{
            status = mx_launch_process(m_s, p, job_id);  // remove pat and env
        }
        if (job->infile != job->stdin)
            close(job->infile);
        if (job->outfile != job->stdout)
            close(job->outfile);
        job->infile = mypipe[0];
        m_s->exit_code = status;
    }
    launch_job_help(m_s, job, job_id, status);
    return status;
}

static void launch_job_help (t_shell *m_s, t_job *job, int job_id, int status) {
    int shell_terminal = STDIN_FILENO;

    if (job->foregrd) {
        tcsetpgrp(STDIN_FILENO, job->pgid);
        if (status == 0)
            status = mx_wait_job(m_s, job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
        signal(SIGTTOU, MX_SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpid());
//        signal(SIGTTOU, SIG_DFL);
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
