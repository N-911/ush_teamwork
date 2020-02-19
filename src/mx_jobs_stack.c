#include "ush.h"

// function writes number jobs in stack FIFO
void mx_init_jobs_stack(t_shell *m_s) {
    t_stack *st = malloc(sizeof(t_stack));

    st->top = -1;  // index of top element -> last_job
    st->last = -1;
    st->prev_last = -1;
    st->size = MX_JOBS_NUMBER;
    st->stack = malloc(sizeof(int) * MX_JOBS_NUMBER);
    for (int i = 0; i < st->size; i ++)
        st->stack[i] = 0;
    m_s->jobs_stack = st;
}

void mx_push_to_stack(t_shell *m_s, int job) {
    if (m_s->jobs_stack->top < m_s->jobs_stack->size)
        m_s->jobs_stack->stack[++m_s->jobs_stack->top] = job;
}

void mx_pop_from_stack(t_shell *m_s, int job) {
    int j = 0;
    int i;
    int size = m_s->jobs_stack->size;
    int *temp = malloc(sizeof(int) * size);

    for (i = 0; i < size; i++)
        temp[i] = m_s->jobs_stack->stack[i];
    for (i = 0; i < size; i ++)
        m_s->jobs_stack->stack[i] = 0;
    if (m_s->jobs_stack->top >= 0) {
        for (i = 0; j < size; i++, j++) {
            if (temp[i] == job)
                i++;
            m_s->jobs_stack->stack[j] = temp[i];
        }
    }
    m_s->jobs_stack->top--;
    free(temp);
}

// temp function only for debug
void mx_print_stack (t_shell *m_s) {
    int job_id;

    if (m_s->jobs_stack->top >= 0) {
        job_id = m_s->jobs_stack->stack[m_s->jobs_stack->top];
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

    if (job_id > MX_JOBS_NUMBER || m_s->jobs[job_id] == NULL)
        return -1;
    for (p = m_s->jobs[job_id]->first_process; p != NULL; p = p->next) {
        if (p->status == status)
            flag = 1;
    }
    return flag;
}

void mx_set_last_job(t_shell *m_s) {
    int size = m_s->jobs_stack->top;
    int last = -1;

    for (int i = size; i >= 0; i--) {
        if (mx_get_job_status(m_s, m_s->jobs_stack->stack[i], 2)) {
            last = m_s->jobs_stack->stack[i];
            break;
        }
    }
    if (last == -1) {
        for (int j = size; j >= 0; j--) {
            if ((mx_get_job_status(m_s, m_s->jobs_stack->stack[j], 0) > 0) &&
                m_s->jobs[m_s->jobs_stack->stack[j]]->foreground == 0) {
                last = m_s->jobs_stack->stack[j];
                break;
            }
        }
    }
    m_s->jobs_stack->last = last;
}
