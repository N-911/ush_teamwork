#include "ush.h"

int mx_set_job_status(t_shell *m_s, int job_id, int status) {
    t_process *p;

    if (job_id > MX_JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_pr; p != NULL; p = p->next) {
        if (p->status != MX_STATUS_DONE)
            p->status = status;
    }
    return 0;
}

int mx_job_completed(t_shell *m_s, int job_id) {
    t_process *p;

    if (job_id > MX_JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_pr; p != NULL; p = p->next) {
        if (p->status != MX_STATUS_DONE && p->status != MX_STATUS_TERMINATED) {
            return 0;
        }
    }
    return 1;
}

int mx_job_is_running(t_shell *m_s, int job_id) {
    t_process *p;
    int status = 1;

    if (job_id > MX_JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_pr; p != NULL; p = p->next) {
        if (p->status != MX_STATUS_RUNNING)
            status = 0;
    }
    return status;
}

int mx_g_find_job(t_shell *m_s, char *arg) {
    int i;
    t_process *p;

    for (i = m_s->max_number_job; i > 0 ; i--) {
        if (m_s->jobs[i] == NULL)
            continue;
        for (p = m_s->jobs[i]->first_pr; p != NULL; p = p->next) {
            if ((mx_strcmp(p->argv[0], arg)) == 0)
                return i;
        }
    }
    return -1;
}

void mx_destroy_jobs(t_shell *m_s, int id) {
    t_process *p;

    for (p = m_s->jobs[id]->first_pr; p != NULL; p = p->next) {
        mx_del_strarr(&p->argv);
        mx_strdel(&p->command);
        mx_strdel(&p->input_path);
        mx_strdel(&p->output_path);
        mx_redir_clear_list(&p->redirect);
        free(p);
    }
    free(m_s->jobs[id]);
}
