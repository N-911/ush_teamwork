#include "ush.h"

void mx_launch_job(t_shell *m_s, t_job *job) {
    //pid_t shell_pgid;
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_echo, &mx_jobs, &mx_fg, &mx_exit, NULL};
//    pid_t wpid;
    setbuf(stdout, NULL); // установить небуферизованный режим
    int status;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive;
    int job_id;  // for job contoll
    pid_t child_pid;

    shell_is_interactive = isatty(shell_terminal);
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
    t_process *p;
    int mypipe[2];
    int infile;
    int outfile = 1;
    int errfile = 2;
    infile = job->stdin;
//  job control
    mx_check_jobs(m_s);
    job_id = mx_insert_job(m_s, job);            //insert process to job control
    for (p = m_s->jobs[job_id]->first_process; p; p = p->next) {  //list of process in job
        if (p->next) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                exit(1);
            }
            outfile = mypipe[1];
        }
        else
            outfile = job->stdout;
        if (p->type) {
            p->exit_code = builtin_functions[p->type](m_s, p);
            //return p->exit_code;
        }
        else {
            child_pid = fork();
            //TELL_WAIT();
            if (child_pid < 0) {
                perror("error fork");
                exit(1);
            }
                //child process
            else if (child_pid == 0) {
                //TELL_PARENT(getpgid(0));
                mx_printstr("child fork\n");
                status = mx_launch_process(m_s, p, job->pgid, infile, outfile, errfile);
                //   mx_printstr("fork done\n");
                //parrent process
            }
            else {
               // WAIT_CHILD();
                mx_printstr("parent process\n");
                p->pid = child_pid;  //PID CHILD
                if (shell_is_interactive) {  //!!!!
                    if (!job->pgid)
                        job->pgid = child_pid;
                    setpgid(child_pid, job->pgid);
                }
                /*
                if (job->first_process->foreground == FOREGROUND) {
                    tcsetpgrp(0, job->pgid);
                    mx_wait_job(m_s, job->job_id);
                    signal(SIGTTOU, SIG_IGN);
                    tcsetpgrp(0, getpid());
                    signal(SIGTTOU, SIG_DFL);
                    mx_print_job_status(m_s, job_id);
                }
                 */
                if (infile != job->stdin)
                    close(infile);
                if (outfile != job->stdout)
                    close(outfile);
                infile = mypipe[0];
            }
        }
    }
    if (shell_is_interactive) {
//    if (job->first_process->foreground == FOREGROUND) {
        tcsetpgrp(0, job->pgid);
        mx_wait_job(m_s, job->job_id);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(0, getpid());
        signal(SIGTTOU, SIG_DFL);
        mx_print_job_status(m_s, job_id);
        if (job_id > 0 && mx_is_job_completed(m_s, job_id)) {
            //mx_print_job_status(m_s, job_id);
            mx_remove_job(m_s, job_id);
        }
    }
    else if (status >= 0 && job->foreground == FOREGROUND) {
        //mx_print_process_in_job(m_s, job->job_id);
        mx_remove_job(m_s, job->job_id);
    }

    else if (job->foreground == BACKGROUND) {
        if (kill (-job->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
        mx_print_process_in_job(m_s, job->job_id);
    }
    //return status;
}
