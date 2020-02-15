#include "ush.h"

static char *mx_get_keys(char *promt);
static void print_command(char *promt, char *line, int position, int max_len);
static void exit_ush();
static void edit_command(int keycode, int *position, char *line);
static void backscape(int *position, char *line);
static char *get_line();
static int get_job_type(t_ast **ast, int i);
static struct termios mx_disable_term();
static void mx_enable_term(struct termios savetty);
static void add_char(int *position, char *line, int keycode);
static void read_input(int *max_len, int *keycode, char *line);
static void exec_signal(int keycode, char *line, int *position);
static void reverse_backscape(int *position, char *line);
sig_atomic_t flag = 0;

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
    
    while (1) {
        mx_check_jobs(m_s);
		line = get_line();
        if (line[0] == '\0') {
//            mx_check_jobs(m_s);
            continue;
        } else {
            if ((ast = mx_ast_creation(line, m_s))) {
                for (int i = 0; ast[i]; i++) {
                    t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
                    new_job = mx_create_job(m_s, ast[i]);
                    new_job->job_type = get_job_type(ast, i);
                    mx_launch_job(m_s, new_job);
                    m_s->history_count = 0;
                }
                mx_ast_clear_all(&ast);  // clear leeks
            }
        }
        mx_strdel(&line);
    }
}

static struct termios mx_disable_term() {
    struct termios savetty;
    struct termios tty;

    tcgetattr (0, &tty);
    savetty = tty;
    tty.c_lflag &= ~(ICANON|ECHO|ISIG|BRKINT|ICRNL
        |INPCK|ISTRIP|IXON|OPOST|IEXTEN);
    tty.c_cflag |= (CS8);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    tcsetattr (0, TCSAFLUSH, &tty);
    return savetty;
}

static void mx_enable_term(struct termios savetty) {
    tcsetattr (0, TCSAFLUSH, &savetty);
}

static int get_job_type(t_ast **ast, int i) {
    t_ast *tmp = NULL;

    if (i != 0) {
        tmp = ast[i - 1];
        while (tmp->next)
            tmp = tmp->next;
        if (tmp->type == AND || tmp->type == OR)
            return tmp->type;
    }
    return 0;
}

static char *get_line() {
    char *line;
    struct termios savetty;

    savetty = mx_disable_term();
    printf ("\ru$h> ");
    fflush (NULL);
    line = mx_get_keys("u$h>");
    printf("\n");
    mx_enable_term(savetty);
    return line;
}

static char *mx_get_keys(char *promt) {
	char *line = mx_strnew(1024);
   	int keycode = 0;
   	int max_len = 0;
   	int position = 0;

    for (;keycode != ENTER && keycode != CTRL_C;) {
    	read_input(&max_len, &keycode, line);
        if (keycode >= 127)
            edit_command(keycode, &position, line);
        else if (keycode < 32)
            exec_signal(keycode, line, &position);
        else
            add_char(&position, line, keycode);
        if (keycode != CTRL_C)
            print_command(promt, line, position, max_len);
    }
    return line;
}

static void read_input(int *max_len, int *keycode, char *line) {
    *max_len = mx_strlen(line);
    *keycode = 0;
    read(STDIN_FILENO, keycode, 4);
}

static void exec_signal(int keycode, char *line, int *position) {
    if (keycode == CTRL_C) {
        for (int i = 0; i < mx_strlen(line); i++) {
            line[i] = '\0';
        }
    }
    if (keycode == CTRL_D)
        if (strcmp(line, "") == 0)
            exit_ush();
        else
            reverse_backscape(position,  line);
    else if (keycode == TAB) {
        //to do
    }
}

static void exit_ush() {
    printf("exit\n");
    exit(EXIT_SUCCESS);
}

static void backscape(int *position, char *line) {
    if (*position > 0) {
        for (int i = *position - 1; i < mx_strlen(line); i++) {
            line[i] = line[i + 1];
        }
        (*position)--;
    }
}

static void reverse_backscape(int *position, char *line) {
        for (int i = *position; i < mx_strlen(line); i++) {
            line[i] = line[i + 1];
        }
}

static void add_char(int *position, char *line, int keycode) {
    for (int i = mx_strlen(line); i > *position; i--) {
        line[i] = line[i - 1];
    }
    line[*position] = keycode;
    (*position)++;
}

static void edit_command(int keycode, int *position, char *line) {
    if (keycode == K_LEFT)
        *position > 0 ? (*position)-- : 0;
    else if (keycode == K_RIGHT) 
        *position < mx_strlen(line) ? (*position)++ : 0;
    else if (keycode == K_END)
        *position = mx_strlen(line);
    else if (keycode == K_DOWN) {
        //  вперед в прошлое
    }
    else if (keycode == K_UP) {
        // назад в будущее
    }
    else if (keycode == BACKSCAPE)
        backscape(position, line);
}

static void print_command(char *promt, char *line, int position, int max_len) {
		for (int i = position; i < mx_strlen(line); i++) {
        	printf (" ");
        }
	    for (int i = 0; i <= max_len + mx_strlen(promt) + 1; i++) {
	        printf ("\b\x1b[2K");
	    }
        printf ("%s %s", promt, line); 
        for (int i = 0; i < mx_strlen(line) - position; i++) {
        	printf ("%c[1D", 27);
        }
        fflush (NULL);
}



