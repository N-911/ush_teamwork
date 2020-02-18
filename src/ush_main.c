#include "ush.h"

int main(int argc, char **argv) {
  // Load config files, if any.
    (void)argc;
    (void)argv;

    t_shell *m_s = mx_init_shell(argc, argv);
    mx_ush_loop(m_s);
    // mx_clear_all(m_s);
    return EXIT_SUCCESS;
}

