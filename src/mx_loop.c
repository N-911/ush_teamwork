#include "ush.h"
//static void print_list(t_input *parsed_line);

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_input *parsed_line = NULL;
//    int status = 1;
    while (1) {
        printf("%s%s%s", GRN, "u$h> ", RESET);
        // if (!mx_ush_read_line())
        //      exit(0);
        line = mx_ush_read_line();
        if (line[0] == '\n') {
           // mx_check_jobs(m_s);
            continue;
        } else if (line[0] != '\n') {
            t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
            parsed_line = mx_ush_parsed_line(line);
            //print_list(parsed_line);
            new_job = mx_create_job(m_s, parsed_line);
            mx_printstr("type - ");
            mx_printint(new_job->first_process->type);
            mx_printstr("\n");
            mx_printstr("foreground - ");
            mx_printint(new_job->first_process->foreground);
            mx_printstr("\n");
            mx_launch_job(m_s, new_job);
            //m_s->exit_code = status;
            //destroy_job;
            m_s->history_count = 0;
            //  mx_add_history(m_s, new_job);
//        termios_restore(m_s);
            mx_ush_clear_list(&parsed_line);  // clear leeks
            mx_strdel(&line);
        }
    }
}
/*
static void print_list(t_input *parsed_line) {
    if(!parsed_line)
        return;
    for (t_input *q = parsed_line; q; q = q->next) {
        mx_print_strarr_in_line(q->args, " ");
        mx_printstr(q->delim);
        mx_printstr("\n");
    }
}
*/

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

