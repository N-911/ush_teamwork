#include "ush.h"

static char *mx_get_keys(t_shell *m_s);
static void print_command(t_shell *m_s, char *line, int position, int max_len);
static void exit_ush();
static void edit_command(int keycode, int *position, char **line, t_shell *m_s);
static void backscape(int *position, char *line);
static char *get_line(t_shell *m_s);
static int get_job_type(t_ast **ast, int i);
static struct termios mx_disable_term();
static void mx_enable_term(struct termios savetty);
static void add_char(int *position, char *line, int keycode);
static void read_input(int *max_len, int *keycode, char *line);
static void exec_signal(int keycode, char *line, int *position);
static void reverse_backscape(int *position, char *line);
static char *get_variable(t_shell *m_s, char *target);
static void edit_prompt(t_shell *m_s);
static void print_prompt(t_shell *m_s);

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;
    
    while (1) {
        // int flag = 0;
        // char *path = ".";
        // while(!flag) {
        //     DIR *dptr  = opendir(path);
        //     struct dirent  *ds;

        //         while ((ds = readdir(dptr)) != 0) {//cчитываем хуйню из директории
        //             if (strcmp(ds->d_name, ".git") == 0) {
        //                 char *gitpath = mx_strjoin(path, "/.git/HEAD");
        //                 char *git = mx_file_to_str(gitpath);
        //                 char **arr = mx_strsplit(git, '/');
        //                 int count = 0;
        //                 while (arr[count] != NULL)
        //                     count++;
        //                 char *last = strdup(arr[count - 1]);
        //                 char *user = strndup(last,mx_strlen(last) - 1);
        //                 printf("%sgit:(%s%s%s)%s\n",BLU, RED,user, BLU, RESET);
        //                 flag++;
        //                 break;
        //             }
        //         }
        //     if (strcmp(realpath(path, NULL), getenv("HOME")) == 0)
        //         flag++;
        //     closedir(dptr);
        //     path = mx_strjoin(path, "/..");
        // }
		line = get_line(m_s);
        if (line[0] == '\0') {
            mx_check_jobs(m_s);
            continue;
        } else {
            if ((ast = mx_ast_creation(line, m_s))) {
                for (int i = 0; ast[i]; i++) {
                    t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
                    new_job = mx_create_job(m_s, ast[i]);
                    new_job->job_type = get_job_type(ast, i);
                    mx_launch_job(m_s, new_job);
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

static char *get_line(t_shell *m_s) {
    char *line;
    struct termios savetty;

    edit_prompt(m_s);

    savetty = mx_disable_term();
    print_prompt(m_s);
    fflush (NULL);
    line = mx_get_keys(m_s);
    if (m_s->history_count == m_s->history_size) {
        m_s->history_size += 1000;
        m_s->history = (char **)realloc(m_s->history, m_s->history_size);
    }
    if (strcmp(line, "") != 0) {
        m_s->history[m_s->history_count] = strdup(line);
        m_s->history_count++;
    }
    m_s->history_index = m_s->history_count;
    printf("\n");
    mx_enable_term(savetty);
    return line;
}

static void edit_prompt(t_shell *m_s) {
    if (!m_s->prompt_status) {
            char *info = mx_strnew(256);
            int i = mx_strlen(m_s->pwd) - 1;
            int len = 0;
            while(m_s->pwd[i] != '/') {
                info[len] = m_s->pwd[i];
                i--;
                len++;
            }
            len--;
            for (int j = 0; j <= len / 2; j++) {
                char tmp = info[j];
                info[j] = info[len - j];
                info[len - j] = tmp; 
            }
            if (strcmp(m_s->pwd, "\\") == 0)
                info = strdup("\\");
            if (strcmp(m_s->pwd, getenv("HOME")) == 0)
                info = strdup("~");
            m_s->prompt = strdup(info);
        }
    else {
        if (get_variable(m_s, "PROMPT"))
                m_s->prompt = get_variable(m_s, "PROMPT");
            else
                m_s->prompt = "u$h";
    }
}

static char *mx_get_keys(t_shell *m_s) {
	char *line = mx_strnew(1024);
   	int keycode = 0;
   	int max_len = 0;
   	int position = 0;

    for (;keycode != ENTER && keycode != CTRL_C;) {
    	read_input(&max_len, &keycode, line);
        max_len += mx_strlen(m_s->prompt);  
        if (keycode >= 127)
            edit_command(keycode, &position, &line, m_s);
        else if (keycode < 32)
            exec_signal(keycode, line, &position);
        else
            add_char(&position, line, keycode);
        if (keycode != CTRL_C){
            print_command(m_s, line, position, max_len);
        }
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

static void edit_command(int keycode, int *position, char **line, t_shell *m_s) {
    if (keycode == K_LEFT)
        *position > 0 ? (*position)-- : 0;
    else if (keycode == K_RIGHT) 
        *position < mx_strlen(*line) ? (*position)++ : 0;
    else if (keycode == K_END)
        *position = mx_strlen(*line);
    else if (keycode == K_DOWN) {
        if (m_s->history[m_s->history_index + 1] && m_s->history_index < m_s->history_count) {
            free(*line);
            *line = strdup(m_s->history[m_s->history_index + 1]);
            *position = mx_strlen(*line);
            m_s->history_index++;
        }
    }
    else if (keycode == K_UP) {
        if (m_s->history[m_s->history_index - 1] && m_s->history_index > 0) {
            free(*line);
            *line = strdup(m_s->history[m_s->history_index - 1]);
            *position = mx_strlen(*line);
            m_s->history_index--;
        }
    }
    else if (keycode == C_PROMPT) {
        m_s->prompt_status ? m_s->prompt_status-- : m_s->prompt_status++;
        edit_prompt(m_s);
    }
    else if (keycode == BACKSCAPE)
        backscape(position, *line);
}

static char *get_variable(t_shell *m_s, char *target) {
    t_export *head = m_s->variables;

    while (head != NULL) {
        if (strcmp(head->name, target) == 0) {
            return head->value;
        }
        head = head->next;
    }
    return NULL;
}

static void print_command(t_shell *m_s, char *line, int position, int max_len) {
		for (int i = position; i < mx_strlen(line); i++) {
        	printf (" ");
        }
	    for (int i = 0; i <= max_len + 2; i++) {
	        printf ("\b\x1b[2K");
	    }
        print_prompt(m_s);
        printf ("%s", line);
        for (int i = 0; i < mx_strlen(line) - position; i++) {
        	printf ("%c[1D", 27);
        }
        fflush (NULL);
}

static void print_prompt(t_shell *m_s) {
    if (!m_s->prompt_status)
        printf("%s", BOLD_CYAN);
    printf ("%s", m_s->prompt);
    if (!m_s->prompt_status)
        printf("%s", RESET);
    printf ("> ");
}






