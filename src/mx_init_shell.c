#include "ush.h"
/* Make sure the shell is running interactively as the foreground job
   before proceeding. */

t_shell *mx_init_shell(int argc, char **argv) {
    pid_t shell_pgid;
//  struct termios shell_tmodes;
    int shell_terminal = STDIN_FILENO;
    int shell_is_interactive;
    t_shell *m_s = (t_shell *) malloc(sizeof(t_shell));

    m_s->argc = argc;
    m_s->argv = argv;
    m_s->builtin_list = (char **) malloc(sizeof(char *) * 5);
    m_s->builtin_list[0] = "echo";
    m_s->builtin_list[1] = "jobs";
    m_s->builtin_list[2] = "fg";
    m_s->builtin_list[3] = "exit";
    m_s->builtin_list[4] = NULL;
    m_s->job_control = (char **) malloc(sizeof(char *) * 4);
    m_s->job_control[0] = "jobs";
    m_s->job_control[1] = "fg";
    m_s->job_control[2] = "bg";
    m_s->job_control[3] = NULL;

    m_s->max_number_job = 2;

    /* See if we are running interactively.  */
    shell_is_interactive = isatty(shell_terminal);
//    mx_terminal_init(m_s);
    if (shell_is_interactive) {
        // Loop until we are in the foreground.
        while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp()))
            kill(-shell_pgid, SIGTTIN);
        /* Ignore interactive and job-control signals.  */
        //  (void)signal(SIGINT, sigint_handler);

        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGCHLD, SIG_IGN);
        /* Put ourselves in our own process group.  */
        shell_pgid = getpid();
        printf("parent shell_pgid %d\n", shell_pgid);
       // tcgetpgrp(int fd);
        /*The function tcgetpgrp() returns the process group ID of the foreground process
         * group on the terminal associated to fd */
        printf("идентификатор группы процессов  %d\n", tcgetpgrp(shell_pgid));
        if (setpgid (shell_pgid, shell_pgid) < 0) {
            perror ("Couldn't put the shell in its own process group");
            exit (1);
        }
        /* Grab control of the terminal.  */
        tcsetpgrp (shell_terminal, shell_pgid);
        /* Save default terminal attributes for shell.  */
        m_s->shell_pgid = shell_pgid;
        tcgetattr(shell_terminal, &m_s->t_original);
        for (int i = -1; i < JOBS_NUMBER; ++i) {
            m_s->jobs[i] = NULL;
        }
     }
    return m_s;
}
