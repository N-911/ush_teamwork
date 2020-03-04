#include "ush.h"

static t_process *init_process(t_ast *list) {
    t_process *p = (t_process *)malloc(sizeof(t_process));

    if (!p)
        return NULL;
    p->argv = mx_strdup_arr(list->args);
    p->delim = list->type;
    p->command = mx_strdup(list->args[0]);
    p->input_path = NULL;
    p->output_path = NULL;
    p->redir_delim = 0;
    p->redirect = NULL;
    p->foregrd = 1;
    p->pipe = 0;
    if (p->delim == FON)
        p->foregrd = 0;
    else if (p->delim == PIPE)
        p->pipe = 1;
    p->next = NULL;
    return p;
}

static t_process *create_process(t_shell *m_s, t_ast *list) {
    t_process *p;
    t_ast *tmp;
    int index = 0;

    if (!(p = init_process(list)))
        return NULL;
    if (list->left) {
        tmp = list->left;
        p->redir_delim = tmp->type;
        if (MX_IS_REDIR_INP(tmp->type))
            p->input_path = mx_strdup(tmp->args[0]);
        else if (MX_IS_REDIR_OUTP(tmp->type))
            p->output_path = mx_strdup(tmp->args[0]);
    }
    if (list->left)
        for (t_ast *q = list->left; q; q = q->next)
            mx_redir_push_back(&p->redirect, q->args[0], q->type);
    if ((index = mx_builtin_commands_idex(m_s, p->argv[0])) == -1)
        p->type = -1;
    else
        p->type = index;
    return p;
}

void mx_push_process_back(t_process **process,
                                 t_shell *m_s, t_ast *list) {
    t_process *tmp;
    t_process *p;

    if (!process || !m_s || !list)
        return;
    tmp = create_process(m_s, list);
    if (!tmp)
        return;
    p = *process;
    if (*process == NULL) {
        *process = tmp;
        return;
    }
    else {
        while (p->next != NULL)
            p = p->next;
        p->next = tmp;
    }
}

void mx_clear_process(t_process *p) {
    mx_del_strarr(&p->argv);
    mx_strdel(&p->command);
    mx_strdel(&p->input_path);
    mx_strdel(&p->output_path);
    mx_redir_clear_list(&p->redirect);
    free(p);
}

t_job *mx_create_job(t_shell *m_s, t_ast *list) {
    t_job *new_job;
    t_process *first_p = NULL;

    for (t_ast *l = list; l; l = l->next) {
        if ((l->args = mx_filters(l->token, m_s)) && *(l->args))
            mx_push_process_back(&first_p, m_s, l);
        else {
            mx_clear_process(first_p);
            return NULL;
        }
    }
    new_job = (t_job *) malloc(sizeof(t_job));
    new_job->first_pr = first_p;
    new_job->foregrd = MX_FOREGROUND;
    for (; first_p != NULL; first_p = first_p->next)
        if (!first_p->foregrd)
            new_job->foregrd = MX_BACKGROUND;
    new_job->job_id = -1;
    new_job->pgid = 0;
    new_job->stdin = STDIN_FILENO;
    new_job->stdout = STDOUT_FILENO;
    new_job->stderr = STDERR_FILENO;
    new_job->infile = new_job->stdin;
    new_job->outfile = 1;
    new_job->errfile = 2;
    return new_job;
}
