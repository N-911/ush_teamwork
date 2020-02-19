#include "ush.h"

void mx_terminal_init(t_shell *m_s) {
  //  char buf[102];
    char *term;

    term = getenv("TERM");
    if (term == NULL)
        term = "UTERM";
//    tgetent(buf, term);
    mx_termios_save(m_s);
    return;
}

void mx_termios_save(t_shell *m_s) {
    if (tcgetattr(STDIN_FILENO, &m_s->t_original) == -1) {
        mx_printerr("tcgetattr() failed");
        exit(MX_EXIT_FAILURE);
    }
    m_s->t_custom = m_s->t_original;
    m_s->t_custom.c_lflag &= ~(ECHO);
    m_s->t_custom.c_lflag &= ~(ICANON);
    m_s->t_custom.c_cc[VMIN] = 1;  //MIN определяет минимальное количество байтов, по прочтении которого функция read должна возвращать управление
    m_s->t_custom.c_cc[VTIME] = 0; //TIME задает количество десятых долей секунды, в течение которых следует ожидать поступления данных.
    if (tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_custom) == -1) {
        mx_printerr("tcsetattr() failed");
        exit(MX_EXIT_FAILURE );
    }
    m_s->custom_terminal = TRUE;
}

void termios_restore(t_shell *m_s) {
    if (m_s->custom_terminal == TRUE)
        tcsetattr(STDIN_FILENO, TCSANOW, &m_s->t_original);
}

/*
 struct termios {
  tcflag_t  c_iflag;  // флаги режима ввода
  tcflag_t  c_oflag;  // флаги режима вывода
  tcflag_t  c_cflag;  // флаги режима управления
  tcflag_t  c_lflag;  // флаги локального режима
  cc_t      c_cc[NCCS]; // управляющие символы
};
*/
