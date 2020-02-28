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

//PIPE_BUF.
char *exec_subshell(char *substr, t_shell *m_s) {
    pid_t pid;
    int status;
    char *path = mx_strjoin(m_s->kernal, "/ush");
    extern char **environ;

    printf("substr = %s\n", substr);
    printf ("1@@@@@@@@@\n");
    int fd1[2];
    int fd2[2];
    int MAXLINE = 1024;
    char line[MAXLINE];
//    (void)substr;

    if (pipe(fd1) < 0 || pipe(fd2) < 0) {
        perror("pipe");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("ошибка вызова функции fork");
    }
    else if (pid > 0) {
        close(fd1[0]);
        close(fd2[1]);
        printf ("1  parent @@@@@@@@@\n");

//        char *line = NULL;
//        size_t linecap = 0;
        int a;
        int n = strlen(substr);

//        if (getline(&line, &substr, fd1[1]) < 0)
//            perror("ошибка записи в канал");

        if (write(fd1[1], substr, n) != n)
            perror("ошибка записи в канал");


        if ((a = read(fd2[0], &line, MAXLINE)) < 0)
            perror("ошибка чтения из канала");
//        line[a] = '\0';

        printf("res parent line = %s\n", line);
        waitpid(pid, &status, MX_WNOHANG | MX_WUNTRACED | MX_WCONTINUED);
        if (ferror(stdin))
            perror("ошибка получения данных со стандартного ввода");
//        wait(NULL);
        printf ("11  parent @@@@@@@@@\n");

//        exit(0);
    }
    else {
        printf ("2  child @@@@@@@@@\n");
        close(fd1[1]);
        close(fd2[0]);
        int  b;
        if ((b = read(fd1[0], line, MAXLINE)) < 0)
            perror("ошибка чтения из канала");

        line[b] = '\0';
        printf("res child line = %s\n", line);


        if (fd1[0] != STDIN_FILENO) {
            if (dup2(fd1[0], STDIN_FILENO) != STDIN_FILENO)
                perror("ошибка вызова функции dup2 для stdin");
            close(fd1[0]);
        }
        if (fd2[1] != STDOUT_FILENO) {
            if (dup2(fd2[1], STDOUT_FILENO) != STDOUT_FILENO)
                perror("ошибка вызова функции dup2 для stdout");
            close(fd2[1]);
        }

        printf ("\n22  child @@@@@@@@@\n");
        if (execve(path, NULL, environ) < 0) {
            perror("ush ");
            exit(0);
        }
        exit(0);
    }
    return strdup(line);
}


/*
void read_from_pipe (int file) {
    FILE *stream;
    int c;
    stream = fdopen (file, 'r');
    while ((c = fgetc (stream)) != EOF)
        putchar (c);
    fclose (stream);
}



void write_to_pipe (int file) {
    FILE *stream;
    stream = fdopen (file, 'w');
    fprintf (stream, 'hello, world!\n');
    fprintf (stream, 'goodbye, world!\n');
    fclose (stream);
}


*/


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
        if (str && str[mx_strlen(str) - 1] == '\n')
            str[mx_strlen(str) - 1] = '\0';
        remove("ttt");
        remove("fff");
    }
    return str;
}
*/
