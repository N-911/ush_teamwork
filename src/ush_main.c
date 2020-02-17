#include "ush.h"

int main(int argc, char **argv) {
    // Load config files, if any.
    (void)argc;
    (void)argv;

    //extern char **environ;
    t_shell *m_s = mx_init_shell(argc, argv);
    // Run command loop.
    mx_ush_loop(m_s);
    // exit works from loop, doees not goes here
    // Perform any shutdown/cleanup.
    return EXIT_SUCCESS;
}
