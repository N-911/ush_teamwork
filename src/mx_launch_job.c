#include "ush.h"

static int get_flag(char **args);

void mx_launch_job(t_shell *m_s, t_job *job) {
    extern char **environ;
    char **env = environ;
    char *path = getenv("PATH");
    setbuf(stdout, NULL); /* установить небуферизованный режим */
    int status;
    int job_id;  // for job contoll
//    tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
    t_process *p;
    int mypipe[2];
    int infile;
    int outfile = 1;
    int errfile = 2;
    infile = job->stdin;
    int shell_is_interactive = isatty(STDIN_FILENO);  //!!

    mx_check_jobs(m_s);  // job control
    job_id = mx_insert_job(m_s, job);  // insert job to job control
    for (p = m_s->jobs[job_id]->first_process; p; p = p->next) {  // list of process in job
    	p->job_id = job_id;
//        //------------- print info
//        mx_print_color(RED, "job :   p->type\t\t");
//        mx_print_color(RED, mx_itoa(p->type));
//        mx_print_color(RED, "\tp->foreground\t");
//        mx_print_color(RED, mx_itoa(p->foreground));
//        mx_print_color(RED, "\t job->pgid\t");
//        mx_print_color(RED,mx_itoa(m_s->jobs[job_id]->pgid));
//        mx_printstr("\n");
//        //------------
        if (m_s->exit_flag == 1 && !(p->type == 10))
            m_s->exit_flag = 0;
        if (p->next != NULL && p->input_path != NULL) { // redirection
            //redirection
        }
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
        p->infile = infile;
        p->outfile = outfile;
        p->errfile = errfile;
        int flag = get_flag(p->argv);
        if (flag) {
        	status = mx_set_parametr(p->argv,m_s);
        }
        else if (p->type != -1)
            status = mx_launch_builtin(m_s, p, job_id);  // fork own buildins
        else
            status = mx_launch_process(m_s, p, job_id, path, env, infile, outfile, errfile);
        if (infile != job->stdin)
            close(infile);
        if (outfile != job->stdout)
            close(outfile);
        infile = mypipe[0];

	}
    if (!shell_is_interactive) {
        status = mx_wait_job(m_s, job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
    }

    else if (status >= 0 && job->foreground == FOREGROUND) {
        tcsetpgrp (STDIN_FILENO, job->pgid);
	    status = mx_wait_job(m_s, job_id);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpid());
        signal(SIGTTOU, SIG_DFL);
	    if (mx_job_completed(m_s, job_id))
	        mx_remove_job(m_s, job_id);
	}
	else if (job->foreground == BACKGROUND) {
	    if (kill (-m_s->jobs[job_id]->pgid, SIGCONT) < 0)
	        perror ("kill (SIGCONT)");
	    mx_print_pid_process_in_job(m_s, job->job_id);
//        if (mx_job_completed(m_s, job_id))
//            mx_remove_job(m_s, job_id);
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

