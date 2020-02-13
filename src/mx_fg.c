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

static int fg_send_signal(t_shell *m_s, int pgid, int job_id);
static int fg_check(t_shell *m_s, t_process *p);
static void print_error_fg(char *arg1, char *arg2, char *arg3);
static int fg_get_job_id (t_shell *m_s, t_process *p);

int mx_fg(t_shell *m_s, t_process *p) {
    int status;
    pid_t pgid = 0;
    int job_id = 0;

    mx_set_last_job(m_s);
    if ((job_id = fg_get_job_id(m_s, p)) < 1)
        return -1;
    if ((pgid = mx_get_pgid_by_job_id(m_s, job_id)) < 1) {
        print_error_fg("fg: ", p->argv[1],": no such job\n");
        return -1;
    }
    status = fg_send_signal(m_s, pgid, job_id);
    return status;
}

static int fg_get_job_id (t_shell *m_s, t_process *p) {
    int job_id;
    int n_args = 0;

    for (int i = 0; p->argv[i] != NULL; i++)
        n_args++;
    if (n_args > 2) {
        mx_printerr("ush: fg: too many arguments\n");
        return -1;
    } else if (n_args == 1) {
        if ((job_id = m_s->jobs_stack->last) < 1) {
            mx_printerr("fg: no current job\n");
            return -1;
        }
    }
    else {
        if ((job_id = fg_check(m_s, p)) < 1)
            return -1;
    }
    return job_id;
}

static int fg_check (t_shell *m_s, t_process *p) {
    int job_id;

    if (p->argv[1][0] == '%' && isdigit(p->argv[1][1])) {
        if ((job_id = atoi(mx_strdup(p->argv[1] + 1))) < 1) {
            print_error_fg("fg: ", p->argv[1],": no such job\n");
            return -1;
        }
    }
    else if (p->argv[1][0] == '%' && !isdigit(p->argv[1][1])) {
        if ((job_id = mx_find_job_by_p_name(m_s, (p->argv[1] + 1))) < 1) {
            print_error_fg("fg: job not found: ", (p->argv[1] + 1),"\n");
            return -1;
        }
    }
    else {
        if ((job_id = mx_find_job_by_p_name(m_s, p->argv[1])) < 1) {
            print_error_fg("fg: job not found: ", p->argv[1],"\n");
            return -1;
        }
    }
    return job_id;
}

static int fg_send_signal(t_shell *m_s, int pgid, int job_id) {
    int status;

//    printf("pid suspended process %d\n", pgid);
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
    return status >> 8;
}

static void print_error_fg(char *arg1, char *arg2, char *arg3) {
    mx_printerr(arg1);
    mx_printerr(arg2);
    mx_printerr(arg3);
}

