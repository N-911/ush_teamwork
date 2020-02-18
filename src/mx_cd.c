#include "ush.h"

static char *replace_sub(char *str, char *sub, char *replace);
static int count_args(char **args, int n_options);
static void change_dir(char *point, cd_t cd_options, t_shell *m_s, int *exit_code);
static char *chpwd(char **args, int n_options, t_shell *m_s);
static void fill_options(int n_options, cd_t *cd_options, char **args);
static int check_path(char *point, cd_t cd_options);
static char *go_home();
static char *go_back();
static char *go_somewere(t_process *p, int n_options);
static void print_error_cd(char *point);
static void manage_env(char *dir, t_shell *m_s,  cd_t cd_options, int *exit_code);

int mx_cd(t_shell *m_s, t_process *p) {
	cd_t cd_options = {0, 0, 0};
	int n_options = mx_count_options(p->argv, "sLP", "cd", " [-s] [-L|-P] [dir]");
	int n_args = count_args(p->argv, n_options);
	char *point = NULL;
	int exit_code = 1;

	fill_options(n_options, &cd_options, p->argv);
	if (n_options >= 0 && n_args < 3) {
		if (n_args == 0)
			point = go_home();
		else if (n_args == 2)
			point = chpwd(p->argv, n_options, m_s);
		else
			point = go_somewere(p, n_options);
		if (point)
			change_dir(point, cd_options, m_s, &exit_code);
		free(point);
	}
	return exit_code;
}

static char *go_somewere(t_process *p, int n_options) {
	char *point = NULL;

	if (strcmp(p->argv[n_options + 1], "-") == 0)
		point = go_back();
	else
		point = strdup(p->argv[n_options + 1]);
	return point;
}

static char *go_back() {
	char *point = NULL;

	if (getenv("OLDPWD"))
		point = strdup(getenv("OLDPWD"));
	else
		mx_printerr("ush: cd: OLDPWD not set\n");
	return point;
}

static char *go_home() {
	char *point = NULL;

	if (getenv("HOME"))
		point = strdup(getenv("HOME"));
	else
		mx_printerr("ush: cd: HOME not set\n");
	return point;
}


static void change_dir(char *point, cd_t cd_options, t_shell *m_s, int *exit_code) {
	char *dir = mx_normalization(point, m_s->pwd);
	int flag = check_path(point, cd_options);

	if (!flag) {
		if (chdir(dir) != 0)
			print_error_cd(point);
		else
			manage_env(dir, m_s, cd_options, exit_code);
	}
	free(dir);
	free(m_s->git);
	m_s->git = mx_get_git_info();
}

static void manage_env(char *dir, t_shell *m_s,  cd_t cd_options, int *exit_code) {
	char *link = malloc(1024);

	readlink(dir, link, 1024);
	if (cd_options.P == 1 && strcmp(link, "") != 0) {
		free(dir);
		dir = getcwd(NULL, 1024);
	}
	free(link);
	setenv("OLDPWD", m_s->pwd, 1);
	mx_set_variable(m_s->variables, "OLDPWD", m_s->pwd);
	mx_set_variable(m_s->exported, "OLDPWD", m_s->pwd);
	setenv("PWD", dir, 1);
	mx_set_variable(m_s->variables, "PWD", dir);
	mx_set_variable(m_s->exported, "PWD", dir);
	free(m_s->pwd);
	m_s->pwd = strdup(dir);
	free(dir);
	(*exit_code) = 0;
}

static void print_error_cd(char *point) {
	mx_printerr("ush: cd: ");
	perror(point);
}

static char *chpwd(char **args, int n_options, t_shell *m_s) {
	char *current = strdup(m_s->pwd);
	char *new = NULL;

	if (!mx_strstr(current, args[n_options + 1])) {
		mx_printerr("ush: cd: string not in pwd: ");
		mx_printerr(args[n_options + 1]);
		mx_printerr("\n");
	}
	else {
		new = replace_sub(current, args[n_options + 1], args[n_options + 2]);
	}
	free(current);
	return new;
}

static char *replace_sub(char *str, char *sub, char *replace) {
	char *result;
	int index = mx_get_substr_index(str,sub);
	char *buff_1 = strndup(str, index);
	char *buff_2;

    for (int i = 0; i < index + mx_strlen(sub); i++) {
        str++;
    }
    buff_2 = strdup(str);
    result = mx_strjoin(buff_1, replace);
    char *tmp = mx_strjoin(result,buff_2);
    free(result);
    result = strdup(tmp);
    free(tmp);
    free(buff_1);
    free(buff_2);
    return(result);
}

static void fill_options(int n_options, cd_t *cd_options, char **args) {
	for(int i = n_options; i > 0; i --) {
		for (int j = mx_strlen(args[i]); j > 0; j--) {
			if (args[i][j] == 'L' && cd_options->P <= 0) cd_options->L = 1;
			if (args[i][j] == 'P' && cd_options->L <= 0) cd_options->P = 1;
			if (args[i][j] == 's') cd_options->s = 1;
		}
	}
}

static int count_args(char **args, int n_options) {
	int n_args = 0;

	for (int i = n_options + 1; args[i] != NULL; i++) {
		n_args++;
	}
	if (n_args > 2)
		mx_printerr("ush: cd: too many arguments\n");
	return n_args;
}

static int check_path(char *point, cd_t cd_options) {
	int flag = 0;
	char *read_link = realpath(point, NULL);

	if (cd_options.s) {
		if(read_link && strcmp(point, read_link) != 0){
			mx_printerr("ush: cd: ");
			mx_printerr(point);
			mx_printerr(": Not a directory");
			mx_printerr("\n");
			flag++;
		}
	}
	free(read_link);
	return flag;
}
