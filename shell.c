#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#define ARRSIZE 40
#define COUNT 4
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

char * word = NULL;
char **str = NULL;
char c;
int fl_end=0, fl_s=0;
int len=0, len_word=0;

void printdir(void)
{
    char* path = NULL;
    char buf[PATH_MAX];
    if((path = getcwd(buf, PATH_MAX)) != NULL)
    {
        printf("%s%s %s", PURPLE, buf, RESET);
    }
    else perror("error");
}

void readword(void)
{
    len=0; fl_end=0; fl_s=0;
    while(((c=getchar())==' ') || (c=='\n'))
    {
        if(c=='\n') printdir();
    }
    while((c!=EOF) && (c!='\n') && (c!=' '))
    {
        if(c=='"')
        {
            while((c=getchar())!='"')
            {
                word[len]=c;
                len++;
                if(len >= ARRSIZE) word=realloc(word, ARRSIZE + len);
            }
            c = getchar();
        }
        else
        {
            word[len]=c;
            len++;
            if(len >= ARRSIZE) word=realloc(word, ARRSIZE + len);
            c = getchar();
        }
    } 
    if (len > 0) word[len] = '\0';
    if (c==EOF) fl_end = 1;
    if (c=='\n') fl_s = 1;
}

int main()
{
    int i;
    for(;;)
    {
    printdir();
    len_word=0;
    word = malloc(sizeof(char)*ARRSIZE);
    str = malloc(sizeof(char*)*COUNT);
    do
    {
            readword();
            str[len_word]=strdup(word);
            len_word++;
            if(len_word >= COUNT) str=realloc(str,sizeof(char*)*(COUNT+len));
        } while((fl_end == 0) && (fl_s == 0));



        str[len_word]=NULL;
        if (len_word!=0)
        {
            if (strcmp(str[0], "exit") == 0) 
            {
                for(i=0; i<=len_word; i++)
                {
                    free(str[i]);
                }
                free(word);
                exit(0);
            }
            else if (strcmp(str[0], "cd") == 0)
            {
                if(str[1] == NULL)
	            {
		            if(chdir(getenv("HOME")) == -1)
		            {
	    	            printf("error\n");
			            exit(1);
		            }
	            } 
                else
	            {
		            if(chdir(str[1]) == -1)
		            {
			            perror(str[1]);
		            }
	            }
            }
            else 
            {
                if(fork() == 0)
                {
                    execvp(str[0], str);
                    perror("execvp's error");
                    exit(0);
                }
                else 
                    wait(0);
            }
            if(fl_end==1) exit(0);
        }
        for(i=0; i<=len_word; i++)
        {
            free(str[i]);
        }
        free(word);
    }

}
