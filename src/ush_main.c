#include "ush.h"

int main(int argc, char **argv) {
  // Load config files, if any.
    (void)argc;
    (void)argv;

    //extern char **environ;
    t_shell *m_s = mx_init_shell(argc, argv);
  // Run command loop.
    mx_ush_loop(m_s);
  // Perform any shutdown/cleanup.
    system ("leaks -q pathfinder");
    return EXIT_SUCCESS;
}
