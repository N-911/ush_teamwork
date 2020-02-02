#include "ush.h"
/*
*  read line from stdin except last char ('\n')
*/
char *mx_ush_read_line(void) {
    char *line = NULL;
    char *res = NULL;
    size_t bufsize = 0; // have getline allocate a buffer for us

    	// struct termios savetty;
        // struct termios tty;

        // if ( !isatty(0) ) { /*Проверка: стандартный ввод - терминал?*/
        //   fprintf (stderr, "stdin not terminal\n");
        //   exit (1);  Ст. ввод был перенаправлен на файл, канал и т.п. 
        // };
        // tcgetattr (0, &tty);
        // savetty = tty; /* Сохранить упр. информацию канонического режима */
        // tty.c_lflag &= ~(ICANON|ECHO|ISIG);
        // tty.c_cc[VMIN] = 1;
        // tcsetattr (0, TCSAFLUSH, &tty);
    	// printf("\n");
        // tcsetattr (0, TCSAFLUSH, &savetty);

    getline(&line, &bufsize, stdin);
    res = mx_strndup(line, mx_strlen(line) - 1);
    mx_strdel(&line);
    return res;
}
