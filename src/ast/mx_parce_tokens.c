#include "ush.h"
/*
* malloc and realloc line if nesessary
*/
static char **create_tokens(char **tokens, int *bufsize) {
    if (tokens == NULL) {
        tokens = malloc((*bufsize) * sizeof(char*));
        if (!tokens) {
            mx_printerr_red("ush: allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
    else {
        (*bufsize) += 64;
        tokens = realloc(tokens, (*bufsize) * sizeof(char*));
        if (!tokens) {
            mx_printerr_red("ush: allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
    return tokens;
}
/*
* get array of pointers to separate tokens in line
*/
char **mx_parce_tokens(char *line) {
    int position = 0;
    int bufsize = 64;
    char **tokens = NULL;
    char *token;

    tokens = create_tokens(tokens, &bufsize);  // malloc
    token = mx_strtok(line, USH_TOK_DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize)
            tokens = create_tokens(tokens, &bufsize);  // realloc
        token = mx_strtok(NULL, USH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}
