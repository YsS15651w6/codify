#include <string.h>

int run_cli(int argc, char **argv);
int run_gui(int argc, char **argv);

int main(int argc, char **argv)
{
    if (argc > 1 && strcmp(argv[1], "cli") == 0) {
        return run_cli(argc - 1, argv + 1);
    }
    return run_gui(argc, argv);
}
