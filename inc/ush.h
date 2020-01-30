#ifndef USH_H
#define USH_H
#include <sys/types.h>
#include <pwd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h> /* определения типов */ 
#include <sys/ioctl.h>
#include <sys/stat.h> /* структура, возвращаемая stat */ 
#include <grp.h>
#include <sys/acl.h>
#include <sys/xattr.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/param.h>     //   const MAXPATHLEN      PATH_MAX
#include <termios.h>
#include <signal.h>
#include <term.h>
#include <curses.h>
#include <malloc/malloc.h>

//#include "../libmx/inc/libmx.h"
#include "libmx/inc/libmx.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
#define USH_TOK_BUFSIZE 64
#define USH_TOK_DELIM " \t\r\n\a"
#define PARSE_DELIM ";|&><"

//  EXIT
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

//  JOBS
#define JOBS_NUMBER 20
#define STATUS_RUNNING 0
#define STATUS_DONE 1
#define STATUS_SUSPENDED 2
#define STATUS_CONTINUED 3
#define STATUS_TERMINATED 4
#define FILTER_ALL 0
#define FILTER_DONE 1
#define FILTER_IN_PROGRESS 2
#define FOREGROUND 1
#define BACKGROUND 0
#define MAX_LEN 10

//  JOB STATUS
#define STATUS_RUN "running"
#define STATUS_DON "done"
#define STATUS_SUS "suspended"
#define STATUS_CON "continued"
#define STATUS_TER "terminated"

//      COLORS
#define BLK   "\x1B[30m"
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define RED_B "\x1B[1;31m"
#define RESET_B "\x1B[1;31m"
#define BLK_F_RED_B "\x1B[0;30;41m"
#define BLK_F_CYAN_B "\x1B[0;30;46m"
#define BLOCK "\x1B[0;34;46m"
#define CHR "\x1B[0;34;43m"
#define DIR_T "\x1B[0;30;42m"
#define DIR_X "\033[0;30;43m"

typedef struct s_input {
    char **args;
    char *delim;
    struct s_input *next;
} t_input;

typedef struct cd_s  {
    int s;
    int L;
    int P;
} cd_t;

typedef struct pwd_s  {
    int L;
    int P;
} pwd_t;

typedef struct echo_s  {
    int n;
    int e;
    int E;
} echo_t;

typedef struct which_s  {
    int s;
    int a;
} which_t;

typedef struct env_s  {
    int i;
    int u;
    int P;
} env_t;

typedef struct  s_export {
    char *name;
    char *value;
    struct s_export *next;
} t_export;

typedef struct s_env_builtin  {
    env_t env_options;
    int n_options;
    int n_variables;
    int n_args;
    t_export *env_list;
    t_export *env_params;
    char *path;
} t_env_builtin;

typedef struct s_process {
    char *fullpath;  //for execve
    char **argv;
    // char **envp;
    char *command;
    char *arg_command;
    pid_t pid;
    int exit_code;
    char *path;
    char **env;
    int status;  //status RUNNING DONE SUSPENDED CONTINUED TERMINATED
    int foreground;
    int pipe;
    char *delim;
    int fd_in;
    int fd_out;
    int type;              // COMMAND_BUILTIN = index in m_s->builtin_list; default = 0
    //  char completed;        // true if process has completed
    //  char stopped;          // true if process has stopped
    struct s_process *next;     // next process in pipeline
//    t_ast			*ast;  //ast tree
    pid_t pgid;
    int infile;
    int outfile;
    int errfile;
} t_process;

// A job is a pipeline of processes.
typedef struct s_job {
    int job_id;                 //number in jobs control
    int mark_job_id;            // " ", "-", "+"   "+" - last added job, "-" - prev added job;
    char *command;              // command line, used for messages
    t_process *first_process;     // list of processes in this job
    pid_t pgid;                 // process group ID
    int exit_code;
    int foreground;                  // foreground = 1 or background execution = 0
    char notified;              // true if user told about stopped job
    struct termios tmodes;      // saved terminal modes/
    int stdin;  // standard i/o channels
    int stdout;  // standard i/o channels
    int stderr;  // standard i/o channels
    struct s_job *next;  //next job separated by ";" "&&" "||"
} t_job;

typedef struct s_shell {
    int     argc;
    char    **argv;
    char	**envp;  //not used
    int		exit_code;  //return if exit
    t_job   *jobs[JOBS_NUMBER];     //arr jobs
    int max_number_job;  // number of added jobs + 1
    char **builtin_list;  // buildin functions
    char **history;
    int history_count;
    int history_index;
    struct termios t_original;
    struct termios t_custom;
    bool custom_terminal;
    pid_t shell_pgid;
    char *pwd;
    t_export *exported;
    t_export *variables;
} t_shell;

static volatile sig_atomic_t sigflag; // устанавливается обработчиком  в ненулевое значение
static sigset_t newmask, oldmask, zeromask;

t_shell *mx_init_shell(int argc, char **argv);

//      TERMINAL
void mx_terminal_init(t_shell *m_s);
void mx_termios_save(t_shell *m_s);
void termios_restore(t_shell *m_s);

//      PARSE
bool mx_check_parce_errors(char *line);
char *mx_ush_read_line(void);
char *mx_strtok(char *s, const char *delim);
char **mx_parce_tokens(char *line);
void mx_ush_push_back(t_input **list, char **arg, char *d);
void mx_ush_clear_list(t_input **list);
t_input *mx_ush_parsed_line(char *line);
/*
*  ---------------------------------------------- quote managing
*/
int mx_get_char_index_quote(const char *str, char *c);
int mx_count_chr_quote(const char *str, char *c);
char *mx_strtrim_quote(const char *s);
char **mx_strsplit_quote(const char *s, char *c);
/*
*  ---------------------------------------------- move to LIBMX
*/
char *mx_strjoin_free(char *s1, char const *s2);
int mx_strlen_arr(char **s);
char **mx_strdup_arr(char **str);
void mx_print_strarr_in_line(char **res, const char *delim);
void mx_printerr_red(char *c);
void mx_print_color(char *macros, char *str);
void mx_set_buff_zero(void *s, size_t n);

//      LOOP
//char *mx_read_line2(void);  // delete
char **mx_ush_split_line(char *line);  // delete
t_job *mx_create_job(t_shell *m_s, t_input *list);
void mx_ush_loop(t_shell *m_s);
int mx_launch_process(t_shell *m_s, t_process *p, int job_id, char *path, char **env,
                      int infile, int outfile, int errfile);
int mx_builtin_commands_idex(t_shell *m_s, char *command);
void mx_launch_job(t_shell *m_s, t_job *job);

//      BUILTIN COMMANDS
int mx_env(t_shell *m_s, t_process *p);
int mx_echo(t_shell *m_s, t_process *p);
int mx_jobs(t_shell *m_s, t_process *p);
int mx_fg(t_shell *m_s, t_process *p);
int mx_bg(t_shell *m_s, t_process *p);
int mx_cd(t_shell *m_s, t_process *p);
int mx_pwd(t_shell *m_s, t_process *p);
int mx_export(t_shell *m_s, t_process *p);
int mx_unset(t_shell *m_s, t_process *p);
int mx_which(t_shell *m_s, t_process *p);
int mx_exit(t_shell *m_s, t_process *p);


//      SIGNALS
void sigchld_handler(int signum);
//void mx_sig_handler(int signal);
//void sig_usr(int signo);
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void WAIT_PARENT(void);
void TELL_CHILD(pid_t pid);
void WAIT_CHILD(void);


//      JOBS
int mx_get_next_job_id(t_shell *m_s);
int mx_insert_job(t_shell *m_s, t_job *job);
void mx_remove_job(t_shell *m_s, int id);
int mx_get_proc_count(t_shell *m_s, int job_id, int filter);
void mx_set_process_status(t_shell *m_s, int pid, int status);
int mx_set_job_status(t_shell *m_s, int job_id, int status);

int mx_job_id_by_pid(t_shell *m_s, int pid);
int mx_get_pgid_by_job_id(t_shell *m_s, int job_id);
int mx_job_completed(t_shell *m_s, int job_id);

void mx_print_pid_process_in_job(t_shell *m_s, int id);  // only if foreground execution
int mx_print_job_status(t_shell *m_s, int id);

void mx_check_jobs(t_shell *m_s);  //waitpid any process
int mx_wait_pid(t_shell *m_s, int pid);  //waitpid process by pid
int mx_wait_job(t_shell *m_s, int id);  //waitpid process in job group

void mx_destroy_jobs(t_shell *m_s, int id);  //free job memory

//      OTHER
void mx_printstr(const char *s);
void mx_printerr(const char *s);
char *mx_normalization (char *point, char *pwd);
void mx_push_export(t_export **list, void *name, void *value);
t_export *mx_set_variables();
t_export *mx_set_export();
int mx_count_options(char **args, char *options, char *command, char *error);
void mx_set_variable(t_export *export, char *name, char *value);
char mx_get_type(struct stat file_stat);
int mx_launch_bin(t_shell *m_s, t_process *p, char *path, char **env);

#endif
