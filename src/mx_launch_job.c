#include "ush.h"

void mx_launch_job(t_shell *m_s, t_job *job) {
    extern char **environ;
    char **env = environ;
    char *path = getenv("PATH");
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_env, &mx_export, &mx_unset,
         &mx_echo, &mx_jobs, &mx_fg, &mx_bg, &mx_cd, &mx_pwd, &mx_which, &mx_exit, NULL};
    setbuf(stdout, NULL); /* установить небуферизованный режим */
    int status;
    int job_id;  // for job contoll
//    int shell_is_interactive = isatty(shell_terminal);
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
    t_process *p;
    int mypipe[2];
    int infile;
    int outfile = 1;
    int errfile = 2;
    infile = job->stdin;

    // mx_check_jobs(m_s);  // job control
    job_id = mx_insert_job(m_s, job);  // insert job to job control
    job->pgid = getpid();
    for (p = m_s->jobs[job_id]->first_process; p; p = p->next) {  // list of process in job
        //------------- print info
        mx_print_color(RED, "p->type\t\t");
        mx_print_color(RED, mx_itoa(p->type));
        mx_printstr("\n");
        mx_print_color(RED, "p->foreground\t");
        mx_print_color(RED, mx_itoa(p->foreground));
        mx_printstr("\n");
        //------------
        if (m_s->exit_flag == 1 && !(p->type == 10))
            m_s->exit_flag = 0;
        if (p->next != NULL && p->input_path != NULL) { // redirection
            /*
            if ((infile = open(p->input_path, O_RDONLY)) < 0) {
                mx_remove_job(m_s, job_id);
                printf("$h: no such file or directory: %s\n", p->input_path);
                //perror("");
                exit(1);
            }
             */
        }
        if (p->pipe) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            outfile = mypipe[1];
        } else
            outfile = job->stdout;
        p->infile = infile;
        p->outfile = outfile;
        p->errfile = errfile;
        //============Tестовая хуйня, переделать==========//
        /**/if (mx_get_char_index(p->argv[0],'=') > 0) {/**/
            /**/	status = mx_set_parametr(p->argv,m_s);	/**/
            /**/	//printf("\n");							/**/
            /**/}											/**/
        //===============================================//
        if (p->type != -1 && !p->pipe && p->foreground == FOREGROUND) {
            if (p->type == 4 || p->type == 5 || p->type == 6) {
                mx_remove_job(m_s, job_id);
            }
            if ((status = builtin_functions[p->type](m_s, p)) >= 0)
                p->status = 1;
        }
        else
            status = mx_launch_process(m_s, p, job_id, path, env, infile, outfile, errfile);
        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];
    }
    if (status >= 0 && job->foreground == FOREGROUND) {
        mx_wait_job(m_s, job_id);
        //mx_print_process_in_job(m_s, job->job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
    }
    else if (job->foreground == BACKGROUND) {
        if (kill (-job->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
        mx_print_pid_process_in_job(m_s, job->job_id);
        // mx_destroy_jobs(m_s, job_id);
    }
    //return status;
}
