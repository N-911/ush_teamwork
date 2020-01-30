#include "ush.h"

static int count_options(char **args, echo_t *echo_options)

int mx_echo(t_shell *m_s, t_process *p) {
    int exit_code = 0;
    echo_t echo_options = {0, 0, 0};
    int n_options = count_options(p->argv, &echo_options);

    if (!p->argv[1])
        mx_printstr("\n");
    else if (strcmp(p->argv[1], "-n") == 0) {
        if (p->argv[2]) {
            mx_printstr(p->argv[2]);
            mx_printstr("%");
            mx_printstr("\n");
        }
    }
    else {
        mx_printstr(p->argv[1]);
        mx_printstr("\n");
    }
    return exit_code;
}

static int count_options(char **args, echo_t *echo_options) {
	int n_options = 0;

	for (int i = 1; args[i] != NULL; i++) {
		if (args[i][0] == '-' ) {
			if (args[i][0] == 'n')
				echo_options->n = 1;
			else if (args[i][0] == 'e')
				echo_options->e = 1;
			else if (args[i][0] == 'E')
				echo_options->E = 1;
			else
				break
		}
		else
			break
		n_options++;
	}
	return n_options;
}



