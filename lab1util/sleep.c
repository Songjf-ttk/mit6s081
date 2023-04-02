#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char *argv[])
{
    if (argc <= 1 || argc > 2)
    {
        fprintf(2, "usage: sleep pattern [time]\n");
        exit(1);
    }
    int tick = atoi(argv[1]);
    sleep(tick);
    exit(0);
}