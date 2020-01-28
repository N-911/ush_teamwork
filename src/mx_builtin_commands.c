#include "ush.h"

int mx_builtin_commands_idex(t_shell *m_s, char *command) {
    int i = 0;

    for (i = 0; m_s->builtin_list[i] != NULL; i++) {
        if (strcmp(command, m_s->builtin_list[i]) == 0)
            return (i);
    }
    return (-1);
}

int mx_jobs(t_shell *m_s, t_process *p) {
    for (int i = 0; i < JOBS_NUMBER; i++) {
        if (m_s->jobs[i] != NULL) {
            mx_print_job_status(m_s, i);
        }
    }
    p->exit_code = 0;
    return 0;
}

int mx_exit(t_shell *m_s, t_process *p) {
//    kill (getpid(), SIGTSTP);
    int exit_code_s = m_s->exit_code;
    exit_code_s +=1;

    int exit_code;
    if ((p->argv[1] != NULL) && atoi(p->argv[1]))
        exit_code = atoi(p->argv[1]);
    else
        exit_code = p->exit_code;
    //free all!!!
    exit(exit_code);
    return (exit_code);
}


int mx_fg(t_shell *m_s, t_process *p) {
    pid_t pgid = 0;
    int job_id = 0;

    if (p->argv[1]) {
        //  if (p->arg_command[0] == '%') {
        job_id = atoi(p->argv[1]);
//        else
//            job_id = mx_get_recent_job(m_s); //most recently placed in the background, find '+'
        //}
    }
    //       else {
//            pid = atoi((p->arg_command[1]));
    printf("job_id %d\n", job_id);
    pgid = mx_get_pgid_by_job_id(m_s, job_id);
    printf("pid suspended process %d\n", pgid);

    if (kill(-pgid, SIGCONT) < 0) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(pgid));
        mx_printerr("\n");
        return -1;
    }

    tcsetpgrp(0, pgid);
    if (job_id > 0) {
        mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
        mx_print_job_status(m_s, job_id);
        if (mx_wait_job(m_s, job_id) >= 0)
            mx_remove_job(m_s, job_id);
    } else
        mx_wait_pid(m_s, pgid);
    signal(SIGTTOU, SIG_IGN);  //Запись в управляющий терминал процессом из группы процессов фонового режима.
    tcsetpgrp(0, getpid());
    signal(SIGTTOU, SIG_DFL);  //
    return 0;
}


int mx_bg(t_shell *m_s, t_process *p) {
    pid_t pgid = 0;
    int job_id = 0;

    if (p->argv[1]) {
        //  if (p->arg_command[0] == '%') {
        job_id = atoi(p->argv[1]);
//        else
//            job_id = mx_get_recent_job(m_s); //most recently placed in the background, find '+'
        //}
    }
    //       else {
//            pid = atoi((p->arg_command[1]));
    printf("job_id %d\n", job_id);
    pgid = mx_get_pgid_by_job_id(m_s, job_id);
    printf("pid background process %d\n", pgid);
    if (kill(-pgid, SIGCONT) < 0) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(pgid));
        mx_printerr("\n");
        return -1;
    }
    if (job_id > 0) {
        mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
        mx_print_job_status(m_s, job_id);
    }
    return 0;
}


int mx_echo(t_shell *m_s, t_process *p) {
    int exit_code = 0;
    int exit_code_s = m_s->exit_code;
    exit_code_s += 1;

    if (!p->argv[1])
        mx_printstr("\n");
    else if (strcmp(p->argv[1], "-n") == 0) {
        if (p->argv[2]) {
            mx_printstr(p->argv[2]);
            mx_printstr("%");
            mx_printstr("\n");
        }
    }
    else {
        mx_printstr(p->argv[1]);
        mx_printstr("\n");
    }
    return exit_code;
}
