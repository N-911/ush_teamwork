#include "ush.h"

static int get_flag(char **args);

void mx_launch_job(t_shell *m_s, t_job *job) {
    extern char **environ;
    char **env = environ;
    char *path = getenv("PATH");
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
        // if (p->input_path) { // redirection > >>
        //     int flags;
        //     if (p->redir_delim == R_INPUT)
        //         flags = O_WRONLY|O_CREAT|O_TRUNC;
        //     if (p->redir_delim == R_INPUT_DBL)
        //         flags = O_WRONLY|O_CREAT;
        //     outfile = open (p->input_path, flags , 0666);
        // }
        // if (p->output_path) { // redirection < <<
        //     if (p->redir_delim == R_OUTPUT) {
        //         infile = open (p->output_path, O_RDONLY, 0666);
        //         if (infile < 0) {
        //             mx_printerr("ush :");
        //             perror(p->output_path);
        //             mx_set_variable(m_s->variables, "?", "1");
        //             mx_remove_job(m_s, job_id);
        //             continue ;
        //         }
        //     }
        //     if (p->redir_delim == R_OUTPUT_DBL) {
        //         char *line = mx_strjoin(p->output_path, "a");
        //         while (strcmp(line , p->output_path) != 0) {
        //             printf("heredoc> ");
        //             free(line);
        //             line = NULL;
        //             line = mx_ush_read_line();
        //         }
        //     }
        // }

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
        printf("%d\n", outfile);
        printf("%d\n", infile);
        p->infile = infile;
        p->outfile = outfile;
        p->errfile = errfile;
        int flag = get_flag(p->argv);
        if (flag) {
        	status = mx_set_parametr(p->argv,m_s);
        }
        else if (p->type != -1)
            status = mx_launch_builtin(m_s, p);  
        else
            status = mx_launch_process(m_s, p, job_id, path, env, infile, outfile, errfile);
        if (infile != job->stdin)
            close(infile);
        // if (outfile != job->stdout)
        //     close(outfile);
        infile = mypipe[0];
	    }
	if (status >= 0 && job->foreground == FOREGROUND) {
	    mx_wait_job(m_s, job_id);
	        //mx_print_process_in_job(m_s, job->job_id);
	    if (mx_job_completed(m_s, job_id))
	        mx_remove_job(m_s, job_id);
	}
	else if (job->foreground == BACKGROUND) {
        printf("aaaa\n");
	    if (kill (-job->pgid, SIGCONT) < 0)
	        perror ("kill (SIGCONT)");
	    mx_print_pid_process_in_job(m_s, job->job_id);
	       //mx_destroy_jobs(m_s, job_id);
	}
    mx_set_variable(m_s->variables, "?", mx_itoa(status));
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

