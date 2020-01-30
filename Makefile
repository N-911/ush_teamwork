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
    mx_get_type.c \
    mx_launch_bin.c \
    mx_check_parce_errors.c \
    mx_count_char_arr.c \
    mx_count_chr_quote.c \
    mx_count_options.c \
    mx_get_char_index_quote.c \
    mx_init_shell.c \
    mx_jobs_functions.c \
    mx_jobs_functions_wait.c \
    mx_jobs_functions_print.c \
    mx_launch_job.c \
    mx_launch_process.c \
    mx_loop.c \
    mx_normalization.c \
    mx_parce_tokens.c \
    mx_print_colored.c \
    mx_print_strarr_in_line.c \
    mx_push_export.c \
    mx_read_line.c \
    mx_set_buff_zero.c \
    mx_set_data.c \
    mx_set_variable.c \
    mx_signal.c \
    mx_split_line.c \
    mx_strdup_arr.c \
    mx_strjoin_free.c \
    mx_strlen_arr.c \
    mx_strsplit_quote.c \
    mx_strtok.c \
    mx_strtrim_quote.c \
    mx_terminal.c \
    mx_ush_clear_list.c \
    mx_ush_parsed_input.c \
    mx_ush_push_back.c \
    mx_ush_read_line.c \
    ush_main.c

OBJ = mx_builtin_commands.o \
    mx_cd.o \
    mx_pwd.o \
    mx_export.o \
    mx_unset.o \
    mx_which.o \
    mx_env.o \
    mx_echo.o \
    mx_get_type.o \
    mx_launch_bin.o \
    mx_check_parce_errors.o \
    mx_count_char_arr.o \
    mx_count_chr_quote.o \
    mx_count_options.o \
    mx_get_char_index_quote.o \
    mx_init_shell.o \
    mx_jobs_functions.o \
    mx_jobs_functions_wait.o \
    mx_jobs_functions_print.o \
    mx_launch_job.o \
    mx_launch_process.o \
    mx_loop.o \
    mx_normalization.o \
    mx_parce_tokens.o \
    mx_print_colored.o \
    mx_print_strarr_in_line.o \
    mx_push_export.o \
    mx_read_line.o \
    mx_set_buff_zero.o \
    mx_set_data.o \
    mx_set_variable.o \
    mx_signal.o \
    mx_split_line.o \
    mx_strdup_arr.o \
    mx_strjoin_free.o \
    mx_strlen_arr.o \
    mx_strsplit_quote.o \
    mx_strtok.o \
    mx_strtrim_quote.o \
    mx_terminal.o \
    mx_ush_clear_list.o \
    mx_ush_parsed_input.o \
    mx_ush_push_back.o \
    mx_ush_read_line.o \
    ush_main.o


CFLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic

all: install clean

install:
	@make install -sC libmx
	@cp $(addprefix src/, $(SRC)) .
	@cp $(addprefix inc/, $(INC)) .
	@clang $(CFLAGS) -c $(SRC) -I $(INC)
	@clang $(CFLAGS) libmx/libmx.a $(OBJ) -o $(NAME)
	@mkdir -p obj
	@mv $(OBJ) ./obj

uninstall: clean
	@make uninstall -sC libmx
	@rm -rf $(NAME)

clean:
	@make clean -sC libmx
	@rm -rf $(INC)
	@rm -rf $(SRC)
	@rm -rf $(OBJ)
	@rm -rf ./obj

reinstall: uninstall install
