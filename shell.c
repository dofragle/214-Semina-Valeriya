#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>

#define RESET "\033[0m"
#define PURPLE "\033[0;35m"
#define ARRSIZE 40
#define COUNT 4

char *word = NULL;
char **str = NULL;
int len=0;
int fl_end = 0;
int fl_s = 0;

//печать директории
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

//чтение слова
void readword(void)
{
    char c;
    len=0;
    while(((c=getchar())==' ') || (c=='\n')){;}
 
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
    printf("%s", word);
} 

int main(int argc, char **argv)
{
    int len_word=0;
    for(;;)
    {
        word = malloc(sizeof(char)*ARRSIZE);
        str = malloc(sizeof(char*)*COUNT);
        readword();
        do
        {
            //добавить слово в массив слов
            str[len_word]=strcpy(str[len_word], word);
            readword();
            len_word++;
            if(len_word >= COUNT) str=realloc(str,sizeof(char*)*(COUNT+len));
        } while((fl_end == 0) && (fl_s == 0));
        if (len_word!=0)
        {
            printf("%s", str[0]);
            if (strcmp(str[0], "exit") == 0) 
            {
                //нужно почистить динамическую память
                free(str); //я могу так чистить массив слов? 
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
                }
                else 
                    wait(0);
            }
            free(str);
            free(word);
            if(fl_end==1) exit(0);
        }
    }
}