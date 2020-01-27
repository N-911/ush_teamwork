#include "ush.h"

void mx_launch_job(t_shell *m_s, t_job *job) {
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_echo, &mx_jobs, &mx_fg, &mx_exit, NULL};
    setbuf(stdout, NULL); // установить небуферизованный режим
    int status = 0;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive;
    int job_id;  // for job contoll

    shell_is_interactive = isatty(shell_terminal);
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
    t_process *p;
    int mypipe[2];
    int infile;
    int outfile = 1;
    int errfile = 2;
    infile = job->stdin;
    mx_check_jobs(m_s);
   // job_id = mx_insert_job(m_s, job);            //insert process to job control
    job_id = 0;            //insert process to job control
    job->pgid= getpid();
    for (p = job->first_process; p; p = p->next) {  //list of process in job
        if (p->pipe) {  //if pipe !!!!!
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            outfile = mypipe[1];
        } else
            outfile = job->stdout;

        if (p->type)
            status = (builtin_functions[p->type](m_s, p));
        // mx_set_process_status(m_s, pid, STATUS_DONE);
        else
            status = mx_launch_process(m_s, p, job_id, infile, outfile, errfile);
        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];
    }

    if (status >= 0 && job->foreground == FOREGROUND) {
        //mx_print_process_in_job(m_s, job->job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
    }
    else if (job->foreground == BACKGROUND) {
       // if (kill (-job->pgid, SIGCONT) < 0)
        //    perror ("kill (SIGCONT)");
        mx_print_process_in_job(m_s, job->job_id);
    }
    //return status;
}
