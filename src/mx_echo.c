#include "ush.h"

static int count_options(char **args);
static char *replace_substr(const char *str, const char *sub, const char *replace);
static void fill_options(char **args, echo_t *echo_options, int n_options);
static char *replace_slash(const char *str);

int mx_echo(t_shell *m_s, t_process *p) {
    int exit_code = m_s->exit_code;
    echo_t echo_options = {0, 0, 0};
    int n_options = count_options(p->argv);
    char *sequenses[] = {"\\a","\\b","\\f","\\n","\\r","\\t","\\v",NULL};
    char *escape[] = {"\a","\b","\f","\n","\r","\t","\v",NULL};

    fill_options(p->argv, &echo_options, n_options);
    for(int i = n_options + 1; p->argv[i] != NULL; i++) {
    	if (!echo_options.E) {
            p->argv[i] = replace_slash(p->argv[i]);
    		for (int j = 0; sequenses[j] != NULL; j++) {
    			if (strstr(p->argv[i],sequenses[j])) {
	    			p->argv[i] = replace_substr(p->argv[i],sequenses[j], escape[j]);
    			}
    		}
    	}
    	printf("%s",p->argv[i]);
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


static char *replace_slash(const char *str) {
    char *res = (char *)malloc(mx_strlen(str));
    int len = 0;

    for (int i = 0; i < mx_strlen(str); i++) {
        if (str[i] == '\\' && str[i + 1] == '\\') {
            i++;
        }
        res[len] = str[i];
        len++;
    }
    res[len] = '\0';
    
    return res;
}
static char *replace_substr(const char *str, const char *sub, const char *replace) {
    char *res = mx_strdup(str);
    char *buff1 = mx_strnew(mx_strlen(str));
    char *buff2 = mx_strnew(mx_strlen(str));
    while(mx_strstr(res,sub) != NULL) {
        int i = mx_get_substr_index(res,sub);
        mx_strncpy(buff1, res, i);
        for(int j = 0; j < i + mx_strlen(sub); j++){
            res++;
        }
        mx_strcpy(buff2,res);
        res = "";
        res = mx_strjoin(res, buff1);
        res = mx_strjoin(res, replace);
        res = mx_strjoin(res, buff2);
    }
    free(buff1);
    free(buff2);
    return res;
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
			

