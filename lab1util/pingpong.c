#include"kernel/types.h"
#include"user/user.h"
int pipeping[2];
int pipepong[2];
int main(int argc,char *argv[]){
    pipe(pipeping);
    pipe(pipepong);
    int cpid = fork();
    char tmp;
    if(cpid<0){
        fprintf(2,"fork failed\n");
        exit(1);
    }else if(cpid == 0){
        close(pipeping[1]);
        close(pipepong[0]);
        read(pipeping[0],&tmp,sizeof tmp);
        // close(pipeping[1]);
        printf("%d: received ping\n",getpid());
        write(pipepong[1],&tmp,sizeof tmp);
        // close(pipepong[0]);
        

    }else{
        close(pipeping[0]);
        close(pipepong[1]);
        write(pipeping[1],&tmp,sizeof tmp);
        // close(pipeping[1]);
        read(pipepong[0],&tmp,sizeof tmp);
        // close(pipepong[1]);
        printf("%d: received pong\n",getpid());        
        wait(0);
    }
    exit(0);
}