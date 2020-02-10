#include "ush.h"

// function writes number jobs in stack FIFO
void mx_init_jobs_stack(t_shell *m_s) {
    t_stack *st = malloc(sizeof(t_stack));

    st->top = -1;  // index of top element -> last_job
    st->last = -1;
    st->prev_last = -1;
    st->size = JOBS_NUMBER;
    st->stack = malloc(sizeof(int) * JOBS_NUMBER);
    for (int i = 0; i < st->size; i ++)
        st->stack[i] = 0;
    m_s->jobs_stack = st;
}

void mx_push_to_stack(t_shell *m_s, int job) {
    if (m_s->jobs_stack->top < m_s->jobs_stack->size)
        m_s->jobs_stack->stack[++m_s->jobs_stack->top] = job;
  //  if (m_s->jobs[job]->foreground == 0)  // if job in foreground - set ir last job +
  //      m_s->jobs_stack->last = job;
    //printf ("top after push %d\n", m_s->jobs_stack->top);
}

void mx_pop_from_stack(t_shell *m_s, int job) {
    int j = 0;
    int size = m_s->jobs_stack->size;
    int *temp = malloc(sizeof(int) * size);

    for (int i = 0; i < size; i++)
        temp[i] = m_s->jobs_stack->stack[i];
    if (m_s->jobs_stack->top >= 0) {
        for (int i = 0; j < size; i++, j++) {
            if (temp[i] == job)
                i++;
            m_s->jobs_stack->stack[j] = temp[i];
        }
    }
    m_s->jobs_stack->top--;
    free(temp);
    //printf ("top after pop %d\n", m_s->jobs_stack->top);
}

// temp function only for debug
void mx_print_stack (t_shell *m_s) {
    int job_id;

    if (m_s->jobs_stack->top >= 0) {
        job_id = m_s->jobs_stack->stack[m_s->jobs_stack->top];
        printf("job_id->top =  %d\n", job_id);
        for (int i = 0; i < m_s->jobs_stack->size; i++)
            printf("%d   ", m_s->jobs_stack->stack[i]);
        printf("\n");
    }
}

bool mx_get_from_stack(t_shell *m_s, int job_id) {
    if (m_s->jobs_stack->top >= 0) {
        if (job_id == m_s->jobs_stack->stack[m_s->jobs_stack->top])
            return true;
    }
    return false;
}

int mx_get_job_status(t_shell *m_s, int job_id, int status) {
    t_process *p;
    int flag = 0;

    if (job_id > JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (p->status == status)
            flag = 0;
    }
    return flag;
}

void mx_set_last_job(t_shell *m_s) {
    int size = m_s->max_number_job;
    int last = 0;
//    int prev_last = 0;

    for (int i = size; i > 0; i--) {
        if (mx_get_job_status(m_s, i, STATUS_SUSPENDED))
            last = i;
    }
    if (!last) {
        for (int i = size; i > 0; i--) {
            if (mx_get_job_status(m_s, i, STATUS_RUNNING))
                last = i;
        }
    }
    m_s->jobs_stack->last = last;
}

