#include "ush.h"

static int count_options(char **args);
static char *replace_substr(char *str, char *sub, char *replace);
static void fill_options(char **args, echo_t *echo_options, int n_options);
static char *replace_slash(const char *str, echo_t *echo_options);
static char *get_result(char *buff1, char *buff2,  char *replace);
static void edit_argv(int n_options, t_process *p, char *sequenses[], char *escape[]);


int mx_echo(t_shell *m_s, t_process *p) {
    int exit_code = m_s->exit_code;
    int n_options = count_options(p->argv);
    char *sequenses[] = {"\\a","\\b","\\f","\\n","\\r","\\t","\\v",NULL};
    char *escape[] = {"\a","\b","\f","\n","\r","\t","\v",NULL};

    edit_argv(n_options, p, sequenses, escape);
    return exit_code;
}

static void edit_argv(int n_options, t_process *p, char *sequenses[], char *escape[]) {
    echo_t echo_options = {0, 0, 0};

    fill_options(p->argv, &echo_options, n_options);
    for(int i = n_options + 1; p->argv[i] != NULL; i++) {
        if (!echo_options.E) {
            p->argv[i] = replace_slash(p->argv[i], &echo_options);
            for (int j = 0; sequenses[j] != NULL; j++) {
                if (strstr(p->argv[i],sequenses[j]))
                    p->argv[i] = replace_substr(p->argv[i],sequenses[j], escape[j]);
            }
        }
        printf("%s",p->argv[i]);
        if(strstr(p->argv[i],"\\c"))
            break;
        if (p->argv[i + 1])
            mx_printstr(" ");
    }
    !echo_options.n ? printf("\n") : 0;
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


static char *replace_slash(const char *str, echo_t *echo_options) {
    char *res = (char *)malloc(mx_strlen(str));
    int len = 0;

    for (int i = 0; i < mx_strlen(str); i++) {
        if (str[i] == '\\' && str[i + 1] == '\\')
            i++;
        if (str[i] == '\\' && str[i + 1] == 'e') {
            if (str[i + 2] != '\\')
                i += 3;
            else
                i+= 2;
        }
        if (str[i] == '\\' && str[i + 1] == 'c') {
            echo_options->n = 1;
            break;
        }
        if (str[i] == '\\' && str[i + 1] == 'x') {
            if (!str[i + 2])
                i += 2;
            else {
                char rep = mx_hex_to_nbr(strndup(str + i + 2, 2));
                res[len] = rep;
                len++;
                i += 4;
            }
        }
        res[len] = str[i];
        len++;
    }
    res[len] = '\0';
    
    return res;
}
static char *replace_substr(char *str,  char *sub, char *replace) {
    char *res = strdup(str);
    char *buff1 = mx_strnew(mx_strlen(str));
    char *buff2 = mx_strnew(mx_strlen(str));
    while(mx_strstr(res,sub) != NULL) {
        int i = mx_get_substr_index(res,sub);
        mx_strncpy(buff1, res, i);
        for(int j = 0; j < i + mx_strlen(sub); j++)
            res++;
        mx_strcpy(buff2,res);
        res = get_result(buff1, buff2, replace);
    }
    free(buff1);
    free(buff2);
    return res;
}

static char *get_result(char *buff1, char *buff2,  char *replace) {
    char *tmp = NULL;
    char *res = NULL;

    tmp = strdup(buff1);
    res = mx_strjoin(tmp, replace);
    free(tmp);
    tmp = strdup(res);
    free(res);
    res = mx_strjoin(tmp, buff2);
    free(tmp);
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
			

