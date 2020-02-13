#include "ush.h"

int mx_bg(t_shell *m_s, t_process *p) {
    int exit_code = 0;
    pid_t pgid = 0;
    int job_id = 0;

    mx_set_last_job(m_s);
    if (p->argv[1]) {
        //  if (p->arg_command[0] == '%') {
        job_id = atoi(p->argv[1]);
    }
    else
        job_id = m_s->jobs_stack->last;
    // printf("job_id %d\n", job_id);
    pgid = mx_get_pgid_by_job_id(m_s, job_id);
    // printf("pid background process %d\n", pgid);



    if (kill(-pgid, SIGCONT) < 0) {
        mx_printerr("bg: job not found: ");
        mx_printerr(mx_itoa(pgid));
        mx_printerr("\n");
        return -1;
    }
        mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
        mx_print_job_status(m_s, job_id, 0);
    return 0;
}
