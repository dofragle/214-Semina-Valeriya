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
    int f;
    f = open(argv[1], O_RDWR)
    if (f==-1) perror(argv[1]);

    int tmp_c;
    off_t pos = 0;

    while((read(f, &tmp_c, sizeof(char))) > 0)
    {
        if(c='\n')
        {
            pos = lseek
        }
    }

    return 0;
}
