#include "ush.h"

static t_process *init_process(void) {
    t_process *p = (t_process *)malloc(sizeof(t_process));

    if (!p)
        return NULL;
    p->input_path = NULL;
    p->output_path = NULL;
    p->redir_delim = 0;
    p->redirect = NULL;
    p->foreground = 1;
    p->pipe = 0;
    p->next = NULL;
    return p;
}

static t_process *create_process(t_shell *m_s, t_ast *list) {
    t_process *p;
    t_ast *tmp;
    int index = 0;

    p = init_process();
    if (!p)
        return NULL;
    p->argv = mx_strdup_arr(list->args);
    p->delim = list->type;
    p->command = mx_strdup(list->args[0]);
    // old redir
    if (list->left) {
        tmp = list->left;
        p->redir_delim = tmp->type;
        if (MX_IS_REDIR_INP(tmp->type))
            p->input_path = mx_strdup(tmp->args[0]);
        else if (MX_IS_REDIR_OUTP(tmp->type))
            p->output_path = mx_strdup(tmp->args[0]);
    }
    // new redirections
    if (list->left) {
        for (t_ast *q = list->left; q; q = q->next)
            mx_redir_push_back(&p->redirect, q->args[0], q->type);
    }
    //
    if (p->delim == FON)
        p->foreground = 0;
    if (p->delim == PIPE)
        p->pipe = 1;
    if ((index = mx_builtin_commands_idex(m_s, p->argv[0])) == -1) {
        p->type = -1;      //COMMAND_BUILTIN = index;   default = -1
    }
    else
        p->type = index;
    return p;
}

static void push_process_back(t_process **process, t_shell *m_s, t_ast *list) {
    t_process *tmp;
    t_process *p;

    if (!process || !m_s || !list)
        return;
    tmp = create_process(m_s, list); // create new
    if (!tmp)
        return;
    p = *process;
    if (*process == NULL) { // find Null-node
        *process = tmp;
        return;
    } else {
        while (p->next != NULL) // find Null-node
            p = p->next;
        p->next = tmp;
    }
}

t_process *mx_create_list_process(t_shell *m_s, t_ast *list) {
    t_ast *l;
    t_process *first_p = NULL;

    for (l = list; l; l = l->next) {
        push_process_back(&first_p, m_s, l);
    }
    return first_p;
}

t_job *mx_create_job(t_shell *m_s, t_ast *list) {
    t_job *new_job = (t_job *) malloc(sizeof(t_job));
    t_process *first_p = mx_create_list_process(m_s, list);
    t_process *p;

    new_job->first_process = first_p;
    new_job->foreground = FOREGROUND;
//    if (!new_job->first_process->foreground)
//        new_job->foreground = BACKGROUND;

    for (p = first_p; p != NULL; p = p->next) {
        if (!p->foreground)
            new_job->foreground = BACKGROUND;
    }
    new_job->job_id = -1;
    new_job->pgid = 0;
    new_job->stdin = STDIN_FILENO;
    new_job->stdout = STDOUT_FILENO;
    new_job->stderr = STDERR_FILENO;
    return new_job;
}


/*
t_job *mx_create_job(t_shell *m_s, t_input *list) {
    extern char **environ;
    int index = 0;
    // t_input *l;
    t_job *new_job = NULL;
    new_job = (t_job *) malloc(sizeof(t_job));
    t_process *first_p = NULL;
    // for (l = list; l; l = l->next) {
    first_p = (t_process *) malloc(sizeof(t_process));
    first_p->argv = list->args;
    first_p->foreground = 1;
    // first_p->path = getenv("PATH");
    // first_p->env = environ;
    for (int i = 0; first_p->argv[i] != NULL; i++) {
        if (strcmp(first_p->argv[i], "&") == 0)
            first_p->foreground = 0;
    }
    first_p->pipe = 0;
    if ((index = mx_builtin_commands_idex(m_s, first_p->argv[0])) == -1) {
        first_p->type = 0;      //COMMAND_BUILTIN = index;   default = 0
    } else
        first_p->type = index;
    first_p->next = NULL;
    // }
    new_job->first_process = first_p;
    if (first_p->foreground)
        new_job->foreground = FOREGROUND;
    else
        new_job->foreground = BACKGROUND;
    //  new_job->pgid = getpid();
    new_job->pgid = 0;
    new_job->stdin = 0;
    new_job->stdout = 1;
    new_job->stderr = 2;
    return new_job;
}
*/
