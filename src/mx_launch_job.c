#include "ush.h"


static void execute_job_env(t_job *job) {
    extern char **environ;

    job->env = environ;
    job->path = getenv("PATH");
    if (!job->path)
        job->path = "";
}

//static void help_ex_job(t_shell *m_s, t_job *job, t_process *p, int job_id) {
////    p->r_infile[0] = job->infile;
//    p->r_outfile[0] = job->outfile;
//    p->errfile = job->errfile;
//    job->flag = 0;
//
//    /////////
//    mx_print_fd(p);
//    if (!p->pipe)
//        job->flag = mx_get_flag(p->argv);
//    if (job->flag) {
//        job->exit_code = mx_set_parametr(p->argv, m_s);
//        mx_remove_job(m_s, job_id);
//    }
//    else if (p->type != -1)
//        job->exit_code = mx_launch_builtin(m_s, p, job_id);
//    else
//        job->exit_code = mx_launch_process(m_s, p, job_id);
//    if (job->infile != job->stdin)
//        close(job->infile);
//    if (job->outfile != job->stdout)
//        close(job->outfile);
//    m_s->exit_code = job->exit_code;
//}

void mx_launch_job(t_shell *m_s, t_job *job) {
    setbuf(stdout, NULL);
    int status;
    int job_id;

    mx_check_jobs(m_s);
    job_id = mx_insert_job(m_s, job);
    if (!job->job_type)
        status = mx_execute_job(m_s, job, job_id);
    else if (job->job_type == AND && m_s->exit_code == 0)
        status = mx_execute_job(m_s, job, job_id);
    else if (job->job_type == OR && m_s->exit_code != 0)
        status = mx_execute_job(m_s, job, job_id);
    else
        mx_remove_job(m_s, job_id);
    char *exit_status = mx_itoa(m_s->exit_code);
    mx_set_variable(m_s->variables, "?", exit_status);
    free(exit_status);
}


 int mx_execute_job(t_shell *m_s, t_job * job, int job_id) {
    t_process *p;
    int mypipe[2];
//    int mypipe_redir[2];

    execute_job_env(job);
    for (p = m_s->jobs[job_id]->first_pr; p; p = p->next) {
        mx_print_info(m_s, job, p, job_id);  ///****************
        mx_sheck_exit(m_s, p);
        int a;  //////********
        if ((a = mx_set_redirections(m_s, job, p)) != 0)
            continue;
        if (p->pipe) {
//            printf ("pipe\n");
            if (pipe(mypipe) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            job->outfile = mypipe[1];
            p->r_outfile[0] = job->outfile;
        }
        //    p->r_infile[0] = job->infile;
        job->flag = 0;

        if (p->c_output > 1) {
            if (pipe(p->redirect->mypipe_redir) < 0) {
                perror("pipe");
                mx_remove_job(m_s, job_id);
                exit(1);
            }
            printf("red _pipe %d, %d\n", p->redirect->mypipe_redir[0], p->redirect->mypipe_redir[1]);
        }
        mx_print_fd(p);  /////////
        if (!p->pipe)
            job->flag = mx_get_flag(p->argv);
        if (job->flag) {
            job->exit_code = mx_set_parametr(p->argv, m_s);
            mx_remove_job(m_s, job_id);
        }
        else if (p->type != -1)
            job->exit_code = mx_launch_builtin(m_s, p, job_id);
        else
            job->exit_code = mx_launch_process(m_s, p, job_id);

        if (p->c_output > 1) {
            mx_read_from_pipe(p);
        }
        if (job->infile != job->stdin)
            close(job->infile);
        if (job->outfile != job->stdout)
            close(job->outfile);
        m_s->exit_code = job->exit_code;
        job->infile = mypipe[0];
    }
    mx_launch_help(m_s, job, job_id, job->exit_code);
    return job->exit_code;
}

void mx_launch_help (t_shell *m_s, t_job *job, int job_id, int status) {
    int shell_terminal = STDIN_FILENO;

    if (job->foregrd) {
        tcsetpgrp(STDIN_FILENO, job->pgid);
        if (status == 0)
            status = mx_wait_job(m_s, job_id);
        if (mx_job_completed(m_s, job_id))
            mx_remove_job(m_s, job_id);
        signal(SIGTTOU, MX_SIG_IGN);
        tcsetpgrp(STDIN_FILENO, getpid());
        tcgetattr(shell_terminal, &job->tmodes);
        tcsetattr(shell_terminal, TCSADRAIN, &m_s->tmodes);
    }
    else
        mx_print_pid_process_in_job(m_s, job->job_id);
    m_s->exit_code = status;
}

void mx_read_from_pipe(t_process *p) {
    printf ("read from pipe\n");
    int j;
    size_t n_read = 0;
    size_t sum_read = 0;
    char buf[BUFSIZ];
    char *res = NULL;

//    printf("+++++++1\n");
//    while ((n_read = read(fd_redir, buf, BUFSIZ)) >= 0) {
        n_read = read(p->redirect->mypipe_redir[0], buf, BUFSIZ);
//    printf("n_read = %zu\n", n_read);
        res = realloc(res, sum_read + n_read + 1);
        memcpy(&res[sum_read], buf, n_read);
        sum_read += n_read;
//        printf("res  = \n%s\n", res);
//    }
//    printf("n_read_2 = %zu\n", n_read);
//    printf("sum_read = %zu\n", sum_read);
    if (sum_read > 0) {
        if (res[sum_read - 1] == '\n')
            res[sum_read - 1] = 0;
    }
//    printf("res  = \n%sn", res);
    close(p->redirect->mypipe_redir[0]);
    for (j = 0; j < p->c_output; j++) {
        write(p->r_outfile[j], res, sum_read);
        close(p->r_outfile[j]);
    }
}


/*
 *
 * //    size_t n_outputs = 0;
//    FILE **descriptors;
//    char buffer[BUFSIZ];
//    ssize_t bytes_read = 0;
//    bool ok = true;
//    char const *mode_string =  ("rw");
//    descriptors = malloc (sizeof (*descriptors) * (p->c_output + 1));
//    descriptors[0] = stdout;
//    n_outputs++;
//    setvbuf (stdout, NULL, _IONBF, 0);




    if (p->redirect) {
        for (r = p->redirect, j = 1; r; r = r->next, j++) {
//            printf("read from pipe \n")mypipe_redir[1];
//            if (r->redir_delim == R_OUTPUT) {
//                flags = O_WRONLY | O_CREAT | O_TRUNC;
//            }
//            if (r->redir_delim == R_OUTPUT_DBL) {
//                flags = O_WRONLY | O_CREAT;
            descriptors[j] = fopen(r->output_path, mode_string);
            if (descriptors[j] == NULL) {
                mx_printerr("ush :");
                perror(r->output_path);
//                mx_set_variable(m_s->variables, "?", "1");
//                m_s->redir = 1;
//                job->exit_code = 1;
            }
            else {
                setvbuf (descriptors[j], NULL, _IONBF, 0);
                n_outputs++;
            }
        }
    }

    printf("n_outputs  %zu\n", n_outputs);

    while (n_outputs) {
        bytes_read = read (p->r_outfile[0], buffer, sizeof buffer);
        if (bytes_read < 0 && errno == EINTR)
            continue;
        if (bytes_read <= 0)
            break;


*/

/*
for (size_t i = 0; i < n_outputs; i++) {
    if (descriptors[i] && fwrite(buffer, bytes_read, 1, descriptors[i]) != 1) {
//                int w_errno = errno;
//                bool fail = errno != EPIPE || (output_error == output_error_exit
//                                               || output_error == output_error_warn);
        if (descriptors[i] == stdout)
            clearerr(stdout); // Avoid redundant close_stdout diagnostic.
//                if (fail) {
//                    error(output_error == output_error_exit
//                          || output_error == output_error_exit_nopipe,
//                          w_errno, "%s", quotef(files[i]));
//                }
        descriptors[i] = NULL;
//                if (fail)
//                    ok = false;
        n_outputs--;
    }
}



}
if (bytes_read == -1) {
perror("read error");
//        error (0, errno, _("read error"));
//        ok = false;
}
for (int i = 1; i <= p->outfile; i++) {
if (descriptors[i] && fclose(descriptors[i]) != 0) {
    perror("close error\n");
//            error (0, errno, "%s", quotef (files[i]));
//            ok = false;
}
}
free (descriptors);
//    return ok;
 */


/*
 *


    int total = 0;
    char buffer[248];
    int size = 248;
    int in = 0;

    int rc = read(fd_pipe, &buffer, 16);
    while (rc > 0 && size > 0) {
        size -= rc;
        in += rc;
        total += rc;
        rc = read(fd_pipe, &buffer, 16);
    }
    write (fd_0, buffer, sizeof(buffer));
    write (fd_1, buffer, sizeof(buffer));
    close(fd_0);
    close(fd_1);
 */
