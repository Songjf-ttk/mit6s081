#include"kernel/types.h"
#include"user/user.h"
void transmitright(int *pipefd){
    int tmp = 0;
    close(pipefd[1]);
    int num = read(pipefd[0],&tmp,sizeof(int));
    if(num == 0) exit(0);
    int prime = tmp;
    printf("prime %d\n",prime);
    int pipeprime[2];
    pipe(pipeprime);
    int cpid = fork();
    if(cpid<0){
        fprintf(2,"fork failed\n");
        exit(1);
    }else if(cpid == 0){
        transmitright(pipeprime);
        close(pipeprime[0]);
        close(pipefd[0]);
        exit(0);
    }else{
        close(pipeprime[0]);
        while(read(pipefd[0],&tmp,sizeof(int))>0){
            int num = tmp;
            if(num%prime!=0){
                write(pipeprime[1],&tmp,sizeof(int));
            }
        }
        //close(pipefd[0]);
        close(pipeprime[1]);
    }
    wait(0);
}
int main(int argc,char* argv[]){
    int pipefd[2];
    pipe(pipefd);
    int cpid = fork();
    int tmp = -1;
    if(cpid<0){
        fprintf(2,"fork failed\n");
        exit(1);
    }else if(cpid == 0){
        transmitright(pipefd);
    }else{
        close(pipefd[0]);
        for(int i = 2;i<=35;i++){
            tmp = i;
            write(pipefd[1],&tmp,sizeof(int));
        }
        close(pipefd[1]);
        wait(0);
    }
    exit(0);
}