#include "ush.h"
static int count_args(char **args, int n_options);
static void fill_options(int n_options, t_jobs *jobs_options, char **args);

int mx_builtin_commands_idex(t_shell *m_s, char *command) {
    int i = 0;

    for (i = 0; m_s->builtin_list[i] != NULL; i++) {
        if (strcmp(command, m_s->builtin_list[i]) == 0)
            return (i);
    }
    return (-1);
}

/*
jobs - List background processes
[2]    done       ls |
       suspended  sleep 5
[3]    suspended  sleep 15

-l   list process IDs in addition to the normal information.
-p  Pass the -p option to jobs command to display PIDs only:  7895 7896
-r  Display only running jobs.
-s  Display only stopped jobs
[3]    10354 suspended  sleep 15
// jobs  list the active jobs [3]    suspended  sleep 15
// jobs -l                    [1]  + 24549 suspended  sleep 11


e2r13p9% jobs -z
jobs: bad option: -z

*/

int mx_jobs(t_shell *m_s, t_process *p) {
    t_jobs jobs_options = {0, 0, 0};
    int n_options = mx_count_options(p->argv, "lprs", "jobs", " [-lrs]");
    int n_args = count_args(p->argv, n_options);
    int exit_code = 1;

    mx_set_last_job(m_s);
    printf ("n_opptions  %d\n", n_options);
    printf("n_args  %d\n", n_args);
    fill_options(n_options, &jobs_options, p->argv);
    printf("jobs in \n");
    if (n_options >= 0 && n_args < 2) {
        if (n_args == 0 && !n_options) {
            for (int i = 0; i < JOBS_NUMBER; i++) {
                if (m_s->jobs[i] != NULL)
                    mx_print_job_status(m_s, i, 0);
            }
        }
        else if (jobs_options.l) {
            if (jobs_options.r) {
                for (int i = 0; i < JOBS_NUMBER; i++)
                    if (m_s->jobs[i] != NULL && mx_job_is_running(m_s, i))
                        mx_print_job_status(m_s, i, 1);
            } else if (jobs_options.s) {
                for (int i = 0; i < JOBS_NUMBER; i++)
                    if (m_s->jobs[i] != NULL && !mx_job_is_running(m_s, i))
                        mx_print_job_status(m_s, i, 1);
            } else {
                for (int i = 0; i < JOBS_NUMBER; i++)
                    if (m_s->jobs[i] != NULL)
                        mx_print_job_status(m_s, i, 1);
            }
        }
        else if (jobs_options.r && !jobs_options.s) {
            for (int i = 0; i < JOBS_NUMBER; i++)
                if (m_s->jobs[i] != NULL && mx_job_is_running(m_s, i))
                    mx_print_job_status(m_s, i, 1);
        }
        else if (jobs_options.s && !jobs_options.r) {
            for (int i = 0; i < JOBS_NUMBER; i++)
                if (m_s->jobs[i] != NULL && !mx_job_is_running(m_s, i))
                    mx_print_job_status(m_s, i, 1);
        }
        else {
            for (int i = 0; i < JOBS_NUMBER; i++)
                if (m_s->jobs[i] != NULL)
                    mx_print_job_status(m_s, i, 0);
        }
    }
    p->exit_code = 0;
//    mx_print_stack(m_s);
    return exit_code;
}

/*
fg [задание]

<<<<<<< HEAD
    if(p->argv[1] != NULL && p->argv[2] != NULL) {
        mx_printerr("ush: exit: too many arguments\n");
        return 1;
    }
    if (p->argv[1] != NULL) {
        if (p->argv[1][0] == '+' || p->argv[1][0] == '-') {
            if (p->argv[1][1] == '\0')
                flag++;
            start++;
        }
        for (int i = start; i < mx_strlen(p->argv[1]); i++) {
            if(!mx_isdigit(p->argv[1][i])) {
                flag++;
                break;
            }
        }
        if (!flag) {
            exit_code = atoi(p->argv[1]);
        }
        else {
            mx_printerr("ush: exit: ");
            mx_printerr(p->argv[1]);
            mx_printerr(": numeric argument required\n");
            exit_code = 255;
        }
    }
    //system ("leaks -q ush");
    exit(exit_code);
}
=======
Возобновляет работу задания в приоритетном режиме и делает это задание текущим. Если задание не указано,
используется текущее задание командного интерпретатора. Возвращается значение статуса выхода команды,
переведенной в приоритетный режим, или 1 если управление заданиями отключено или, при включенном
управлении заданиями, если указано несуществующее задание или задание, запущенное при отключенном
управлении заданиями.
>>>>>>> master

Строки %% и %+ обозначают текущее задание командного интерпретатора - последнее задание, остановленное
при работе в приоритетном режиме или запущенное в фоновом режиме. На предыдущее задание можно сослаться
с помощью строки %-. В результатах работы команд, связанных с управлением заданиями, (в частности, в
результатах выполнения команды jobs), текущее задание всегда помечается знаком +, а предыдущее - знаком -.
*/

int mx_fg(t_shell *m_s, t_process *p) {
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


    mx_print_color(MAG, "child\t");
    mx_print_color(MAG, "m_s->jobs[job_id]->pgid ");
    mx_print_color(MAG, mx_itoa(m_s->jobs[job_id]->pgid));
    mx_printstr("\n");
    printf("job_id %d\n", job_id);
    pgid = mx_get_pgid_by_job_id(m_s, job_id);
    printf("pid suspended process %d\n", pgid);
    tcsetpgrp (STDIN_FILENO, pgid);
    tcsetattr (STDIN_FILENO, TCSADRAIN, &m_s->jobs[job_id]->tmodes);
    if (kill(- pgid, SIGCONT) < 0) {
        mx_printerr("fg: job not found: ");
        mx_printerr(mx_itoa(pgid));
        mx_printerr("\n");
        return -1;
    }
    if (job_id > 0) {
        mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
        mx_print_job_status(m_s, job_id, 0);
        if (mx_wait_job(m_s, job_id) >= 0)
            mx_remove_job(m_s, job_id);
    }
//    else
//        mx_wait_pid(m_s, pgid);
//    signal(SIGTTOU, SIG_IGN);  //Запись в управляющий терминал процессом из группы процессов фонового режима.
//    tcsetpgrp(0, getpid());
//    signal(SIGTTOU, SIG_DFL);  //
    return exit_code;
}


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
    if (job_id > 0) {
        mx_set_job_status(m_s, job_id, STATUS_CONTINUED);
        mx_print_job_status(m_s, job_id, 0);
    }
    return exit_code;
}

static int count_args(char **args, int n_options) {
    int n_args = 0;

    for (int i = n_options + 1; args[i] != NULL; i++) {
        n_args++;
    }
//    if (n_args > 1)
//        mx_printerr("ush: jobs: bad option:\n");
    return n_args;
}

static void fill_options(int n_options, t_jobs *jobs_options, char **args) {
    for (int i = n_options; i > 0; i--) {
        for (int j = mx_strlen(args[i]); j > 0; j--) {
            if (args[i][j] == 'l')
                jobs_options->l = 1;
            if (args[i][j] == 'r')
                jobs_options->r = 1;
            if (args[i][j] == 's')
                jobs_options->s = 1;
        }
    }
}
