#include "ush.h"

/*
 * kill PID PID     send signal CONT to PID
 * kill %job_id     send signal CONT to all proc in job
 * kill %sleep      send signal CONT to all proc in job
 */

static int count_args(char **args) {
    int n_args = 0;

    for (int i = 1; args[i] != NULL; i++) {
        n_args++;
    }
    return n_args;
}


int mx_kill(t_shell *m_s, t_process *p) {
    p->exit_code = 0;
    pid_t pgid = 0;
    int job_id;
    int n_args = 0;
    (void)m_s;

    n_args = count_args(p->argv);
    mx_set_last_job(m_s);

//    if ((job_id = fg_get_job_id(m_s, p)) < 1)
//        return 1;


    if (n_args == 0) {
        mx_printerr("ush: kill: not enough arguments\n");
        p->exit_code = 0;
    }
    if (n_args) {
        for (int i = 1; i <= n_args; i++) {
            if (p->argv[i][0] == '%') {
                if ((job_id = mx_check_args(m_s, p)) < 1) {
                    p->exit_code = 1;
                    continue;
                }
                if ((pgid = mx_get_pgid_by_job_id(m_s, job_id)) < 1) {
                    mx_err_j(p->argv[0], ": ", p->argv[i],": no such job\n");
                    p->exit_code = 1;
                    continue;
                }
//                if (kill(-pgid, SIGCONT) < 0) {
//                    mx_err_j(p->argv[0], ": job not found: ", p->argv[i], "\n");
//                    p->exit_code = 1;
//                }
                printf("pgid 11111 = %d\n", pgid);
            }
//            else if (!(mx_isdigit(p->argv[i][0]))) {
//               if ((job_id = mx_g_find_job(m_s, p->argv[i])) < 1) {
//                   mx_err_j(p->argv[0], ": illegal pid: ", p->argv[i], "\n");
//                   p->exit_code = 1;
//                   continue;
//                }
//                if ((pgid = mx_get_pgid_by_job_id(m_s, job_id)) < 1) {
//                    mx_err_j(p->argv[0], ": ", p->argv[i],": no such job\n");
//                    p->exit_code = 1;
//                    continue;
//                }
//                printf("pgid 22222 = %d\n", pgid);
//            }
            else {
               if (!(mx_isdigit(p->argv[i][0]))) {
                   mx_err_j(p->argv[0], ": illegal pid: ", p->argv[i], "\n");
                   p->exit_code = 1;
                   continue;
               }
               else {
                   pgid = atoi(p->argv[i]);
                   printf("pgid 3333 = %d\n", pgid);
               }
            }
        }
    }
//
//    status = mx_kill_send_signal(m_s, p);
    return p->exit_code;
}
