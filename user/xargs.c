#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    char *args[MAXARG];
    char buf[512];
    int i;

    for (i = 1; i < argc; i++)
    {
        args[i - 1] = argv[i];
    }

    if (fork() == 0)
    {
        read(0, buf, sizeof buf);
        args[i - 1] = buf;
        exec(argv[1], args);
        exit(0);
    }
    else
    {
        wait((int *)0);
        exit(0);
    }

    exit(0);
}