#include"kernel/types.h"
#include"kernel/stat.h"
#include"user/user.h"
#include"kernel/fs.h"

#define FILSIZ 100

char str[FILSIZ+1];

int matchhere(char*,char*);
int matchstar(int,char*,char*);

int 
match(char* re,char *text)
{
    if(re[0] == '^')
        return matchhere(re+1,text);
    do{
        if(matchhere(re,text))
            return 1;
    }while(*text++ != '\0');

    return 0;
}

int 
matchhere(char *re,char *text){
    if(re[0] == '\0')
        return 1;
    if(re[1] == '*')
        return matchstar(re[0],re+2,text);
    if(re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if(*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re+1,text+1);
    return 0;
}

int 
matchstar(int c,char *re,char *text){
    do{
        if(matchhere(re,text))
            return 1;
    }while(*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}


void 
fmtname(char *path){
    char *p;
    for(p = path+strlen(path);p>=path && *p != '/';p--)
        ;
    p++;
    if(strlen(p) >= FILSIZ)
    {
        printf("find: filename too long %s\n",p);
        return;
    }
    
    if(strcmp(str,p) == 0)
        printf("%s\n",path);
   
}


void 
findindir(char *path){
    char buf[512],*p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path,0)) < 0)
    {
        fprintf(2,"find: cannot open %s\n",path);
        return;
    }
    if(fstat(fd,&st)<0)
    {
        fprintf(2,"find: cannot stat %s\n",path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_FILE:
        fmtname(path);
        break;
    case T_DIR:
        if(strlen(path) + 1 + DIRSIZ +1 > sizeof (buf))
        {
            printf("find: path too long %s\n",path);
            break;
        }
        strcpy(buf,path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd,&de,sizeof de) == sizeof(de))
        {
            if(de.inum == 0)
                continue;
            memmove(p,de.name,DIRSIZ);
            p[DIRSIZ] = 0;
            if(stat(buf,&st)<0)
            {
                printf("find: cannot find %s\n");
                continue;
            }
            if(strcmp(".",p) != 0&& strcmp("..",p) != 0){
                p[DIRSIZ] = '/';
                findindir(buf);
            }
        }
        break;
    }
    close(fd);
}

int 
main(int argc,char *argv[])
{
    if(argc != 3)
    {
        fprintf(2,"Usage: find directory filename...\n");
        exit(1);
    }
    memmove(str,argv[2],sizeof(argv[2]));
    findindir(argv[1]);
    exit(0);
}

