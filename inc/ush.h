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
#include <sys/types.h>
#include <stdlib.h>
#include <sys/param.h>     //   const MAXPATHLEN      PATH_MAX
#include <termios.h>
#include <signal.h>
#include <term.h>  // compile with flags -ltermcap or -lncurses
#include <curses.h>
#include <malloc/malloc.h>
#include <limits.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>

#include "libmx/inc/libmx.h"

#define LSH_RL_BUFSIZE 1024
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

//  EXIT
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

//  JOBS
#define JOBS_NUMBER 100
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

//KEYBOARDS
#define INPUT_SIZE  1024
#define K_LEFT      4479771  // edit keys
#define K_RIGHT     4414235
#define K_HOME      4741915
#define K_END       4610843
#define K_UP        4283163  // history keys
#define K_DOWN      4348699
#define P_UP        2117425947
#define P_DOWN      2117491483
#define C_PROMPT    42946
#define CTRL_D      4
#define CTRL_C      3
#define CTRL_R      18
#define BACKSCAPE   127
#define TAB         9
#define ENTER       10

/*
*  ---------------------------------------------- Abstract Syntax Tree
*/
/* Operators and delimeters for parse tokens */
#define PARSE_DELIM ";|&><"
#define QUOTE "\'\"`"
#define USH_TOK_DELIM " \t\r\n\a"

/* Macroces for recognizing delimeters */
#define IS_SEP(x) (!mx_strcmp(x, ";"))
#define IS_FON(x) (!mx_strcmp(x, "&"))
#define IS_AND(x) (!mx_strcmp(x, "&&"))
#define IS_OR(x) (!mx_strcmp(x, "||"))
#define IS_PIPE(x) (!mx_strcmp(x, "|"))
#define IS_R_INPUT(x) (!mx_strcmp(x, "<"))  // redirections
#define IS_R_INPUT_DBL(x) (!mx_strcmp(x, "<<"))
#define IS_R_OUTPUT(x) (!mx_strcmp(x, ">"))
#define IS_R_OUTPUT_DBL(x) (!mx_strcmp(x, ">>"))
#define IS_SEP_FIRST_LWL(x) (x == SEP || x == FON || x == AND || x == OR)
#define IS_REDIR_INP(x) (x == R_INPUT || x == R_INPUT_DBL)
#define IS_REDIR_OUTP(x) (x == R_OUTPUT || x == R_OUTPUT_DBL)
#define IS_REDIRECTION(x) (IS_REDIR_INP(x) || IS_REDIR_OUTP(x))

/* Types of operators */
enum e_type {
    SEP, //;
    FON, // &
    AND, // &&
    OR,  // ||
    PIPE,
    R_INPUT,
    R_INPUT_DBL,
    R_OUTPUT,
    R_OUTPUT_DBL,
    NUL
};

/* For creation Abstract Syntax Tree */
typedef struct s_ast {
    char **args;            // cmd with args
    int type;               // type of delim after cmd (last -> ;)
    struct s_ast *next;
    struct s_ast *left;     // for redirections
    struct s_ast *parent;   // delete !!!!!
} t_ast;

typedef struct s_jobs  {
    int l;
    int r;
    int s;
} t_jobs;

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

typedef struct		s_stack
{
    int			size;  // size = JOBS_NUMBER
    int*		stack;
    int 		top;  // index of last add job
    int         last;  // current job gor fg
    int         prev_last;
} t_stack;


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
    char **argv;    // gets in create_job.c
    // char **envp;
    char *command;
    char *arg_command;
    char *input_path;  // > >>
    char *output_path;  // < <<
    int redir_delim;  // <, <<, >, >> from e_type
    pid_t pid;
    int exit_code;
    char *path;
    char **env;
    int status;  //status RUNNING DONE SUSPENDED CONTINUED TERMINATED
    int foreground;
    int pipe;  // gets in create_job.c
    int delim;  // gets in create_job.c (first - | || &&) (end - ; &)
    int fd_in;
    int fd_out;
    int type;              // COMMAND_BUILTIN = index in m_s->builtin_list; default = 0
    struct s_process *next;     // next process in pipeline
    pid_t pgid;
    int infile;
    int outfile;
    int errfile;
} t_process;

// A job is a pipeline of processes.
typedef struct s_job {
    int job_id;                 //number in jobs control
    int job_type;           // 0 if normal, or enum &&, || of previos job
    char *command;              // command line, used for messages
    t_process *first_process;     // list of processes in this job
    pid_t pgid;                 // process group ID
    int exit_code;
    int foreground;                  // foreground = 1 or background execution = 0
    struct termios tmodes;      // saved terminal modes/
    int stdin;  // standard i/o channels
    int stdout;  // standard i/o channels
    int stderr;  // standard i/o channels
    struct s_job *next;  //next job separated by ";" "&&" "||"
} t_job;

typedef struct s_shell {
    int     argc;
    char    **argv;  // check usage, becouse the same in process    ??????
    char	**envp;  //not used
    int		exit_code;  //return if exit
    t_job   *jobs[JOBS_NUMBER];     //arr jobs
    t_stack *jobs_stack;
    int max_number_job;  // number of added jobs + 1
    char **builtin_list;  // buildin functions
    int     exit_flag;  // defaults 0, cheack if you have suspended jobs
    char **history;
    int history_count;
    int history_index;
    struct termios tmodes;
    struct termios t_original;
    struct termios t_custom;
    bool custom_terminal;
    pid_t shell_pgid;
    char *pwd;
    char *prompt;
    int prompt_status;
    t_export *exported;
    t_export *variables;
} t_shell;

static volatile sig_atomic_t sigflag; // устанавливается обработчиком  в ненулевое значение
static sigset_t newmask, oldmask, zeromask;

/*
 *  ---------------------------------------------- Abstract Syntax Tree
 * ast_print            print array of ast-lists (all jobs);
 * mx_ast_creation      get parsed_line -> get ast (array of lists);
 * mx_ush_parsed_line   get list of all cmds and operators -> use filters;
 * mx_ast_parse         create ast (array of lists) from parsed_line (list);
 * mx_ast_push_back     std push_back and create_node in it;
 * mx_ast_push_back_redirection     std push_back left for redirection;
 * mx_ast_clear_list    clear one lists (parsed_line);
 * mx_ast_clear_all     clear array of lists (Abstract Syntax Tree);
 * mx_check_parce_errors    check all possible errors of parsing;
 * mx_ush_read_line     read line from stdin except last char ('\n');
 */
void ast_print(t_ast **ast);                            // mx_ast_creation.c
t_ast **mx_ast_creation(char *line, t_shell *m_s);
t_ast *mx_ush_parsed_line(char *line, t_export *variables);
t_ast **mx_ast_parse(t_ast *parsed_line);
void mx_ast_push_back(t_ast **head, char **args, int type);
/* rewrite */ void mx_ast_push_back_redirection(t_ast **head, t_ast **list);
void mx_ast_clear_list(t_ast **list);
void mx_ast_clear_all(t_ast ***list);                 // mx_ast_clear_list.c
bool mx_check_parce_errors(char *line);
char *mx_ush_read_line(void);
/*
 *  ---------------------------------------------- FILTERS
 * mx_filters       parse by USH_TOK_DELIM, subst ~, $, trim'' "";
 * mx_strtok        like std strtok return one token,
 *                  unlike std - works correct with '', "" and func () { x; };
 * mx_parce_tokens  get array of pointers to separate tokens in line,
 *                  don't strdup input line, but "cut" existing line with '\0';
 * mx_subst_tilde   subst ~ (tilde);
 * mx_substr_dollar subst $ (variable);
 */
/* check */char **mx_filters(char *arg, t_export *variables);
/* check */char *mx_strtok (char *s, const char *delim);
/* check */char **mx_parce_tokens(char *line);
char *mx_subst_tilde(char *s);
char *mx_substr_dollar(char *s, t_export *variables);
/*
 *  ---------------------------------------------- mx_quote_manage.c
 * mx_get_char_index_quote      get char index outside of the quote (exc is \);
 * mx_get_char_index_ush        get char index, except isolated with \;
 * mx_count_chr_quote           count chars (outside of the quote);
 * mx_count_chr_ush             count chars, except isolated with \;
 * mx_strtrim_quote             trim all ' or " in quote;
 */
int mx_get_char_index_quote(const char *str, char *c, char *quote);
int mx_get_char_index_ush(const char *str, char c);
int mx_count_chr_quote(const char *str, char *c, char *q);
int mx_count_chr_ush(const char *str, char c);
/* rewrite */ char *mx_strtrim_quote(char *s, char q_char);
/*
 *  ---------------------------------------------- move to LIBMX
 */
/*
 *  libmx1.c
 */
char *mx_strjoin_free(char *s1, char const *s2);
int mx_strlen_arr(char **s);
char **mx_strdup_arr(char **str);
void mx_print_strarr_in_line(char **res, const char *delim);
void mx_set_buff_zero(void *s, size_t n);
/*
 *  libmx2.c
 */
void mx_printerr_red(char *c);
void mx_print_color(char *macros, char *str);
int mx_get_char_index_reverse(const char *str, char c);
bool mx_isdelim (char c, char *delim);
bool mx_check_allocation_error(const void *c);
/*
 *  ----------------------------------------------
 */

t_shell *mx_init_shell(int argc, char **argv);

//      TERMINAL
void mx_terminal_init(t_shell *m_s);
void mx_termios_save(t_shell *m_s);
void termios_restore(t_shell *m_s);

//      LOOP
//char *mx_read_line2(void);  // delete
// char **mx_ush_split_line(char *line);  // delete
t_job *mx_create_job(t_shell *m_s, t_ast *list);  // create one job from ast
void mx_ush_loop(t_shell *m_s);  // create ast -> create jobs -> ...
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
void mx_sig_h(int signal);
void mx_sig_handler_exit(int sig);
void sigchld_handler(int signum);
void mx_sig_handler(int signal);
//void sig_usr(int signo);
void TELL_WAIT(void);
void TELL_PARENT(pid_t pid);
void WAIT_PARENT(void);
void TELL_CHILD(pid_t pid);
void WAIT_CHILD(void);


//      JOBS
int mx_get_next_job_id(t_shell *m_s);
int mx_insert_job(t_shell *m_s, t_job *job);
void mx_remove_job(t_shell *m_s, int job_id);
int mx_get_proc_count(t_shell *m_s, int job_id, int filter);
void mx_set_process_status(t_shell *m_s, int pid, int status);
int mx_set_job_status(t_shell *m_s, int job_id, int status);
int mx_get_job_status(t_shell *m_s, int job_id, int status);
void mx_set_last_job(t_shell *m_s);
int mx_find_job_by_p_name(t_shell *m_s, char *arg);

int mx_job_is_running(t_shell *m_s, int job_id);

void mx_init_jobs_stack(t_shell *m_s);
void mx_push_to_stack (t_shell *m_s, int job);
void mx_pop_from_stack(t_shell * m_s, int job);
bool mx_get_from_stack(t_shell *m_s, int job_id);
void mx_print_stack (t_shell *m_s);
//int mx_get_from_stack(t_shell *m_s);

int mx_job_id_by_pid(t_shell *m_s, int pid);
int mx_get_pgid_by_job_id(t_shell *m_s, int job_id);
int mx_job_completed(t_shell *m_s, int job_id);

void mx_print_pid_process_in_job(t_shell *m_s, int id);  // only if foreground execution
int mx_print_job_status(t_shell *m_s, int job_id, int flag);
void mx_print_args_in_line(char **res, const char *delim);

void mx_check_jobs(t_shell *m_s);  //waitpid any process
int mx_wait_pid(t_shell *m_s, int pid);  //waitpid process by pid
int mx_wait_job(t_shell *m_s, int id);  //waitpid process in job group

void mx_destroy_jobs(t_shell *m_s, int id);  //free job memory

//      OTHER
void mx_error_fg_bg(char *arg0, char *arg1, char *arg2, char *arg3);
int mx_check_args(t_shell *m_s, t_process *p);  // use in fg and bg

void mx_printstr(const char *s);
void mx_printerr(const char *s);
char *mx_normalization (char *point, char *pwd);
void mx_push_export(t_export **list, void *name, void *value);
t_export *mx_set_variables();
t_export *mx_set_export();
int mx_count_options(char **args, char *options, char *command, char *error);
void mx_set_variable(t_export *export, char *name, char *value);
char mx_get_type(struct stat file_stat);
int mx_launch_bin(t_process *p, char *path, char **env);
int mx_set_parametr(char **args,  t_shell *m_s);
char *mx_nbr_to_hex(unsigned long nbr);
unsigned long mx_hex_to_nbr(const char *hex);

int mx_launch_builtin(t_shell *m_s, t_process *p, int job_id);
//int mx_launch_builtin(t_shell *m_s, t_process *p);


#endif
