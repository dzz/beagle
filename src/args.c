#include "args.h"
#include <string.h>
int arg_pos = 1;

int next_arg(int argc, char ** argv, char **flag, char **arg) {
    if (arg_pos == argc)
        return 0;
    *flag = argv[arg_pos++];
    if (strlen(argv[arg_pos]) > 1 && *argv[arg_pos] == '-' && *argv[arg_pos+1] != '-')
        return 1;
    if (arg_pos == argc)
        return 0;
    *arg = argv[arg_pos++];
    return 1;
}
