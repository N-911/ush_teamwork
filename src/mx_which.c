#include "ush.h"

static void fill_options(int n_options, which_t *which_options, char **args);
static void check_path(char **arr, char *command, t_list **output, int *flag);
static void print_path(t_list *output, int flag, char *command, which_t which_options);
static void check_builtin (char **list, char *command, t_list **output, int *flag);

int mx_which(t_shell *m_s, t_process *p) {
	which_t which_options = {0, 0};
	int n_options = mx_count_options(p->argv, "as", "which", " [-as] program ...");
	int exit_code = 0;
	int flag;

	fill_options(n_options, &which_options, p->argv);
	if (n_options < 0) 
		return 1;
	for (int i = n_options + 1; p->argv[i] != NULL; i++) {
		flag = 0;
		t_list *output = NULL;
		char **arr = mx_strsplit(getenv("PATH"), ':');

		check_builtin(m_s->builtin_list, p->argv[i], &output, &flag);
        check_path(arr, p->argv[i], &output, &flag);
        if (!which_options.s)
        	print_path(output, flag, p->argv[i], which_options);
	}
	if (!flag)
        exit_code = 1;
    return exit_code;
}

static void fill_options(int n_options, which_t *which_options, char **args) {
	for(int i = n_options; i > 0; i --) {
		for (int j = mx_strlen(args[i]); j > 0; j--) {
			if (args[i][j] == 'a') which_options->a = 1;
			if (args[i][j] == 's') which_options->s = 1;
		}
	}
}

static void check_path(char **arr, char *command, t_list **output, int *flag) {
	int i = 0;
    char *name = NULL;

    while (arr[i] != NULL) {
        DIR *dptr  = opendir(arr[i]);
        if (dptr != NULL) {
            struct dirent  *ds;
            while ((ds = readdir(dptr)) != 0) {
                if (strcmp(ds->d_name, command) == 0 && command[0] != '.') {
                    name = mx_strjoin(arr[i], "/");
                    name = mx_strjoin(name, command);
                    mx_push_back(&*output, name);
                    (*flag)++;
                }
            }
        closedir(dptr);
        }
        i++;
    }
}

static void check_builtin (char **list, char *command, t_list **output, int *flag) {
	for (int j = 0; list[j] != NULL; j++) {
		if (strcmp(list[j], command) == 0) {
			char *str = mx_strjoin(command, ": shell built-in command");
			mx_push_back(&*output, str);
			(*flag)++;
		}
	}
}

static void print_path(t_list *output, int flag, char *command, which_t which_options) {
	if (!flag){
		printf("%s not found\n", command);
	}
	else {
		t_list *head = output;

		while (head) {
			printf("%s\n", head->data);
			if (!which_options.a)
				break;
			head = head->next;
		}
	}
}
