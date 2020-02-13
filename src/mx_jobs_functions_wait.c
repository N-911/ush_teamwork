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
        if (WIFEXITED(status))
            mx_set_process_status(m_s, pid, STATUS_DONE);
        else if (WIFSTOPPED(status))
            mx_set_process_status(m_s, pid, STATUS_SUSPENDED);
        else if (WIFCONTINUED(status)) {
            mx_set_process_status(m_s, pid, STATUS_CONTINUED);
        }
        job_id = mx_job_id_by_pid(m_s, pid);
        if (job_id > 0 && mx_job_completed(m_s, job_id)) {
            mx_print_job_status(m_s, job_id, 0);
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
    proc_count = mx_get_proc_count(m_s, job_id, FILTER_IN_PROGRESS);

//    printf("proc count %d\n ", proc_count);
    while (wait_count < proc_count) {
        wait_pid = waitpid(-m_s->jobs[job_id]->pgid, &status, WUNTRACED);
        wait_count++;
        if (WIFEXITED(status)) {
            mx_set_process_status(m_s, wait_pid, STATUS_DONE);
            //  mx_pop_from_stack(m_s, job_id);
        }
        else if (WIFSIGNALED(status))
            mx_set_process_status(m_s, wait_pid, STATUS_TERMINATED);
        else if (WSTOPSIG(status)) {
//            status = -1;  // !!!
            mx_set_process_status(m_s, wait_pid, STATUS_SUSPENDED);
            if (wait_count == proc_count) {
                mx_print_job_status(m_s, job_id, 0);
            }
        }
    }
    return status >> 8;
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


int mx_get_proc_count(t_shell *m_s, int job_id, int filter) {
    t_process *p;
    int count = 0;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL) {
        return -1;
    }
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (filter == FILTER_ALL ||
            (filter == FILTER_DONE && p->status == STATUS_DONE) ||
            (filter == FILTER_IN_PROGRESS && p->status != STATUS_DONE)) {
            count++;
        }
    }
    return count;
}

void mx_set_process_status(t_shell *m_s, int pid, int status) {
    int i;
    t_process *p;
    int job_id = mx_job_id_by_pid(m_s, pid);

    for (i = 1; i < m_s->max_number_job; i++) {
        if (m_s->jobs[i] == NULL) {
            continue;
        }
        for (p = m_s->jobs[i]->first_process; p != NULL; p = p->next) {
            if (p->pid == pid) {
                p->status = status;
                if (status == STATUS_SUSPENDED) {
                    if (m_s->jobs_stack->prev_last && m_s->jobs_stack->last)
                        m_s->jobs_stack->prev_last = m_s->jobs_stack->last;
                    m_s->jobs_stack->last = job_id;

                }
                break;
            }
        }
    }
//    mx_set_last_job(m_s);
}

