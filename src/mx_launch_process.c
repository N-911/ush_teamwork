#include "ush.h"

static char *check_path(char **arr, char *command);
static char *get_error(char **name, char *command, int *status);
static void print_error(char *command, char *error);
static void child_work(t_shell *m_s, t_process *p, int job_id, char *path, char **env, int child_pid);

int mx_launch_process(t_shell *m_s, t_process *p, int job_id, char *path, char **env) {
//    int status = 0;
    pid_t child_pid;
    p->status = MX_STATUS_RUNNING;
    int shell_is_interactive = isatty(STDIN_FILENO);  //!!
    child_pid = fork();
    if (child_pid < 0) {
        perror("error fork");
        exit(1);
    }
    else if (child_pid == 0)
        child_work(m_s, p, job_id, path, env, child_pid);
    else {
        p->pid = child_pid;
        if (shell_is_interactive) {
            pid_t pid = child_pid;
            if (m_s->jobs[job_id]->pgid == 0)
                m_s->jobs[job_id]->pgid = pid;
            setpgid (pid, m_s->jobs[job_id]->pgid);
        }
    }
    return p->status >> 8;//WEXITSTATUS(status)
}


static void child_work(t_shell *m_s, t_process *p, int job_id, char *path, char **env, int child_pid) {
    int shell_is_interactive = isatty(STDIN_FILENO);  //!!

    if (shell_is_interactive)
        mx_pgid(m_s, job_id, child_pid);
    mx_dup_fd(p);
    char **arr = mx_strsplit(path, ':');
    char *command = p->argv[0];
    path  = check_path(arr, command);
    mx_del_strarr(&arr);
    char *error = get_error(&path, command, &p->status);
    if (execve(path, p->argv, env) < 0) {
        print_error(command, error);
        free(error);
        free(path);
        _exit(p->status);
    }
    free(path);
    free(error);
    exit(p->status);
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
                    char *tmp = mx_strjoin(arr[i], "/");
                    name = mx_strjoin(tmp, command);
                    free(tmp);
                    break;
                }
            }
            closedir(dptr);
        }
        i++;
    }
    return name;
}

static char *get_error(char **name, char *command, int *status) {
    char *error = NULL;

    *status = 127;
    if (strstr(command, "/")) {
        *name = command;
        struct stat buff;
        if (lstat(*name, &buff) < 0) {
            error = NULL;//strdup(": No such file or directory\n");
        }
        else {
            if (mx_get_type(buff) == 'd') {
                error = strdup(": is a directory\n");
                *status = 126;
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
