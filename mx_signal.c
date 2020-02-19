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
    if (signal == SIGPIPE)
        mx_printerr("err write PIPE!!!!\n");
}

