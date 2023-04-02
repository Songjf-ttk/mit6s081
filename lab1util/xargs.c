#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

char *
getline(int fd)
{
    static char buf[MAXARG];
    char *p = buf;
    while (1)
    {
        int num = read(fd, p, sizeof(char));
        if (num <= 0)
            return 0;
        if (*p == '\n')
        {
            break;
        }
        p++;
    }
    *p = 0;
    return buf;
}

int main(int argc, char *argv[])
{
    if (argc < 1)
    {
        fprintf(2, "xargs: missing operand\n");
        exit(1);
    }

    for (;;)
    {
        char *p = getline(0);
        if (p == 0)
            break;
        int cpid = fork();
        if (cpid == 0)
        {
            char *buf[MAXARG];
            for (int i = 0; i + 1 < argc; ++i)
            {
                buf[i] = argv[i + 1];
            }
            buf[argc - 1] = p;
            buf[argc] = 0;
            // for (int i = 0; i < argc + 1; ++i)
            //     printf("%s ", argv[i]);
            // printf("\n");

            exec(buf[0], buf);
            fprintf(2, "exec %s failed\n", buf[1]);
            exit(1);
        }
        else
            wait(0);
    }
    exit(0);
}