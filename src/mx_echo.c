#include "ush.h"

static int count_options(char **args);
static char *replace_sub(char *str, char *sub, char *replace);
static void fill_options(char **args, echo_t *echo_options, int n_options);

int mx_echo(t_shell *m_s, t_process *p) {
    int exit_code = m_s->exit_code;
    echo_t echo_options = {0, 0, 0};
    int n_options = count_options(p->argv);
    char *sequenses[] = {"\\a","\\b","\\f","\\n","\\r","\\t","\\v",NULL};
    char *escape[] = {"\a","\b","\f","\n","\r","\t","\v",NULL};

    fill_options(p->argv, &echo_options, n_options);
    for(int i = n_options + 1; p->argv[i] != NULL; i++) {
    	if (echo_options.e) {
    		for (int j = 0; sequenses[j] != NULL; j++) {
    			if (strstr(p->argv[i],sequenses[j])) {
	    			p->argv[i] = replace_sub(p->argv[i],sequenses[j], escape[j]);
    			}
    		}
    	}
    	mx_printstr(p->argv[i]);
    	if (p->argv[i + 1])
    		mx_printstr(" ");
    }
    if (!echo_options.n)
    	mx_printstr("\n");
    return exit_code;
}

static int count_options(char **args) {
	int n_options = 0;

	for (int i = 1; args[i] != NULL; i++) {
		if (args[i][0] == '-' ) {
			if (!strchr("neE",args[i][1])) 
				break;
		}
		else
			break;
		n_options++;
	}
	return n_options;
}

static char *replace_sub(char *str, char *sub, char *replace) {
	char *result;
	int index = mx_get_substr_index(str,sub);
	char *buff_1 = strndup(str, index);
	char *buff_2;

    for (int i = 0; i < index + mx_strlen(sub); i++) {
        str++;
    }
    buff_2 = strdup(str);
    result = mx_strjoin(buff_1, replace);
    char *tmp = mx_strjoin(result,buff_2);
    free(result);
    result = strdup(tmp);
    free(tmp);
    free(buff_1);
    free(buff_2);
    return(result);
}

static void fill_options(char **args, echo_t *echo_options, int n_options) {
	for (int i = n_options; i >= 0; i--) {
		for (int j = mx_strlen(args[i]); j >= 1; j--) {
			if (args[i][j] == 'n') {
				echo_options->n = 1;
			}
			else if (args[i][j] == 'e') {
				if (!echo_options->E)
					echo_options->e = 1;
			}
			else if (args[i][j] == 'E') {
				if (!echo_options->e)
					echo_options->E = 1;
			}
		}
	}
}
			

