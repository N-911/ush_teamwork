#include "ush.h"

static int count_args(char **args, int n_all);
static int count_variables(char **args, int n_options);
static int count_options(char **args, t_env_builtin *env);
static char *strdup_from(char *str, int index);
static void set_data(t_env_builtin *env, char *args[]);
static void delete_name(t_export **list, char *arg);
static void get_params (t_export *env_params, t_export *env_list, char **path);
static void print_env(t_export *env_list);
static char **get_args(t_process *p, int start);
static char **get_env_arr(t_export *env_list);
static void launch_command(t_shell *m_s, t_process *p, t_env_builtin *env, int *exit_code);
static t_env_builtin *init_env (t_process *p);
static void print_env_error(char option, char *error);
static char *get_parameter (char **args, int *i, int j, int *n_options);
static int add_param(char *param, t_export **env_params, char option);
static int add_option(char **args, int *i, int *n_options, t_env_builtin *env);
static void get_data(int i, char **args, t_env_builtin *env);
static char *get_option(char c);

int mx_env(t_shell *m_s, t_process *p) {
    t_env_builtin *env = init_env(p);
    int exit_code = 0;

    set_data(env, p->argv);
    if (env->n_options < 0 || env->n_variables < 0) 
        return 1;
    if (env->n_args == 0) {
        print_env(env->env_list);
    } 
    else {
        launch_command(m_s, p, env, &exit_code);
    }
  	return exit_code;
}

static int count_variables(char **args, int n_options) {
    int n_variables = 0;

    for (int i = n_options + 1; args[i] != NULL; i++) {
        if (mx_get_char_index(args[i],'=') < 0)
            break;
        if (mx_get_char_index(args[i],'=') == 0) {
            printf("env: setenv %s: Invalid argument\n", args[i]);
            return -1;
        }
        n_variables++;
    }
    return n_variables;
}

static int count_args(char **args, int n_all) {
    int n_args = 0;

    for (int i = n_all + 1; args[i] != NULL; i++) {
        n_args++;
    }
    return n_args;
}

static int count_options(char **args, t_env_builtin *env) {
    int n_options = 0;

    for (int i = 1; args[i] != NULL; i++) {
        if (args[i][0] != '-') break;
        if (strcmp(args[i], "--") == 0) {
            n_options++;
            break;
        }
        int valid_arg = add_option(args, &i, &n_options, env);
        if (valid_arg < 0)
        	return -1;
        n_options++;
    }
    return n_options;
}

static int add_option(char **args, int *i, int *n_options, t_env_builtin *env) {
    int exit_code = 0;
    char option;

	for (int j = 1; j <= mx_strlen(args[*i]); j++) {
        option = args[*i][j];
        if (args[*i][j] == 'u' || args[*i][j] == 'P') {
            char *param = get_parameter(args, i, j, n_options);
            exit_code = add_param(param, &env->env_params, option);

            free(param);
            break;
        }
        else if (args[*i][j] == 'i' || args[*i][j] == '-' || mx_strlen(args[*i]) == 1)
            env->env_options.i = 1;
        else if (j != mx_strlen(args[*i])) {
            print_env_error(option, "env: illegal option -- ");
            exit_code = -1;
            break;
        }
    }
    return exit_code;
}

static char *get_option(char c) {
    char *option = malloc(2);

    option[0] = c;
    option[1] = '\0';
    return option;
}

static void print_env_error(char option, char *error) {
	mx_printerr(error);
    write(2, &option, 1);
    mx_printerr("\nusage: env [-iv] [-P utilpath] [-u name]\n");
    mx_printerr("           [name=value ...] [utility [argument ...]]\n");
}

static int add_param(char *param, t_export **env_params, char option) {         
    if (param) {
    	if (option == 'u' && strchr(param, '=')) {
	        mx_printerr("env: unsetenv ");
	        mx_printerr(param);
	        mx_printerr(": Invalid argument\n");
	        return -1;
	    }
        char *str_option = get_option(option);
        mx_push_export(&*env_params, str_option, param);
        free(str_option);
        return 0;
    }
    else {
    	print_env_error(option, "env: option requires an argument -- ");
        return -1;
    }
}

static char *get_parameter (char **args, int *i, int j, int *n_options) {
	char *param = NULL;

    if(args[*i][j + 1] != '\0') {
        param = strdup_from(args[*i], j);
    }
    else if (args[*i + 1]) {
        param = strdup(args[*i + 1]);
        (*n_options)++;
        (*i)++;
    }
    return param;
}

static char *strdup_from(char *str, int index) {
    for (int i = 0; i <= index; i++) {
        str++;
    }
    return strdup(str);
}

static void set_data(t_env_builtin *env, char *args[]) {
    extern char** environ;

    for (int i = 0; environ[i] != NULL; i++) {
        if (!env->env_options.i) {
            get_data(i, environ, env);   
        }
    }
    get_params(env->env_params, env->env_list, &env->path);
    for (int i = env->n_options + 1; i <= env->n_options + env->n_variables; i++) {
        get_data(i, args, env);
    }
}

static void get_data(int i, char **args, t_env_builtin *env) {
    int idx = mx_get_char_index(args[i],'=');
    char *name = strndup(args[i],idx);
    char *value = strdup_from(args[i],idx);
            
    mx_push_export(&env->env_list, name, value);
    free(name);
    free(value);  
}

static void delete_name(t_export **list, char *arg) {
    t_export *head = *list;
    if (strcmp(head->name, arg) == 0) {
        *list = (*list)->next;
        return;
    }
    while (head != NULL) {
        if (head->next != NULL) {
            if (strcmp(head->next->name, arg) == 0) {
                free(head->next);
                head->next = head->next->next;
                break;
            }
        }
        head = head->next;
    }
}

static void get_params (t_export *env_params, t_export *env_list, char **path) {
    t_export *head = env_params;

    while (head != NULL) {
        if (strcmp(head->name, "u") == 0) {
            delete_name(&env_list, head->value);
        }
        if (strcmp(head->name, "P") == 0) {
            if (*path)
                free(*path);
            *path = strdup(head->value);
        }
        head = head->next;
    }
}

static void print_env(t_export *env_list) {
    t_export *head = env_list;

    while (head != NULL) {
        printf("%s=%s\n", head->name, head->value);
        head = head->next;
    }
}

static char **get_args(t_process *p, int start) {
    char **args_arr = (char **)malloc(sizeof(char *) * 256);
    int i = start;

    while (p->argv[i] != NULL) {
        args_arr[i - start] = strdup(p->argv[i]);
        i++;
    }
    return args_arr;
}

static char **get_env_arr(t_export *env_list) {
    char **env_arr = (char **)malloc(sizeof(char *) * 256);
    t_export *head = env_list;
    int i = 0;

    while (head != NULL) {
        char *str = NULL;

        str = mx_strjoin(head->name, "=");
        str = mx_strjoin(str, head->value);
        env_arr[i] = strdup(str);
        i++;
        head = head->next;
    }
    env_arr[i] = NULL;
    return env_arr;
}

static void launch_command(t_shell *m_s, t_process *p, t_env_builtin *env, int *exit_code) {
    char **args_arr = get_args(p, env->n_options + env->n_variables + 1);
    char **env_arr = get_env_arr(env->env_list);
        
    if (!env->path)
        env->path = getenv("PATH");
    p->argv = args_arr;
    *exit_code = mx_launch_bin(m_s, p, env->path, env_arr);
}

static t_env_builtin *init_env (t_process *p) {
    t_env_builtin *env = (t_env_builtin *)malloc(sizeof(t_env_builtin));

    env->env_params = NULL;
    env->env_list = NULL;
    env->env_options.i = 0;
    env->env_options.u = 0;
    env->env_options.P = 0;
    env->n_options = count_options(p->argv, env);
    env->n_variables = count_variables(p->argv, env->n_options);
    env->n_args = count_args(p->argv, env->n_variables + env->n_options);
    env->path = NULL;
    return env;
}
