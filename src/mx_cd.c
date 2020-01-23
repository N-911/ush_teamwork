#include "ush.h"

static char *replace_sub(char *str, char *sub, char *replace);
static int count_args(char **args, int n_options);
static void change_dir(char *point, cd_t cd_options, t_shell *m_s);
static char *chpwd(char **args, int n_options, t_shell *m_s);
static void fill_options(int n_options, cd_t *cd_options, char **args);
static int check_path(char *point, cd_t cd_options); 

int mx_cd(t_shell *m_s, t_process *p) {
	cd_t cd_options = {0, 0, 0};
	int n_options = mx_count_options(p->argv, "sLP", "cd", " [-s] [-L|-P] [dir]");
	int n_args = count_args(p->argv, n_options);
	char *point = NULL;

	mx_set_variable(m_s->variables, "?", "1");
	fill_options(n_options, &cd_options, p->argv);
	if (n_options >= 0 && n_args < 3) {
		if (n_args == 0) {
			if (getenv("HOME"))
				point = strdup(getenv("HOME"));
			else
				mx_printerr("ush: cd: HOME not set\n");
		}
		else if (n_args == 2)
			point = chpwd(p->argv, n_options, m_s);
		else {
			if (strcmp(p->argv[n_options + 1], "-") == 0) {
				if (getenv("OLDPWD"))
					point = strdup(getenv("OLDPWD"));
				else
					mx_printerr("ush: cd: OLDPWD not set\n");
			}
			else
				point = strdup(p->argv[n_options + 1]);
		}
		if (point) {
			change_dir(point, cd_options, m_s);
		}
	}
	return 1;
}

static void change_dir(char *point, cd_t cd_options, t_shell *m_s) {
	char *link = malloc(256);
	char *dir = mx_normalization(point, m_s->pwd);
	int flag = check_path(point, cd_options);

	if (!flag) {
		if (chdir(dir) != 0) {
			mx_printerr("ush: ");
			mx_printerr("cd: ");
			perror(point);
		}
		else {
			readlink(dir, link, 256);
			if (cd_options.P == 1 && strcmp(link, "") != 0) {
				free(dir);
				dir = getcwd(NULL, 256);
			}
			free(link);
			setenv("OLDPWD", m_s->pwd, 1);
			setenv("PWD", dir, 1);
			m_s->pwd = dir;
			mx_set_variable(m_s->variables, "?", "0");
		}
	}
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

    for(int i = 0; i < index + mx_strlen(sub); i++){
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

	if (cd_options.s) {
		char *tmp_1 = mx_strjoin(point, "/");
		int idx = 0;

		while (mx_get_char_index(tmp_1, '/') >= 0) {
			idx += mx_get_char_index(tmp_1, '/');
			char *link1 = malloc(256);
			char *tmp_2 = strndup(point, idx);

			readlink(tmp_2, link1, 256);
			idx ++;
			tmp_1 += mx_get_char_index(tmp_1, '/') + 1;
			if (strcmp(link1,"") != 0) {
				mx_printerr("ush: cd: ");
				mx_printerr(point);
				mx_printerr(": Not a directory");
				mx_printerr("\n");
				flag ++;
				break;
			}
		}
	}
	return flag;
}
