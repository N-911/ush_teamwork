#include "ush.h"

static char *get_pwd();
static char *get_shlvl();
static void set_shell_defaults(t_shell *m_s);
static void set_shell_grp(t_shell *m_s);

t_shell *mx_init_shell(int argc, char **argv) {
//  struct termios shell_tmodes; // not used?? - del
    t_shell *m_s = (t_shell *) malloc(sizeof(t_shell));
    set_shell_defaults(m_s);
    m_s->argc = argc;
    m_s->argv = argv;
    m_s->pwd = get_pwd();//PWD for further work
    setenv("PWD", m_s->pwd, 1);
    setenv("OLDPWD", m_s->pwd, 1);
    char *shlvl = get_shlvl();
    setenv("SHLVL", shlvl, 1);
    free(shlvl);
    m_s->exported = mx_set_export();
    m_s->variables = mx_set_variables();
    m_s->prompt = strdup("u$h");
    m_s->prompt_status = 1;
    mx_set_variable(m_s->variables, "PROMPT", "u$h");
    mx_set_variable(m_s->variables, "PROMPT1", "Auditor dlya lohov>");
    set_shell_grp(m_s);
//    mx_terminal_init(m_s);
    m_s->exit_code = -1;
    return m_s;
}


static void set_shell_grp(t_shell *m_s) {
    pid_t shell_pgid;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive;

    shell_is_interactive = isatty(shell_terminal);  // See if we are running interactively.
    if (shell_is_interactive) {
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);
        signal(SIGINT, SIG_IGN);  // Control-C
        signal(SIGQUIT, SIG_IGN);  // 'Control-\'
        signal(SIGTSTP, SIG_IGN);  // Control-Z
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }
        tcsetpgrp(shell_terminal, shell_pgid);  // Grab control of the terminal.
        m_s->shell_pgid = shell_pgid;  //  Save default terminal attributes for shell.
        char *c_shell_pgid = mx_itoa(m_s->shell_pgid);
        mx_set_variable(m_s->variables, "$", c_shell_pgid);
        free(c_shell_pgid);
        tcgetattr(shell_terminal, &m_s->t_original);
        tcgetattr(shell_terminal, &m_s->tmodes);
    }
}

static char *get_shlvl() {
    char *shlvl = NULL;

    shlvl = getenv("SHLVL");
    int lvl = atoi(shlvl);
    lvl++;
    shlvl = mx_itoa(lvl);
    return shlvl;
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
    m_s->builtin_list = (char **) malloc(sizeof(char *) * 12);
    m_s->builtin_list[0] = "env";
    m_s->builtin_list[1] = "export";
    m_s->builtin_list[2] = "unset";
    m_s->builtin_list[3] = "echo";  // not full
    m_s->builtin_list[4] = "jobs";
    m_s->builtin_list[5] = "fg";
    m_s->builtin_list[6] = "bg";
    m_s->builtin_list[7] = "cd";
    m_s->builtin_list[8] = "pwd";
    m_s->builtin_list[9] = "which";
    m_s->builtin_list[10] = "exit";
    m_s->builtin_list[11] = NULL;
    m_s->max_number_job = 1;
    m_s->exit_flag = 0;
    mx_init_jobs_stack(m_s);
    m_s->history_count = 0;
    m_s->history_size = 1000;
    m_s->history = (char **)malloc(sizeof(char *) * m_s->history_size);
    for (int i = -1; i < MX_JOBS_NUMBER; ++i)
        m_s->jobs[i] = NULL;
}
