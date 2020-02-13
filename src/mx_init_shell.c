#include "ush.h"
/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

static char *get_pwd();
static char *get_shlvl();

t_shell *mx_init_shell(int argc, char **argv) {
    pid_t shell_pgid;
//  struct termios shell_tmodes;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive;
    t_shell *m_s = (t_shell *) malloc(sizeof(t_shell));

    m_s->argc = argc;
    m_s->argv = argv;
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
    m_s->pwd = get_pwd();//PWD for further work
    setenv("PWD", m_s->pwd, 1);
    setenv("OLDPWD", m_s->pwd, 1);
    char *shlvl = get_shlvl();
    setenv("SHLVL", shlvl, 1);
    free(shlvl);
    m_s->exported = mx_set_export();
    m_s->variables = mx_set_variables();
    shell_is_interactive = isatty(shell_terminal);  // See if we are running interactively.
//    mx_terminal_init(m_s);
    if (shell_is_interactive) {
        // Loop until we are in the foreground.
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);
        /* Ignore interactive and job-control signals.  */
        //  (void)signal(SIGINT, sigint_handler);
        signal(SIGINT, SIG_IGN);  // Control-C
        signal(SIGQUIT, SIG_IGN);  // 'Control-\'
//        signal(SIGTSTP, mx_sig_h);  // Control-Z
        signal(SIGTSTP, SIG_IGN);  // Control-Z
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGPIPE, mx_sig_h);
//        signal(SIGCHLD, mx_sig_h);
        shell_pgid = getpid();
        if (setpgid(shell_pgid, shell_pgid) < 0) {
            perror("Couldn't put the shell in its own process group");
            exit(1);
        }
        tcsetpgrp(shell_terminal, shell_pgid);  // Grab control of the terminal.
        m_s->shell_pgid = shell_pgid;  //  Save default terminal attributes for shell.
        printf("shell_pgid == %d\n", m_s->shell_pgid);
        tcgetattr(shell_terminal, &m_s->t_original);
        tcgetattr(shell_terminal, &m_s->tmodes);
    }
        for (int i = -1; i < JOBS_NUMBER; ++i) {
            m_s->jobs[i] = NULL;
        }
    m_s->exit_code = -1;
    return m_s;
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

    if (read_link && strcmp(cur_dir, read_link) == 0)
        pwd = getenv("PWD");
    else
        pwd = strdup(cur_dir);
    return pwd;
}
