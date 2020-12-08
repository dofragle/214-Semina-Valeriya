#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <limits.h>
#include <fcntl.h> 



int main(int argc, char **argv)
{
    int fd[2];
    pipe(fd);
    int f;
    
    if(fork() == 0)
    {
        f = open(argv[4], O_RDONLY);
        if(f==-1) perror(argv[4]);
        dup2 (f, 0);
        dup2(fd[1], 1);
        
        close(f);
        close(fd[0]);
        close(fd[1]);

        execlp(argv[3], argv[3], NULL);
        exit(1);
    }

    if(fork() == 0)
    {
        dup2(fd[0], 0);
        
        close(fd[0]);
        close(fd[1]);

        if(!fork())
        {
            execlp(argv[1], argv[1], NULL);
            exit(4);
        }

        int status;
        wait(&status);
        if(WEXITSTATUS(status))
        {
            if (WEXITSTATUS(status)!=0)
            {
                if(!fork())
                {
                    execlp(argv[2], argv[2], NULL);
                    exit(3);
                }
            } 
        }
        wait(0);
        exit(2);
    }
 
    close(fd[0]);
    close(fd[1]);
    while(wait(NULL) > 0);
    return 0;
}
