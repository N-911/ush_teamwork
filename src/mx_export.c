#include "ush.h"

static int count_args(char **args, int n_options);
static char *strdup_from(char *str, int index);
static int check_identifier(char *arg);
static void export_value(t_export *export, char *name, char *value);
static void print_export(t_export *export);
static void get_data (char *arg, char **name, 
    char **value, t_export *variables);
static void export_or_error(char *arg, t_export *export, t_export *variables, int *exit_code);

int mx_export(t_shell *m_s, t_process *p) {
    int n_options = mx_count_options(p->argv, "p", "export",
     " [name[=value] ...] or export -p");
    int n_args = count_args(p->argv, n_options);
    int i = 0;
    int exit_code = 0;

    if (n_options <  0)
        return 1;
    if (n_args == 1) 
        print_export(m_s->exported);
    else {
        i = n_options + 1;
        while (p->argv[i] != NULL) {
            export_or_error(p->argv[i], m_s->exported, m_s->variables, &exit_code);
            i++;
        }
    }
  	return exit_code;
}

static char *strdup_from(char *str, int index) {
    for (int i = 0; i <= index; i++) {
        str++;
    }
    return strdup(str);
}

static int count_args(char **args, int n_options) {
    int n_args = 0;

    for (int i = n_options; args[i] != NULL; i++) {
        n_args++;
    }
    return n_args;
}

static int check_identifier(char *arg) {
    int flag = 0;
    int name_len = mx_get_char_index(arg, '=');

    if (name_len == 0) {
        flag++;
    }
    for (int j = 0; j < name_len; j++) {
        if (!isalpha(arg[j]) && !isdigit(arg[j]) && arg[j] != '_') {
            flag++;
        }
    }
    return flag;
}

static void export_value(t_export *export, char *name, char *value) {
    int flag = 0;
    t_export *head = export;

    while (head != NULL) {
        if (strcmp(head->name, name) == 0) {
            flag++;
            head->value = value;
            break;
        }
        head = head->next;
    }
    if (!flag) {
        mx_push_export(&export, name, value);
    }
}

static void print_export(t_export *export) {
    t_export *head = export;
        
    while (head != NULL) {
        if (strncmp(head->name,"BASH_FUNC_",10) != 0) {
            printf("export %s", head->name);
            if (head->value)
                printf("=\"%s\"", head->value);
            printf("\n");
        }
        head = head->next;
    }
}

static void get_data (char *arg, char **name, char **value, t_export *variables) {
    int idx = mx_get_char_index(arg,'=');

    if (idx < 0) {
        *name = strdup(arg);
        *value = NULL;
    } 
    else {
        *name = strndup(arg,idx);
        *value = strdup_from(arg,idx);
    }
    if (*value == NULL) {
        t_export *head = variables;
        while (head != NULL) {
            if (strcmp(head->name, *name) == 0) {
                *value = head->value;
                break;
            }
            head = head->next;
        }
    }
}

static void export_or_error(char *arg, t_export *export, t_export *variables, int *exit_code) {
    int flag = check_identifier(arg);

    if (flag) {
        mx_printerr("ush: export: `");
        mx_printerr(arg);
        mx_printerr("': not a valid identifier\n");
        *exit_code = 1;
    }
    else {
        char *name;
        char *value;
                
        get_data(arg, &name, &value, variables);
        if (value != NULL) {
            setenv(name, value, 1);
        }
        export_value(export, name, value);
        export_value(variables, name, value);
        *exit_code = 0;
    }
}




