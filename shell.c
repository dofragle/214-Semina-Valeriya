#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h> 

#define ARRSIZE 40
#define COUNT 4
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

char * word = NULL;
char **str = NULL;
char c;
int fl_end=0, fl_s=0; 
int fl_per=0;
int len=0, len_word=0;
int fl_biNexWord=0; //флаг начала слова (чтобы не потерять первый символ)

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
    if(fl_biNexWord==0)
    {
        while(((c=getchar())==' ') || (c=='\n'))
        {
            if(c=='\n') printdir();
        }
    }
    while((c!=EOF) && (c!='\n') && (c!=' ') && (c!='<') &&(c!='>'))
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
    while(c==' ')
    {
        c = getchar();
    }
    if(c=='<') fl_per=3;
    if (c=='>')
    {
        if((c=getchar())=='>') fl_per=1; else fl_per=2;
    }
    if (len > 0) word[len] = '\0';
    if (c==EOF) fl_end = 1;
    if (c=='\n') fl_s = 1;
    if(fl_end==0 && fl_s==0 && fl_per==0) fl_biNexWord = 1; else fl_biNexWord = 0;
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
        fl_per=0;
        readword();
        str[len_word]=strdup(word);
        len_word++;
        if(len_word >= COUNT) str=realloc(str,sizeof(char*)*(COUNT+len));
    } while((fl_end == 0) && (fl_s == 0) && (fl_per==0));
    if(fl_per!=0)
    {
        readword();
    }

    str[len_word]=NULL;
    if (len_word!=0)
    {
        if (strcmp(str[0], "exit") == 0) 
        {
            for(i=0; i<=len_word; i++)
            {
                free(str[i]);
            }
	        free(str);
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

        int f;
        if(fl_per==3)
        {
            f = open(word, O_RDONLY);
            if (f==-1) perror(word);
            dup2 (f,0);
			close (f);

        }
        else if(fl_per==2)
        {
            f = open(word, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if (f==-1) perror(word);
            dup2 (f,1);
			close (f);
        }
        else if(fl_per==1)
        {
            f = open(word, O_WRONLY | O_APPEND | O_CREAT, 0666);
            if (f==-1) perror(word);
            dup2 (f,1);
			close (f);
            }


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
    free(str);
    free(word);
    }
}
