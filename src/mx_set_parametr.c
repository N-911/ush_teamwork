#include "ush.h"

static void get_data (char *arg, char **name, char **value, t_export *variables);
static char *strdup_from(char *str, int index);
static void export_value(t_export *export, char *name, char *value);

int mx_set_parametr(char **args, t_shell *m_s) {
    char *name;
    char *value;
                
    get_data(args[0], &name, &value, m_s->variables);
    if (value != NULL) {
        mx_set_variable(m_s->variables, name, value);
        export_value(m_s->exported, name, value);
    }
    return 0;   
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
    if (value == NULL) {
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

static char *strdup_from(char *str, int index) {
    for (int i = 0; i <= index; i++) {
        str++;
    }
    return strdup(str);
}

static void export_value(t_export *export, char *name, char *value) {
    t_export *head = export;

    while (head != NULL) {
        if (strcmp(head->name, name) == 0) {
            head->value = value;
            setenv(name, value, 1);
            break;
        }
        head = head->next;
    }
}

