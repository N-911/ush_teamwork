#include "ush.h"
/*
 * Get substitution.
 */
static char *get_subst(char *s, int *len) {
    char *subst = NULL;
    int i = 0;

    if (s[0] == '$' && s[1] == '(') {
        i = mx_get_char_index_quote(s, ")", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[2], i - 2);
        *len = i + 1;
    }
    else if (s[0] == '`') {
        i = mx_get_char_index_quote(&s[1], "`", "\'");
        if (i < 0)
            return NULL;
        subst = mx_strndup(&s[1], i);
        *len = i + 1;
    }
    return subst;
}

char *exec_subshell(char *substr) {
    t_ast **ast = NULL;
    // t_job *new_job = NULL;
    char *argv[] = {"./ush", NULL};
    t_shell *m_s = mx_init_shell(1, argv);
    m_s->variables = NULL;

    if (substr[0] != '\0') {
        if ((ast = mx_ast_creation(substr, m_s))){
            mx_ast_print(ast);

            // launch_blow_job(m_s, ast, new_job);

            // mx_strdel(&substr);
            return "SUBS";
        }
    }
    // mx_strdel(&substr);
    return "FAIL";
}

/*
 * Combine new string.
 */
static char *expantion(char *s, int pos) {
    char *res = NULL;
    int len = 0;
    char *subst;

    res = mx_strndup(s, pos);
    if ((subst = get_subst(&s[pos], &len))) {
        subst = exec_subshell(subst);
        res = mx_strjoin_free(res, subst);
        // mx_strdel(&subst);
    }
    if (s[pos + len + 1])
        res = mx_strjoin_free(res, &s[pos + len + 1]);
    mx_strdel(&s);
    return res;
}
/*
 * Command substitutiont.
 */
char *mx_subst_command(char *s) {
    char *res = s;
    int pos = 0;

    if (!s || !*s)
        return s;
    while (res && (pos = mx_get_char_index_quote(res, "`$", "\'")) >= 0)
        if (!(res = expantion(res, pos))) {
            mx_printerr("u$h: command substitushion doesn't work.\n");
            return NULL;
        }
    return res;
}
