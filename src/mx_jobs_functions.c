#include "ush.h"

int mx_job_completed(t_shell *m_s, int job_id) {
    t_process *p;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (p->status != STATUS_DONE) {
            return 0;
        }
    }
    return 1;
}

int mx_get_next_job_id(t_shell *m_s) {
    for (int i = 1; i < m_s->max_number_job + 1; i++) {
        if (m_s->jobs[i] == NULL)
            return i;
    }
    return -1;
}

int mx_insert_job(t_shell *m_s, t_job *job) {
    int id;

    if ((id = mx_get_next_job_id(m_s)) < 0)
        return -1;
    job->job_id = id;
    m_s->jobs[id] = job;
    mx_push_to_stack(m_s, id);
    m_s->max_number_job++;
    return id;
}

void mx_remove_job(t_shell *m_s, int job_id) {
    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return;
//    mx_destroy_jobs(m_s, id);
    if (job_id == m_s->max_number_job)
        m_s->max_number_job--;
    m_s->jobs[job_id] = NULL;
    mx_pop_from_stack(m_s, job_id);
}

void mx_destroy_jobs(t_shell *m_s, int id) {
    t_process *p;

    for (p = m_s->jobs[id]->first_process; p != NULL; p = p->next) {
        free(p->argv);
        free(p->arg_command);
        free(p->command);
    }
    m_s->jobs[id]->first_process = NULL;
    free(m_s->jobs[id]->command);
}


int mx_job_id_by_pid(t_shell *m_s, int pid) {
    int i;
    t_process *p;

    for (i = 1; i <= m_s->max_number_job + 1; i++) {
        if (m_s->jobs[i] != NULL) {
            for (p = m_s->jobs[i]->first_process; p != NULL; p = p->next) {
                if (p->pid == pid) {
                    return i;
                }
            }
        }
    }
    return -1;
}

int mx_get_pgid_by_job_id(t_shell *m_s, int job_id) {
    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    return m_s->jobs[job_id]->pgid;
}

int mx_set_job_status(t_shell *m_s, int job_id, int status) {
    t_process *p;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (p->status != STATUS_DONE)
            p->status = status;
    }
    return 0;
}

int mx_job_is_running(t_shell *m_s, int job_id) {
    t_process *p;
    int status = 1;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (p->status != STATUS_RUNNING)
            status = 0;
    }
    return status;
}


// not used
void mx_print_exit(int status) {
    if (WIFEXITED(status))
        printf("нормальное завершение, код выхода = %d\n",
               WEXITSTATUS(status));
    else if (WIFSIGNALED(status))
        printf("аварийное завершение, номер сигнала = %d%s\n",
               WTERMSIG(status),

#ifdef WCOREDUMP
               WCOREDUMP(status) ? " (создан файл core)" : "");
#else
        "");
#endif

    else if (WIFSTOPPED(status))
        printf("дочерний процесс остановлен, номер сигнала = %d\n", WSTOPSIG(status));
}
