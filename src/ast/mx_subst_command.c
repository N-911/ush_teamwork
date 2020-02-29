#include "ush.h"
/*
 * Get substitution.
 */
static char *get_subst(char *s, int *len) {
    char *subst = NULL;
    int i = 0;

    if (s[0] == '$' && s[1] == '(') {
        i = mx_get_char_index_quote(s, ")", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[2], i - 2);
        *len = i + 1;
    }
    else if (s[0] == '`') {
        i = mx_get_char_index_quote(&s[1], "`", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[1], i);
        *len = i + 1;
    }
    return subst;
}


char *exec_subshell(char *substr, t_shell *m_s) {
    pid_t pid;
    int status;
    char *path = mx_strjoin(m_s->kernal, "/ush");
    extern char **environ;
    int len;
    int fd1[2];
    int fd2[2];
    char buf[BUFSIZ];
    size_t n_read = 0;
    char *res = NULL;

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("pipe");
        exit(1);
    }
    len = strlen(substr) + 1;
    if (write(fd1[1], substr, len) != len)
        perror("error write to pipe");
    close(fd1[1]);
    if ((pid = fork()) < 0) {
        perror("error fork");
    }
    else if (pid > 0) {  // Parent
        close(fd1[0]);
        close(fd2[1]);
        int test = 0;
        while ((n_read = read(fd2[0], buf, BUFSIZ)) > 0) {
            res = realloc(res, test + n_read + 1);
            memcpy(&res[test], buf,  n_read);
            test += n_read;
        }
        if (test > 0) {
            if (res[test - 1] == '\n')
                res[test - 1] = 0;
        }
//        n_read = read(fd2[0], buf, BUFSIZ);
//        buf[n_read - 1] = '\0';
        waitpid(pid, &status, MX_WNOHANG | MX_WUNTRACED | MX_WCONTINUED);
        m_s->exit_code = status;
        close(fd2[0]);
    }
    else {
        mx_dup2_fd(fd1, fd2);
        if (execve(path, NULL, environ) < 0) {
            perror("ush ");
            _exit(EXIT_SUCCESS);
        }
        exit(EXIT_SUCCESS);
    }
    mx_strdel(&path);
//    return strdup(buf);
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

/*
 * Combine new string.
 */

static char *expantion(char *s, int pos, t_shell *m_s) {
    char *res = NULL;
    int len = 0;
    char *subst = NULL;

    res = mx_strndup(s, pos);
    if ((subst = get_subst(&s[pos], &len))) {
        subst = exec_subshell(subst, m_s);
        res = mx_strjoin_free(res, subst);
        //mx_strdel(&subst);
    }
    if (s[pos + len + 1])
        res = mx_strjoin_free(res, &s[pos + len + 1]);
    mx_strdel(&s);
    return res;
}

/*
 * Command substitutiont.
 */

char *mx_subst_command(char *s, t_shell *m_s) {
    char *res = s;
    int pos = 0;

    if (!s || !*s)
        return s;
    while (res && (pos = mx_get_char_index_quote(res, "`$", "\'")) >= 0)
        if (!(res = expantion(res, pos, m_s))) {
            mx_printerr("u$h: command substitushion doesn't work.\n");
            return NULL;
        }
    return res;
}

/*
char *exec_subshell(char *substr, t_shell *m_s) {
    char *str = NULL;
    pid_t pid;
    int status = 1;
    char *path = mx_strjoin(m_s->kernal, "/ush");
    extern char **environ;

    pid = fork();
    if (pid == 0) {
        int fd = open("fff", O_RDWR | O_CREAT | O_TRUNC, 0666);
        int out = open("ttt", O_RDWR | O_CREAT | O_TRUNC, 0666);
        write(fd, substr, mx_strlen(substr));
        close(fd);
        fd = open("fff", O_RDWR, 0666);
        dup2 (fd, 0);
        dup2 (out, 1);
        if (execve(path, NULL, environ) < 0) {
            perror("ush ");
            exit(status);
        }
        exit(0);
    }
    else if (pid < 0)
        perror("ush "); 
    else {
        waitpid(pid, &status, 0);
        str = mx_file_to_str("ttt");
        if (str) {
            char *tmp = strdup(str);
            free(str);
            str = strndup(tmp,mx_strlen(tmp) - 1);
            free(tmp);
        }
        remove("ttt");
        remove("fff");
    }
    return str;
}
*/

