#include "ush.h"

static char *strdup_from(char *str, int index);

t_export *mx_set_variables() {
    t_export *variables = NULL;;
    extern char** environ;

    for (size_t i = 0; environ[i] != NULL; i++) {
        int idx = mx_get_char_index(environ[i],'=');
        char *name = strndup(environ[i],idx);
        char *value = strdup_from(environ[i],idx);
            
        mx_push_export(&variables, name, value);
    }
    mx_push_export(&variables, "?", "0");
    return variables;
}

t_export *mx_set_export() {
    t_export *export = NULL;;
    extern char** environ;

    for (size_t i = 0; environ[i] != NULL; i++) {
        int idx = mx_get_char_index(environ[i],'=');
        char *name = strndup(environ[i],idx);
        char *value = strdup_from(environ[i],idx);
            
        mx_push_export(&export, name, value);   
    }
    return export;
}

static char *strdup_from(char *str, int index) {
    for (int i = 0; i <= index; i++) {
        str++;
    }
    return strdup(str);
}

