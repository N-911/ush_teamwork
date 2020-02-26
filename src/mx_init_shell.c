#include "ush.h"

static void shell_grp_help(t_shell *m_s, pid_t shell_pgid) {
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0) {
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }
    tcsetpgrp(STDIN_FILENO, shell_pgid);  // Grab control of the termina
    m_s->shell_pgid = shell_pgid;
}

static void set_shell_grp(t_shell *m_s) {
    pid_t shell_pgid;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive = isatty(shell_terminal);

    if (shell_is_interactive) {
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);
        signal(SIGINT, MX_SIG_IGN);
        signal(SIGQUIT, MX_SIG_IGN);
        signal(SIGTSTP, MX_SIG_IGN);
        signal(SIGTTIN, MX_SIG_IGN);
        signal(SIGTTOU, MX_SIG_IGN);
        shell_grp_help(m_s, shell_pgid);
        char *c_shell_pgid = mx_itoa(m_s->shell_pgid);
        mx_set_variable(m_s->variables, "$", c_shell_pgid);
        free(c_shell_pgid);
        tcgetattr(shell_terminal, &m_s->t_original);
        tcgetattr(shell_terminal, &m_s->tmodes);
    }
}

static char *get_pwd() {
    char *pwd = getenv("PWD");
    char *cur_dir = getcwd(NULL, 256);
    char *read_link = realpath(pwd, NULL);

    if (read_link && strcmp(cur_dir, read_link) == 0){
        pwd = strdup(getenv("PWD"));
        free(read_link);
        free(cur_dir);
    }
    else {
        pwd = strdup(cur_dir);
        free(cur_dir);
    }
    return pwd;
}

static void set_shell_defaults(t_shell *m_s) {
    char *b_list[14] = {"env", "export", "unset", "echo", "jobs", "fg", "bg",
                        "cd", "pwd", "which", "exit", "set", "mx_kill", NULL};

    m_s->builtin_list = (char **) malloc(sizeof(char *) * 14);
    for (int i = 0; i < 15; i++)
        m_s->builtin_list[i] = b_list[i];
    m_s->max_number_job = 1;
    m_s->exit_flag = 0;
    m_s->history_count = 0;
    m_s->history_size = 1000;
    m_s->history = (char **)malloc(sizeof(char *) * m_s->history_size);
    for (int i = -1; i < MX_JOBS_NUMBER; ++i)
        m_s->jobs[i] = NULL;
    mx_init_jobs_stack(m_s);
}

t_shell *mx_init_shell(int argc, char **argv) {
    char *shlvl;
    t_shell *m_s = (t_shell *) malloc(sizeof(t_shell));
    set_shell_defaults(m_s);
    m_s->argc = argc;
    m_s->argv = argv;
    m_s->pwd = get_pwd();
    setenv("PWD", m_s->pwd, 1);
    setenv("OLDPWD", m_s->pwd, 1);
    setenv("PATH", "/Users/mlibovych/.brew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:/usr/local/munki", 1);
    shlvl = mx_get_shlvl();
    setenv("SHLVL", shlvl, 1);
    free(shlvl);
    m_s->exported = mx_set_export();
    m_s->variables = mx_set_variables();
    m_s->prompt = strdup("u$h");
    m_s->prompt_status = 1;
    mx_set_variable(m_s->variables, "PROMPT", "u$h");
    set_shell_grp(m_s);
    m_s->exit_code = -1;
    return m_s;
}
