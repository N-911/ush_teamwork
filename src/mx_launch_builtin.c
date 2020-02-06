#include "ush.h"

int mx_launch_builtin(t_shell *m_s, t_process *p, int job_id) {
    int (*builtin_functions[])(t_shell *m_s, t_process *p) = {&mx_env, &mx_export, &mx_unset,
        &mx_echo, &mx_jobs, &mx_fg, &mx_bg, &mx_cd, &mx_pwd, &mx_which, &mx_exit, NULL};
    int status = 0;
    int shell_is_interactive = isatty(STDIN_FILENO);
    pid_t pid;

    pid_t pgid = m_s->jobs[job_id]->pgid;
    p->status = STATUS_RUNNING;
    if (p->type == 4 || p->type == 5 || p->type == 6) {
        if(!p->pipe && p->foreground)
            mx_remove_job(m_s, job_id);
    }
    // if pipe or in foreground -> fork
    if (p->pipe || !p->foreground) {
        pid = fork();
        p->pid = pid;
        if (pid < 0) {
            perror("error fork");
            exit(1);
        }
        if (pid == 0) {
            //printf("is interactive %d\n", shell_is_interactive);
            if (shell_is_interactive) {
                pid = getpid ();
                if (pgid == 0) pgid = pid;
                    setpgid (pid, pgid);
                }
               // mx_print_color(BLU, "pgid ");
               // mx_print_color(BLU, mx_itoa(pgid));
//                mx_printstr("\n");
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
            if (p->infile != STDIN_FILENO) {
                dup2(p->infile, STDIN_FILENO);
                close(p->infile);
            }
            if (p->outfile != STDOUT_FILENO) {
                dup2(p->outfile, STDOUT_FILENO);
                close(p->outfile);
            }
            if (p->errfile != STDERR_FILENO) {
                dup2(p->errfile, STDERR_FILENO);
                close(p->errfile);
            }
            status = builtin_functions[p->type](m_s, p);
            exit (status);
        }
        else {

            if (kill (-pgid, SIGCONT) < 0)
                perror ("kill (SIGCONT)");
           // mx_print_pid_process_in_job(m_s, job_id);

           // mx_print_color(YEL, "parent\n");
/*
            waitpid(p->pid, &status, WUNTRACED);
            if (WIFEXITED(status)) {
                mx_set_process_status(m_s, pid, STATUS_DONE);
            }
            else if (WIFSIGNALED(status)) {
                mx_set_process_status(m_s, pid, STATUS_TERMINATED);
            }
            else if (WSTOPSIG(status)) {
                status = -1;
                mx_set_process_status(m_s, pid, STATUS_SUSPENDED);
            }
            if (mx_job_completed(m_s, job_id))
                mx_remove_job(m_s, job_id);
//            wait(&status);
*/            //return status;
            }
    }
    else {
        int defoult;
        if(p->input_path) {
            defoult = dup(1);
            if (p->outfile != STDOUT_FILENO) {
                lseek(p->outfile, 0, SEEK_END);  
                dup2(p->outfile, STDOUT_FILENO);
                close(p->outfile);
            }
            if (p->infile != STDIN_FILENO) {  
                dup2(p->infile, STDIN_FILENO);
                close(p->infile);
            }
        }
        status = builtin_functions[p->type](m_s, p);
        p->status = STATUS_DONE;
        if(p->input_path) {
            if (p->outfile != STDOUT_FILENO) {
                dup2(defoult, 1);
                close(defoult);
            }
        }
    }
    return status >> 8;//WEXITSTATUS(status)
}
