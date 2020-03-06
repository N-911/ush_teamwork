NAME = ush

INC = ush.h

SRC = mx_builtin_commands.c \
    mx_cd.c\
    mx_pwd.c \
    mx_export.c \
    mx_unset.c \
    mx_which.c \
    mx_env.c \
    mx_echo.c \
    mx_fg.c \
    mx_bg.c \
    mx_jobs.c \
    mx_kill.c \
    mx_set_parametr.c \
    mx_nbr_to_hex.c \
    mx_hex_to_nbr.c \
    mx_get_type.c \
    mx_launch_bin.c \
    mx_launch_builtin.c \
    mx_count_options.c \
    mx_create_job.c \
    mx_init_shell2.c \
    mx_init_shell.c \
    mx_jobs_stack.c \
    mx_jobs_stack2.c \
    mx_jobs_functions.c \
    mx_jobs_functions_2.c \
    mx_jobs_functions_wait.c \
    mx_jobs_functions_print.c \
    mx_launch_job.c \
    mx_launch_job2.c \
    mx_launch_process.c \
    mx_loop.c \
    mx_normalization.c \
    mx_push_export.c \
    mx_push_process_back.c \
    mx_set.c \
    mx_set_data.c \
    mx_set_variable.c \
    mx_signal.c \
    mx_terminal.c \
    mx_exit.c \
    mx_clear_all.c \
    mx_cd_functions.c \
    mx_cd_functions_1.c \
    mx_export_functions.c \
    mx_strdup_from.c \
    mx_env_functions.c \
    mx_env_functions_1.c \
    mx_env_functions_2.c \
    mx_echo_functions.c \
    mx_which_functions.c \
    mx_get_git_info.c \
    mx_get_keys.c \
    mx_prompt.c \
    mx_edit_command.c \
    mx_exec_signal.c \
    mx_add_option.c \
    mx_get_line.c \
    mx_chdir.c \
    mx_true.c \
    mx_run_subshell.c \
    mx_set_redirects.c \
    mx_set_redirects_in.c \
    ush_main.c

SRC_PARSER = libmx1.c \
    libmx2.c \
    mx_ast_clear_list.c \
    mx_ast_creation.c \
    mx_ast_parse.c \
    mx_ast_push_back.c \
    mx_check_parce_errors.c \
    mx_filters.c \
    mx_get_aliases.c \
    mx_get_functions.c \
    mx_get_token_and_delim.c \
    mx_parce_tokens.c \
    mx_print_errors.c \
    mx_strtok.c \
    mx_quote_manage.c \
    mx_redir_push_back.c \
    mx_subst_command.c \
    mx_subst_dollar.c \
    mx_subst_tilde.c \
    mx_subst_tilde_add_login.c \
    mx_ush_parsed_line.c \
    mx_ush_read_line.c \

OBJ = mx_builtin_commands.o \
    mx_cd.o \
    mx_pwd.o \
    mx_export.o \
    mx_unset.o \
    mx_which.o \
    mx_env.o \
    mx_echo.o \
    mx_fg.o \
    mx_bg.o \
    mx_jobs.o \
    mx_kill.o \
    mx_set_parametr.o \
    mx_nbr_to_hex.o \
    mx_hex_to_nbr.o \
    mx_get_type.o \
    mx_launch_bin.o \
    mx_launch_builtin.o \
    mx_count_options.o \
    mx_create_job.o \
    mx_init_shell2.o \
    mx_init_shell.o \
    mx_jobs_stack.o \
    mx_jobs_stack2.o \
    mx_jobs_functions.o \
    mx_jobs_functions_2.o \
    mx_jobs_functions_wait.o \
    mx_jobs_functions_print.o \
    mx_launch_job.o \
    mx_launch_job2.o \
    mx_launch_process.o \
    mx_loop.o \
    mx_normalization.o \
    mx_push_export.o \
    mx_push_process_back.o \
    mx_set.o \
    mx_set_data.o \
    mx_set_variable.o \
    mx_signal.o \
    mx_terminal.o \
    mx_exit.o \
    mx_clear_all.o \
    mx_cd_functions.o \
    mx_cd_functions_1.o \
    mx_export_functions.o \
    mx_strdup_from.o \
    mx_env_functions.o \
    mx_env_functions_1.o \
    mx_env_functions_2.o \
    mx_echo_functions.o \
    mx_which_functions.o \
    mx_get_git_info.o \
    mx_get_keys.o \
    mx_prompt.o \
    mx_edit_command.o \
    mx_exec_signal.o \
    mx_add_option.o \
    mx_get_line.o \
    mx_chdir.o \
    mx_true.o \
    mx_run_subshell.o \
    mx_set_redirects.o \
    mx_set_redirects_in.o \
    ush_main.o

OBJ_PARSER = libmx1.o \
    libmx2.o \
    mx_ast_clear_list.o \
    mx_ast_creation.o \
    mx_ast_parse.o \
    mx_ast_push_back.o \
    mx_check_parce_errors.o \
    mx_filters.o \
    mx_get_aliases.o \
    mx_get_functions.o \
    mx_get_token_and_delim.o \
    mx_parce_tokens.o \
    mx_print_errors.o \
    mx_quote_manage.o \
    mx_redir_push_back.o \
    mx_strtok.o \
    mx_subst_command.o \
    mx_subst_dollar.o \
    mx_subst_tilde.o \
    mx_subst_tilde_add_login.o \
    mx_ush_parsed_line.o \
    mx_ush_read_line.o \

CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install clean

install:
	#@make install -C libmx
	@cp $(addprefix src/, $(SRC)) .
	@cp $(addprefix inc/, $(INC)) .
	@cp $(addprefix src/ast/, $(SRC_PARSER)) .
	@clang $(CFLAGS) -c $(SRC) $(SRC_PARSER) -I $(INC)
	@clang $(CFLAGS) libmx/libmx.a $(OBJ) $(OBJ_PARSER) -o $(NAME)
	@mkdir -p obj
	@mv $(OBJ) $(OBJ_PARSER) ./obj

uninstall: clean
	#@make uninstall -C libmx
	@rm -rf $(NAME)

clean:
	#@make clean -C libmx
	@rm -rf $(INC)
	@rm -rf $(SRC) $(SRC_PARSER)
	@rm -rf $(OBJ) $(OBJ_PARSER)
	@rm -rf ./obj

reinstall: uninstall install
