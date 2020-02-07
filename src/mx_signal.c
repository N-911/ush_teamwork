#include "ush.h"

void sigchld_handler(int signum) {
    int pid, status, serrno;
    serrno = errno;
    while (1) {
        pid = waitpid(WAIT_ANY, &status, WNOHANG);
        if (pid < 0) {
            perror("waitpid");
            break;
        }
        if (pid == 0)
            break;
        //notice_termination (pid, status);
    }
    errno = serrno;
    printf("%d\n", signum);
}

void mx_sig_handler(int signal) {
    if (signal == SIGTSTP)
        printf("\n");
    if (signal == SIGINT){
        printf("\n%s%s%s", GRN, "u$h> ", RESET);
    }
    //printf("%d\n", signal);
}

void mx_sig_handler_exit(int sig) {
    // clean all
    signal(sig, SIG_DFL);
    raise(sig);
}

void mx_sig_h(int signal) {
//    (void)m_s;
    if (signal == SIGTSTP)
        printf("\n");
    if (signal == SIGINT)
        printf("\n");
    if (signal == SIGCHLD) {
        printf("\n");
//        mx_check_jobs(void)m_s);
    }

    // printf("%d\n", signal);
/*
 * handler fo exit
 * sig_int_flag = 1;        // установить флаг для проверки if exit
 *
    if (signo == SIGINT)
    {
        if (g_sh->state == STATE_READ)
        {
            ft_bzero(g_sh->buffer, g_sh->bufsize);
            g_sh->buf_i = 0;
            g_sh->input_size = 0;
            ft_printf(STDOUT_FILENO, "\n");
            display_shell_prompt(g_sh);
        }
        else if (g_sh->state == STATE_EXEC)
        {
        }
    }
    */

}

void sig_usr(int signo) {
    int a = 0;
    a += signo;
    sigflag = 1;
}

void TELL_WAIT(void) {
    if (signal(SIGUSR1, sig_usr) == SIG_ERR)
        mx_printerr("ошибка вызова функции signal(SIGUSR1)");
    if (signal(SIGUSR2, sig_usr) == SIG_ERR)
        mx_printerr("ошибка вызова функции signal(SIGUSR2)");
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
        mx_printerr("ошибка выполнения операции SIG_BLOCK");
}

void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2); // сообщить родительскому процессу, что мы готовы
}

void WAIT_PARENT(void) {
    while (sigflag == 0)
        sigsuspend(&zeromask); // ждать ответа от родительского процесса
    sigflag = 0;
    //Восстановить маску сигналов в начальное состояние.
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        mx_printerr("ошибка выполнения операции SIG_SETMASK");
}

void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1); // сообщить дочернему процессу, что мы готовы
}

void WAIT_CHILD(void) {
    while (sigflag == 0)
        sigsuspend(&zeromask); // дождаться ответа от дочернего процесса
    sigflag = 0;
//Восстановить маску сигналов в начальное состояние.
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
        mx_printerr("ошибка выполнения операции SIG_SETMASK");
}

/*
void sig_int(int), sig_quit(int);
if (signal(SIGINT, SIG_IGN) != SIG_IGN)
signal(SIGINT, sig_int);
if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
signal(SIGQUIT, sig_quit);

 */

/*
void	sigint_handler(int signo)
{
    if (signo == SIGINT)
    {
        if (g_sh->state == STATE_READ)
        {
            ft_bzero(g_sh->buffer, g_sh->bufsize);
            g_sh->buf_i = 0;
            g_sh->input_size = 0;
            ft_printf(STDOUT_FILENO, "\n");
            display_shell_prompt(g_sh);
        }
        else if (g_sh->state == STATE_EXEC)
        {
        }
    }
}
*/

//static volatile sig_atomic_t sigflag; // устанавливается обработчиком  в ненулевое значение
//static sigset_t newmask, oldmask, zeromask;
