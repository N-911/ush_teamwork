#include "ush.h"

//  [1] +	done	ls -la src
int mx_print_job_status(t_shell *m_s, int job_id) {
    t_process *p;
    const char* status[] = { "running", "done", "suspended", "continued", "terminated" };

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    printf("[%d]  %c ", job_id, m_s->jobs[job_id]->mark_job_id);

    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
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

void mx_print_process_in_job(t_shell *m_s, int job_id) {
    t_process *p;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL) {
        mx_printstr("error job_id");
        mx_printstr("\n");
    }
    else {
        printf("[%d]", id);
        for (p = m_s->jobs[jobs_id]->first_process; p != NULL; p = p->next) {
            printf(" %d", p->pid);
        }
        printf("\n");
    }
}
