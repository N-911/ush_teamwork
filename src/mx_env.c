#include "ush.h"

static int count_args(char **args, int n_all);
static int count_variables(char **args, int n_options);
static int count_options(char **args, env_t *env_options, t_export **env_params);
static char *strdup_from(char *str, int index);
static void set_data(t_export **export, int n_options, int n_variables, char *args[], env_t *env_options);
static void delete_name(t_export **list, char *arg);

int mx_env(t_shell *m_s, t_process *p) {
    env_t env_options = {0, 0, 0};
    t_export *env_params = NULL;
    int n_options = count_options(p->argv, &env_options, &env_params);
    int n_variables = count_variables(p->argv, n_options);
    int n_args = count_args(p->argv, n_variables + n_options);

  	extern char** environ;
    t_export *env_list = NULL;
    set_data(&env_list, n_options, n_variables, p->argv, &env_options);

    t_export *head_1 = env_params;
    char *path = NULL;

    while (head_1 != NULL) {
        if (strcmp(head_1->name, "u") == 0) {
            delete_name(&env_list, head_1->value);
        }
        if (strcmp(head_1->name, "P") == 0) {
            if (path)
                free(path);
            path = strdup(head_1->value);
        }
        head_1 = head_1->next;
    }

    if (n_options < 0) 
        return 1;
    if (n_args == 0) {
        t_export *head = env_list;

        while (head != NULL) {
            printf("%s=%s\n", head->name, head->value);
            head = head->next;
        }
    } 
    else {
        char **args_arr = (char **)malloc(sizeof(char *) * 256);
        int m = n_options + n_variables + 1;
        int j = 0;
        while (p->argv[m] != NULL) {
            args_arr[j] = strdup(p->argv[m]);
            m++;
            j++;
        }
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
        if (!path)
            path = getenv("PATH");
        p->argv = args_arr;
        mx_launch_bin(m_s, p, path, env_arr);
    }
    mx_set_variable(m_s->variables, "?", "0");
  	return 1;
}

static int count_variables(char **args, int n_options) {
    int n_variables = 0;

    for (int i = n_options + 1; args[i] != NULL; i++) {
        if (mx_get_char_index(args[i],'=') <= 0)
            break;
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

static int count_options(char **args, env_t *env_options, t_export **env_params) {
    int n_options = 0;

    for (int i = 1; args[i] != NULL; i++) {
        if (args[i][0] != '-') break;
        if (strcmp(args[i], "--") == 0) {
            n_options++;
            break;
        }
        for (int j = 1; j <= mx_strlen(args[i]); j++) {
            if (args[i][j] == 'u' || args[i][j] == 'P') {
                char *option = malloc(2);
                option[0] = args[i][j];
                option[1] = '\0';
                char *param = NULL;
                if(args[i][j + 1] != '\0') {
                    param = strdup_from(args[i], j);
                }
                else if (args[i + 1]) {
                    param = strdup(args[i + 1]);
                    n_options++;
                    i++;
                }
                if (param) {
                    if (strcmp(option, "u") == 0 && strchr(param, '=')) {
                        printf("env: unsetenv %s: Invalid argument\n", param);
                        return -1;
                    }
                    mx_push_export(&*env_params, option, param);
                }
                else {
                    printf("env: option requires an argument -- %c\nusage: env [-iv] [-P utilpath] [-u name]\n           [name=value ...] [utility [argument ...]]\n", args[i][j]);
                    return -1;
                }
                break;
            }
            else if (args[i][j] == 'i' || args[i][j] == '-' || mx_strlen(args[i]) == 1) {
                env_options->i = 1;
            }
            else if (j != mx_strlen(args[i])) {
                printf("env: illegal option -- %c\nusage: env [-iv] [-P utilpath] [-u name]\n           [name=value ...] [utility [argument ...]]\n", args[i][j]);
                return -1;
            }
        }
        n_options++;
    }
    return n_options;
}

static char *strdup_from(char *str, int index) {
    for (int i = 0; i <= index; i++) {
        str++;
    }
    return strdup(str);
}

static void set_data(t_export **export, int n_options, int n_variables, char *args[], env_t *env_options) {
    extern char** environ;

    for (int i = 0; environ[i] != NULL; i++) {
        if (!env_options->i) {
            int idx = mx_get_char_index(environ[i],'=');
            char *name = strndup(environ[i],idx);
            char *value = strdup_from(environ[i],idx);
                    
            mx_push_export(&*export, name, value);   
        }
    }

    for (int i = n_options + 1; i <= n_options + n_variables; i++) {
        int idx = mx_get_char_index(args[i],'=');
        char *name = strndup(args[i],idx);
        char *value = strdup_from(args[i],idx);
            
        mx_push_export(&*export, name, value);   
    }
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
                // free(head->next);
                head->next = head->next->next;
                break;
            }
        }
        head = head->next;
    }
}


