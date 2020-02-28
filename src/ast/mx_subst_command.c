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
        if (str && str[mx_strlen(str) - 1] == '\n')
            str[mx_strlen(str) - 1] = '\0';
        remove("ttt");
        remove("fff");
    }
    return str;
}

/*
 * Combine new string.
 */
static char *expantion(char *s, int pos, t_shell *m_s) {
    char *res = NULL;
    int len = 0;
    char *subst;

    res = mx_strndup(s, pos);
    if ((subst = get_subst(&s[pos], &len))) {
        subst = exec_subshell(subst, m_s);
        res = mx_strjoin_free(res, subst);
        // mx_strdel(&subst);
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
