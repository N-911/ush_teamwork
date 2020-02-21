#include "ush.h"

static struct termios mx_disable_term() {
    struct termios savetty;
    struct termios tty;

    tcgetattr (0, &tty);
    savetty = tty;
    tty.c_lflag &= ~(ICANON|ECHO|ISIG|BRKINT|ICRNL
        |INPCK|ISTRIP|IXON|OPOST|IEXTEN);
    tty.c_cflag |= (CS8);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tcsetattr (0, TCSAFLUSH, &tty);
    return savetty;
}

static void mx_enable_term(struct termios savetty) {
    printf("\n");
    tcsetattr (0, TCSAFLUSH, &savetty);
}

static int get_job_type(t_ast **ast, int i) {
    t_ast *tmp = NULL;

    if (i != 0) {
        tmp = ast[i - 1];
        while (tmp->next)
            tmp = tmp->next;
        if (tmp->type == AND || tmp->type == OR)
            return tmp->type;
    }
    return 0;
}

static char *get_line(t_shell *m_s) {
    char *line;
    struct termios savetty;

    mx_edit_prompt(m_s);
    savetty = mx_disable_term();
    m_s->line_len = 1024;
    mx_print_prompt(m_s);
    line = mx_get_keys(m_s);
    if (m_s->history_count == m_s->history_size) {
        m_s->history_size += 1000;
        m_s->history = (char **)realloc(m_s->history, m_s->history_size);
    }
    if (strcmp(line, "") != 0) {
        m_s->history[m_s->history_count] = strdup(line);
        m_s->history_count++;
    }
    m_s->history_index = m_s->history_count;
    mx_enable_term(savetty);
    return line;
}

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
    t_job *new_job;

    m_s->git = mx_get_git_info();
    while (1) {
        line = get_line(m_s);
        if (line[0] == '\0') {
            free(line);
            mx_check_jobs(m_s);
            continue;
        }
        else {
            if ((ast = mx_ast_creation(line, m_s))) {
                for (int i = 0; ast[i]; i++) {
                    new_job = mx_create_job(m_s, ast[i]);
                    new_job->job_type = get_job_type(ast, i);
                    mx_launch_job(m_s, new_job);
                }
                mx_ast_clear_all(&ast);  // clear leeks
            }
        }
        free(line);
    }
}
