#include "ush.h"

static char *subshell_parent(t_shell *m_s, int *fd1, int *fd2, int pid) {
    size_t n_read = 0;
    size_t sum_read = 0;
    char buf[BUFSIZ];
    int status;
    char *res = NULL;
    char *token;
    size_t len_token;
    size_t sum_len = 0;
    char *tokens = NULL;

    close(fd1[0]);
    close(fd2[1]);
    while ((n_read = read(fd2[0], buf, BUFSIZ)) > 0) {
        res = realloc(res, sum_read + n_read + 1);
        memcpy(&res[sum_read], buf, n_read);
        sum_read += n_read;
    }
    if (sum_read > 0) {
        if (res[sum_read - 1] == '\n')
            res[sum_read - 1] = 0;
    }
    waitpid(pid, &status, MX_WNOHANG | MX_WUNTRACED | MX_WCONTINUED);
    m_s->exit_code = status;
    close(fd2[0]);


    printf("res %s\n", res);


    token = mx_strtok(res, MX_USH_TOK_DELIM);
    while (token != NULL) {
        len_token = strlen(token);

        printf("len_tok - %zu\n", len_token);

        tokens = realloc(tokens, len_token + 1);

        strcat((tokens + len_token)), " ");
        sum_len += len_token;

        strcat((tokens + sum_len), token);
        token = mx_strtok(NULL, MX_USH_TOK_DELIM);
    }
    printf("sum_len - %zu\n", sum_len);
    tokens[sum_len] = '\0';
    printf("tokens - %s\n", tokens);


    return tokens;
}

static char *exec_subshell (t_shell *m_s, int *fd1, int *fd2) {
    char *path = mx_strjoin(m_s->kernal, "/ush");
    extern char **environ;
    pid_t pid;
    char *res = NULL;

    if ((pid = fork()) < 0) {
        perror("error fork");
    }
    else if (pid > 0)  // Work parent
        res = subshell_parent(m_s, fd1, fd2, pid);
    else {  // Work child
        mx_dup2_fd(fd1, fd2);
        if (execve(path, NULL, environ) < 0) {
            perror("ush ");
            _exit(EXIT_SUCCESS);
        }
        exit(EXIT_SUCCESS);
    }
    mx_strdel(&path);
    return res;
}


char *mx_run_subshell(char *substr, t_shell *m_s) {
    int len;
    int fd1[2];
    int fd2[2];
    char *res = NULL;

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    len = strlen(substr) + 1;
    if (write(fd1[1], substr, len) != len)
        perror("error write to pipe");
    close(fd1[1]);
    mx_strdel(&substr);
    res = exec_subshell(m_s, fd1, fd2);
    return res;
}


void mx_dup2_fd(int *fd1, int *fd2) {
    if (fd1[0] != STDIN_FILENO) {
        if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
            perror("error dup2 stdin");
        close(fd1[0]);
    }
    if (fd2[1] != STDOUT_FILENO) {
        if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
            perror("error dup2 stdout");
        close(fd2[1]);
    }
}
