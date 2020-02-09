#include "ush.h"

static char *mx_get_keys();

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

void mx_ush_loop(t_shell *m_s) {
    char *line;
    t_ast **ast = NULL;

    while (1) {

        struct termios savetty;
  		struct termios tty;

        tcgetattr (0, &tty);
		savetty = tty;
		tty.c_lflag &= ~(ICANON|ECHO|ISIG|BRKINT | ICRNL | INPCK | ISTRIP | IXON |OPOST |IEXTEN);
		tty.c_cflag |= (CS8);
		tty.c_cc[VMIN] = 1;
		tty.c_cc[VTIME] = 0;
		tcsetattr (0, TCSAFLUSH, &tty);
		printf ("\ru$h> ");
		fflush (NULL);
        line = mx_get_keys();
        tcsetattr (0, TCSAFLUSH, &savetty);
        if (line[0] == '\0') {
            mx_check_jobs(m_s);
            continue;
        } else {
            if ((ast = mx_ast_creation(line, m_s))) {
                // ast_print(ast);  // печать дерева
                for (int i = 0; ast[i]; i++) {
                    t_job *new_job = (t_job *) malloc(sizeof(t_job));  //create new job
                    new_job = mx_create_job(m_s, ast[i]);
                    new_job->job_type = get_job_type(ast, i);
                    mx_launch_job(m_s, new_job);
                    //m_s->exit_code = status;
                    // mx_destroy_jobs(m_s, 0);
                    m_s->history_count = 0;
                    //  mx_add_history(m_s, new_job);
        //        	termios_restore(m_s);
                }
                mx_ast_clear_all(&ast);  // clear leeks
                // system ("leaks -q ush");
            }
        }
        mx_strdel(&line);
    }
}

static char *mx_get_keys() {
	char *line = mx_strnew(256);
	int len = 0;
   	int keycode = 0;

    while (keycode != 10) {
    	keycode = 0;
    	read(STDIN_FILENO, &keycode, 4);
        if (keycode == K_LEFT) {

        }
        else if (keycode == CTRL_D) {
            exit(EXIT_SUCCESS);
        }
        else if (keycode == BACKSCAPE) {
        	if (len > 0) {
        		line[len - 1] = '\0';
        		len--;
        	}
        }
        else {
        	line[len] = keycode;
        	len++;
        }
        printf ("\r");
	    for (int i = 0; i < 25; i++) {
	        printf (" ");
	        fflush (NULL);
	    }
        printf ("\ru$h> %s", line);
        fflush (NULL);
    }
    return line;
}
