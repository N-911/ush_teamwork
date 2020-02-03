//#include <inc/ush.h>
#include "ush.h"

static int get_flag(char **args);

void mx_launch_job(t_shell *m_s, t_job *job) {
    //pid_t shell_pgid;
    extern char **environ;
    char **env = environ;
    char *path = getenv("PATH");

    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_env, &mx_export, &mx_unset,
        &mx_echo, &mx_jobs, &mx_fg, &mx_bg, &mx_cd, &mx_pwd, &mx_which, &mx_exit, NULL};

//    pid_t wpid;
    setbuf(stdout, NULL); /* установить небуферизованный режим */
    int status;
  //  int shell_terminal = STDIN_FILENO;
    int job_id;  // for job contoll
//    int shell_is_interactive = isatty(shell_terminal);
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
    t_process *p;
    int mypipe[2];
    int infile;
    int outfile = 1;
    int errfile = 2;
    infile = job->stdin;

    mx_check_jobs(m_s);  // job control
    job_id = mx_insert_job(m_s, job);  // insert process to job control
    job->pgid= getpid();
    for (p = m_s->jobs[job_id]->first_process; p; p = p->next) {  // list of process in job
        if (p->pipe) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            outfile = mypipe[1];
        } 
        else
            outfile = job->stdout;
        //============Tестовая хуйня, переделать==========//
        /**/int flag = get_flag(p->argv);               /**/
        /**/if (flag) {                                 /**/
        /**/	status = mx_set_parametr(p->argv,m_s);	/**/
        /**/}											/**/
        //===============================================//
        else if (p->type != -1) {
            p->outfile = outfile;
            if (p->pipe) {
                pid_t pid;
                int status;

                pid = fork();
                if (pid == 0) {
                    status = builtin_functions[p->type](m_s, p);
                    exit (status);
                }
                else { // Родительский процесс
                    wait(&status);
                }   
            }
            else {
                if ((status = builtin_functions[p->type](m_s, p)) >= 0)
                    p->status = 1;
            }
        }
        else
            status = mx_launch_process(m_s, p, job_id, path, env, infile, outfile, errfile);

        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];
        //printf("STATUS($?) = %d\n", status);
        mx_set_variable(m_s->variables, "?", mx_itoa(status));
    }
    if (status >= 0 && job->foreground == FOREGROUND) {
        //mx_print_process_in_job(m_s, job->job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
    }
    else if (job->foreground == BACKGROUND) {
         if (kill (-job->pgid, SIGCONT) < 0)
            perror ("kill (SIGCONT)");
        mx_print_pid_process_in_job(m_s, job->job_id);
    }
    //return status;
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

