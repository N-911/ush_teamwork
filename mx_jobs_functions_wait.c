#include "ush.h"

/*
Значение pid -1 или WAIT_ANY информация состояния для любого дочернего процесса
значение pid 0 или WAIT_MYPGRP запрашивает информацию для любого дочернего процесса
в той же самой группе процесса как вызывающий процесс
значение -pgid запрашивает информацию для любого дочернего процесса, чей ID группы - pgid.
 */

void mx_check_jobs(t_shell *m_s) {
    int status;
    pid_t pid;  // waitpid return pid child process
    int job_id;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        mx_printstr("pid=");
        mx_printint(pid);
        mx_printstr("\n");
        if (WIFEXITED(status))
            mx_set_process_status(m_s, pid, STATUS_DONE);
        else if (WIFSTOPPED(status))
            mx_set_process_status(m_s, pid, STATUS_SUSPENDED);
        else if (WIFCONTINUED(status)) {
            mx_set_process_status(m_s, pid, STATUS_CONTINUED);
        }
        job_id = mx_get_job_id_by_pid(m_s, pid);
        if (job_id > 0 && mx_job_completed(m_s, job_id)) {
            mx_print_job_status(m_s, job_id);
            mx_remove_job(m_s, job_id);
        }
    }
}

/*
   WUNTRACED флаг, чтобы запросить информацию состояния остановленных процессов также как процессов, которые завершились
 */
int mx_wait_job(t_shell *m_s, int job_id) {
    int proc_count;
    int wait_pid = -1;
    int wait_count = 0;
    int status = 0;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;

    proc_count = mx_get_proc_count(m_s, job_id, PROC_FILTER_REMAINING);

    do {
        wait_pid = waitpid(-m_s->jobs[job_id]->pgid, &status, WUNTRACED);
        wait_count++;
        //   printf("wait_pid = %d\n", wait_pid);
        if (WIFEXITED(status))
            mx_set_process_status(m_s, wait_pid, STATUS_DONE);
        else if (WIFSIGNALED(status))
            mx_set_process_status(m_s, wait_pid, STATUS_TERMINATED);
        else if (WSTOPSIG(status)) {
            status = -1;
            mx_set_process_status(m_s, wait_pid, STATUS_SUSPENDED);
            if (wait_count == proc_count) {
                mx_print_job_status(m_s, job_id);
            }
        }
    } while (wait_count < proc_count);
    return status;
}

int mx_wait_pid(t_shell *m_s, int pid) {
    int status = 0;

    waitpid(pid, &status, WUNTRACED);
    if (WIFEXITED(status)) {
        mx_set_process_status(m_s, pid, STATUS_DONE);
    }
    else if (WIFSIGNALED(status)) {
        mx_set_process_status(m_s, pid, STATUS_TERMINATED);
    }
    else if (WSTOPSIG(status)) {
        status = -1;
        mx_set_process_status(m_s, pid, STATUS_SUSPENDED);
    }
    return status;
}