#include "ush.h"

int mx_job_completed(t_shell *m_s, int id) {
    t_process *proc;

    if (id > JOBS_NUMBER || m_s->jobs[id] == NULL) {
        return 0;
    }
    for (proc = m_s->jobs[id]->first_process; proc != NULL; proc = proc->next) {
        if (proc->status != STATUS_DONE) {
            return 0;
        }
    }
    return 1;
}

int mx_get_next_job_id(t_shell *m_s) {
    for (int i = 1; i < m_s->max_number_job; i++) {
        if (m_s->jobs[i] == NULL) {
            m_s->max_number_job++;
            return i;
        }
    }
    return -1;
}

int mx_insert_job(t_shell *m_s, t_job *job) {
    int id = mx_get_next_job_id(m_s);

    if (id < 0) {
        return -1;
    }
    job->job_id = id;
    job->mark_job_id = 43;
    m_s->jobs[id] = job;
   // write(1, "job insert\n", strlen("job insert\n"));
    return id;
}

void mx_remove_job(t_shell *m_s, int id) {
    if (id > JOBS_NUMBER || m_s->jobs[id] == NULL)
        exit(-1);
//    mx_destroy_jobs(m_s, id);
    if (id == m_s->max_number_job - 1)
        m_s->max_number_job--;
    m_s->jobs[id] = NULL;
   // write(1, "job remove\n", strlen("job remove\n"));
}

void mx_destroy_jobs(t_shell *m_s, int id) {
    t_process *proc;

    for (proc = m_s->jobs[id]->first_process; proc != NULL; proc = proc->next) {
        free(proc->argv);
        free(proc->arg_command);
        free(proc->command);
    }
    m_s->jobs[id]->first_process = NULL;
    free(m_s->jobs[id]->command);
}

void mx_print_process_in_job(t_shell *m_s, int id) {
    t_process *p;

    if (id > JOBS_NUMBER || m_s->jobs[id] == NULL) {
        mx_printstr("error job_id");
        mx_printstr("\n");
    }
    else {
        printf("[%d]", id);
        for (p = m_s->jobs[id]->first_process; p != NULL; p = p->next) {
            printf(" %d", p->pid);
        }
        printf("\n");
    }
}

int mx_get_proc_count(t_shell *m_s, int job_id, int filter) {
    t_process *proc;
    int count = 0;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL) {
        return -1;
    }
    for (proc = m_s->jobs[job_id]->first_process; proc != NULL; proc = proc->next) {
        if (filter == PROC_FILTER_ALL ||
            (filter == PROC_FILTER_DONE && proc->status == STATUS_DONE) ||
            (filter == PROC_FILTER_REMAINING && proc->status != STATUS_DONE)) {
            count++;
        }
    }
    return count;
}


void mx_set_process_status(t_shell *m_s, int pid, int status) {
    int i;
    t_process *proc;

    for (i = 1; i < m_s->max_number_job; i++) {
        if (m_s->jobs[i] == NULL) {
            continue;
        }
        for (proc = m_s->jobs[i]->first_process; proc != NULL; proc = proc->next) {
            if (proc->pid == pid) {
                proc->status = status;
                break;
            }
        }
    }
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


//  [1] +	done	ls -la src
int mx_print_job_status(t_shell *m_s, int job_id) {
    t_process *proc;
    const char* status[] = { "running", "done", "suspended", "continued", "terminated" };

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    printf("[%d]  %c ", job_id, m_s->jobs[job_id]->mark_job_id);
    for (proc = m_s->jobs[job_id]->first_process; proc != NULL; proc = proc->next) {
        printf("%s  ", status[proc->status]);
        printf("%s", proc->command);

/*
        for (int i = 0; proc->argv[i] != NULL; i++) {
            printf("%s ", proc->argv[i]);
        }
*/
        (proc->next != NULL) ? printf(" | ") : printf("\n");
    }
    return 0;
}


int mx_get_job_id_by_pid(t_shell *m_s, int pid) {
    int i;
    t_process *proc;

    for (i = 1; i <= m_s->max_number_job; i++) {
        if (m_s->jobs[i] != NULL) {
            for (proc = m_s->jobs[i]->first_process; proc != NULL; proc = proc->next) {
                if (proc->pid == pid) {
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

