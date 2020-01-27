#include "ush.h"

static char *check_path(char **arr, char *command);
static char *get_error(char **name, char *command);
static void print_error(char *command, char *error);

int mx_launch_process(t_shell *m_s, t_process *p, int job_id, char *path, char **env,
        int infile, int outfile, int errfile) {
    int status = 0;
    pid_t child_pid;
    pid_t pgid = m_s->jobs[job_id]->pgid;
    p->status = STATUS_RUNNING;
    int shell_is_interactive = isatty(STDIN_FILENO);  //!!

    child_pid = fork();
    //TELL_WAIT();
    if (child_pid < 0) {
        perror("error fork");
        exit(1);
    }
        //child process
    else if (child_pid == 0) {
        //TELL_PARENT(getpgid(0));
        mx_printstr("child \n");
        if (shell_is_interactive) {
            p->pid = getpid();
            if (pgid > 0)
                setpgid(0, pgid);
            else {
                pgid = p->pid;
                setpgid(0, pgid);
            }
            if (p->foreground)
                tcsetpgrp(STDIN_FILENO, pgid);
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);
            signal(SIGCHLD, SIG_DFL);
        }
        if (infile != STDIN_FILENO) {
            dup2(infile, STDIN_FILENO);
            close(infile);
        }
        if (outfile != STDOUT_FILENO) {
            dup2(outfile, STDOUT_FILENO);
            close(outfile);
        }
        if (errfile != STDERR_FILENO) {
            dup2(errfile, STDERR_FILENO);
            close(errfile);
        }
        char **arr = mx_strsplit(path, ':');
        char *command = p->argv[0];
        path  = check_path(arr, command);
        char *error = get_error(&path, command);

        if (execve(path, p->argv, env) < 0) {
            print_error(command, error);
            // perror("execvp");
            _exit(EXIT_FAILURE);
        }
        _exit(EXIT_SUCCESS);
    }
        //parrent process
    else {
        //WAIT_CHILD();
        mx_printstr("parent\n");
        p->pid = child_pid;  //PID CHILD
        if (shell_is_interactive) {
            if (!pgid)
                pgid = child_pid;
            setpgid(child_pid, pgid);
        }
        if (m_s->jobs[job_id]->foreground == FOREGROUND) {
            tcsetpgrp(0, pgid);
            status = mx_wait_job(m_s, job_id);
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(0, getpid());
            signal(SIGTTOU, SIG_DFL);
            mx_print_job_status(m_s, job_id);
//            if (job_id > 0 && mx_job_completed(m_s, job_id)) {
//                //mx_print_job_status(m_s, job_id);
//                mx_remove_job(m_s, job_id);
            //          }
        }

    }
    return (p->exit_code);
}


static char *check_path(char **arr, char *command) {
    int i = 0;
    char *name = NULL;
    int flag = 0;

    while (arr[i] != NULL && !flag) {
        DIR *dptr  = opendir(arr[i]);
        if (dptr != NULL) {
            struct dirent  *ds;
            while ((ds = readdir(dptr)) != 0) {
                if (strcmp(ds->d_name, command) == 0 && command[0] != '.') {
                    flag++;
                    name = mx_strjoin(arr[i], "/");
                    name = mx_strjoin(name, command);
                    break;
                }
            }
        closedir(dptr);
        }
        i++;
    }
    return name;
}

static char *get_error(char **name, char *command) {
    char *error = NULL;

    if (strstr(command, "/")) {
        *name = command;
        struct stat buff;
        if (lstat(*name, &buff) < 0) {
            error = strdup(": No such file or directory\n");
        }
        else {
            if (mx_get_type(buff) == 'd') {
                error = strdup(": is a directory\n");
            }
        }
    } 
    else
        error = strdup(": command not found\n");
    return error;
}

static void print_error(char *command, char *error) {
    mx_printerr("ush: ");
    if (error) {
        mx_printerr(command);
        mx_printerr(error);
    }
    else
        perror(command);
}


/*
 *

            {
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
                status = mx_launch_process(m_s, p, path, env);
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
                if (job->first_process->foreground == FOREGROUND) {
                    tcsetpgrp(0, job->pgid);
                    mx_wait_job(m_s, job->job_id);
                    signal(SIGTTOU, SIG_IGN);
                    tcsetpgrp(0, getpid());
                    signal(SIGTTOU, SIG_DFL);
                    mx_print_job_status(m_s, job_id);
                }
                if (p->infile != job->stdin)
                    close(p->infile);
                if (p->outfile != job->stdout)
                    close(p->outfile);
                p->infile = mypipe[0];
            }
        }
    }

 */
