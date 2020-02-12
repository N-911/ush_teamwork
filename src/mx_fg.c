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

int mx_fg(t_shell *m_s, t_process *p) {
    int status;
    pid_t pgid = 0;
    int job_id = 0;

    mx_set_last_job(m_s);
//    printf("last %d\n",  m_s->jobs_stack->last);
    if (p->argv[1]) {
        //  if (p->arg_command[0] == '%') {
        job_id = atoi(p->argv[1]);
    }
    else
        job_id = m_s->jobs_stack->last;

    if (job_id == -1 || job_id == 0) {
        mx_printerr("fg: no current job\n");
        return -1;
    }
//printf("job_id %d\n",  job_id);
    if ((pgid = mx_get_pgid_by_job_id(m_s, job_id)) < 1) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(job_id));
        mx_printerr("\n");
        return -1;
    }
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
