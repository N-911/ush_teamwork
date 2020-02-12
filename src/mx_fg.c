#include "ush.h"
/*
Возобновляет работу задания в приоритетном режиме и делает это задание текущим. Если задание не указано,
используется текущее задание командного интерпретатора. Возвращается значение статуса выхода команды,
переведенной в приоритетный режим, или 1 если управление заданиями отключено или, при включенном
управлении заданиями, если указано несуществующее задание или задание, запущенное при отключенном
управлении заданиями.
    Строки %% и %+ обозначают текущее задание командного интерпретатора - последнее задание, остановленное
при работе в приоритетном режиме или запущенное в фоновом режиме. На предыдущее задание можно сослаться
с помощью строки %-. В результатах работы команд, связанных с управлением заданиями, (в частности, в
результатах выполнения команды jobs), текущее задание всегда помечается знаком +, а предыдущее - знаком -.
*/

static int get_number_job (t_shell  *m_s, char **args);
static int fg_send_signal (int pgid, int job_);

int mx_fg(t_shell *m_s, t_process *p) {
    int status;
    pid_t pgid = 0;
    int job_id = 0;

    mx_set_last_job(m_s);

    if ((job_id = get_number_job(m_s, p->argv)) == -1 || job_id == 0) {
        mx_printerr("fg: no current job\n");
        return -1;
    }

    job_id = get_number_job(m_s, p->argv);
//printf("job_id %d\n",  job_id);


    if ((pgid = mx_get_pgid_by_job_id(m_s, job_id)) < 1) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(job_id));
        mx_printerr("\n");
        return -1;
    }
    mx_fg_send_signal(job_i,)

    return status;
}

static int fg_send_signal (int pgid, int job_) {
    int status;

    printf("pid suspended process %d\n", pgid);
    if (kill(-pgid, SIGCONT) < 0) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(job_id));
        mx_printerr("\n");
        return -1;
    }
    tcsetpgrp(STDIN_FILENO, pgid);
    mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
    mx_print_job_status(m_s, job_id, 0);
    status = mx_wait_job(m_s, job_id);
    if (mx_job_completed(m_s, job_id))
        mx_remove_job(m_s, job_id);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, getpid());
    signal(SIGTTOU, SIG_DFL);
    tcgetattr(STDERR_FILENO, &m_s->jobs[job_id]->tmodes);
    tcsetattr(STDIN_FILENO, TCSADRAIN, &m_s->jobs[job_id]->tmodes);
    return status;
}

static int get_number_job (t_shell  *m_s, char **args) {
    int n_args = 0;
    int job_id = -1;
//    int pgid;

    for (int i = 0; args[i] != NULL; i++)
        n_args++;

    if (n_args > 2)
        mx_printerr("ush: fg: too many arguments\n");
    else if (n_args == 1) {
        job_id = m_s->jobs_stack->last;
    }
    else {
        if (args[1][0] == '%') {
            job_id = atoi(mx_strdup(args[1] + 1));
        }
        else
            job_id = mx_find_job_by_p_name(m_s, args[1]);
    }
    return job_id;
}

int mx_find_job_by_p_name(t_shell *m_s, char *arg) {
    int i;
    t_process *p;

    for (i = m_s->max_number_job; i > 0 ; i--) {
        if (m_s->jobs[i] == NULL)
            continue;
        for (p = m_s->jobs[i]->first_process; p != NULL; p = p->next) {
            if ((mx_strcmp(p->argv[0], arg)) == 0)
                return i;
            }
        }
    return -1;
}
