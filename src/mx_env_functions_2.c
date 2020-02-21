#include "ush.h"

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
        mx_push_export(env_params, str_option, param);
        free(param);
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
        param = mx_strdup_from(args[*i], j);
    }
    else if (args[*i + 1]) {
        param = strdup(args[*i + 1]);
        (*n_options)++;
        (*i)++;
    }
    return param;
}

static int add_option(char **args, int *i, int *n_options, t_env_builtin *env) {
    int exit_code = 0;
    char option;

    for (int j = 1; j <= mx_strlen(args[*i]); j++) {
        option = args[*i][j];
        if (args[*i][j] == 'u' || args[*i][j] == 'P') {
            char *param = get_parameter(args, i, j, n_options);
            exit_code = add_param(param, &env->env_params, option);

            //free(param);
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

int mx_count_env_options(char **args, t_env_builtin *env) {
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


