#include "ush.h"

static void print_spaces(int number);
/*
[1]    suspended  sleep 14
[2]    done       ls |
       suspended  sleep 5
[3]    suspended  sleep 15
[4]    suspended  sleep 16
[5]    suspended  sleep 16
[6]    suspended  sleep 17
[7]    suspended  sleep 18
[8]  - suspended  sleep 19
[9]  + suspended  sleep 20
*/
int mx_print_job_status(t_shell *m_s, int job_id) {
    t_process *p;
    int len;
    const char* status[] = { "running", "done", "suspended", "continued", "terminated" };

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    printf("[%d]  %c ", job_id, m_s->jobs[job_id]->mark_job_id);

    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        printf("%s", status[p->status]);
        len = mx_strlen(status[p->status]);
        print_spaces(MAX_LEN + 1 - len);
        printf("%s", p->command);

/*
        for (int i = 0; proc->argv[i] != NULL; i++) {
            printf("%s ", proc->argv[i]);
        }
*/
        (p->next != NULL) ? printf(" |\n ") : printf("\n");
    }
    return 0;
}

void mx_print_pid_process_in_job(t_shell *m_s, int job_id) {
    t_process *p;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL) {
        mx_printstr("error job_id");
        mx_printstr("\n");
    }
    else {
        printf("[%d]", job_id);
        for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
            printf(" %d", p->pid);
        }
        printf("\n");
    }
}

static void print_spaces(int number) {
    for (int i = 0; i < number; i++) {
        mx_printchar(' ');
    }
}
