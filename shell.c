#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h> 

#define ARRSIZE 40
#define COUNT 40
#define RESET "\033[0m"
#define PURPLE "\033[0;35m"

char * word = NULL;
char * word_per1 = NULL;
char **str = NULL;
char c;
int fl_end=0, fl_s=0; 
int fl_per=0, fl_per1=0, fl_lp=0;
int len=0, len_word=0, n_commands=0;
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
    while((c!=EOF) && (c!='\n') && (c!=' ') && (c!='<') &&(c!='>') &&(c!='|'))
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

    if(c=='|') fl_lp=1;
    if (len > 0) word[len] = '\0';
    if (c==EOF) fl_end = 1;
    if (c=='\n') fl_s = 1;
    if(fl_end==0 && fl_s==0 && fl_per==0 && fl_lp==0) fl_biNexWord = 1; else fl_biNexWord = 0;
}
void del(void)
{
    for(int i=0; i<=len_word; i++)
    {
        free(str[i]);
    }
    free(str);
    free(word);
}

void ch_inout(void)
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


    if(fl_per1==3)
    {
        f = open(word_per1, O_RDONLY);
        if (f==-1) perror(word_per1);
        dup2 (f,0);
		close (f);
    }
    else if(fl_per1==2)
    {
        f = open(word_per1, O_CREAT | O_WRONLY | O_TRUNC, 0666);
        if (f==-1) perror(word_per1);
        dup2 (f,1);
		close (f);
    }
    else if(fl_per1==1)
    {
        f = open(word_per1, O_WRONLY | O_APPEND | O_CREAT, 0666);
        if (f==-1) perror(word_per1);
        dup2 (f,1);
		close (f);
    }
}




/*int PipeN(char **words, int j)
{
    int fd[2], i=0, a=0, b=0, pid;
    char **mas=NULL;

    while (words[i]!=NULL)
    {
        if (strcmp(words[i],"|")==0){
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=NULL;
            if (pipe(fd)<0)
            {
                perror("Pipe's error");
                exit(1);
            }      
            if ((pid=fork())==0)
            {
                if ((i+1)!=j) dup2(fd[1],1);
                close(fd[0]);
                close(fd[1]);
                execvp(mas[0],mas);
                perror("Execvp error");
                return 4;    
            }
            else if (pid<0){
                perror("Fork error");
                return 5;
            }  
            dup2(fd[0],0);
            close(fd[0]);
            close(fd[1]);
            for (b=0;b<a;b++)
            {
                free(mas[b]);
                mas[b]=NULL;
            } 
            free(mas);
            mas=NULL;
            a=0; 
            i++;  
        }
        else
        {
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=strdup(words[i]);
            a++;  
            i++;  
        }     
    }
    if (mas==NULL)
    {
        fprintf(stderr,"Error!!!\n");
    }
    else
    { 
        if ((pid=fork())==0)
        {
            mas=realloc(mas,sizeof(char**)*(a+1));
            mas[a]=NULL;
            execvp(mas[0],mas);
            perror("Execvp error");
            return 4;    
        }
        else if (pid<0)
        {
            perror("Fork error");
            return 5;
        } 
        else 
            wait(NULL); 
    }
    while (wait(NULL)!=-1);
    for (b=0;b<a;b++)
    {
        free(mas[b]);
        mas[b]=NULL;
    } 
    free(mas);
    return 0;
}*/



int main()
{
    char **konv_str = NULL;
    int len_konv_str;
    for(;;)
    {
        len_konv_str = 0;
        printdir();
        len_word=0;
        word = malloc(sizeof(char)*ARRSIZE);
        str = malloc(sizeof(char*)*COUNT);
        do
        {
            fl_per=0; fl_per1=0;
            readword();
            str[len_word]=strdup(word);
            len_word++;
            if(len_word >= COUNT) str=realloc(str,sizeof(char*)*(COUNT+len));
        } while((fl_end == 0) && (fl_s == 0) && (fl_per==0) && (fl_lp==0));
        str[len_word]=NULL;

        if(fl_per!=0)
        {
          fl_per1=fl_per;
          readword();
          word_per1 = strdup(word);
          while(c!='\n')
          {
              readword();
              if(fl_per1==fl_per || (fl_per==1 && fl_per1==2) ||(fl_per==2 && fl_per1==1))
              {
                  free(word_per1);
                  word_per1 = strdup(word);
                  fl_per1=fl_per;
              }
          }
        }

        if(fl_lp==1)
        {
            n_commands=2;
            char s1[3], s2[2], s3[2], lp[2];
            s1[0] = '>', s1[1] = '>', s1[2] = '\0';
            s2[0] = '>', s2[1] = '\0';
            s3[0] = '<', s3[1] = '\0';
            lp[0] = '|', lp[1] = '\0';
            konv_str = malloc(sizeof(char*)*COUNT);
            for(int i=0; i<len_word; i++)
            {
                len_konv_str++;
                konv_str[i] = strdup(str[i]);
                if(i >= COUNT) konv_str=realloc(konv_str,sizeof(char*)*(COUNT+i));
            }
            if(fl_per!=0)
            {
                if(fl_per==1) konv_str[len_konv_str]=strdup(s1);
                else if(fl_per==2) konv_str[len_konv_str]=strdup(s2);
                else if(fl_per==3) konv_str[len_konv_str]=strdup(s3);

                len_konv_str++;
                konv_str[len_konv_str]=strdup(word);
                if(len_konv_str >= COUNT) konv_str=realloc(konv_str,sizeof(char*)*(COUNT+len_konv_str));
                len_konv_str++;
            }
            konv_str[len_konv_str] = strdup(lp);
            len_konv_str++;

            while(c!=EOF && c!='\n')
            {
                fl_per = 0; fl_lp=0;
                readword();
                konv_str[len_konv_str] = strdup(word);
                len_konv_str++;
                if(fl_per!=0)
                {
                    if(fl_per==1) konv_str[len_konv_str]=strdup(s1);
                    else if(fl_per==2) konv_str[len_konv_str]=strdup(s2);
                    else if(fl_per==3) konv_str[len_konv_str]=strdup(s3);
                    len_konv_str++;
                }
                if(len_konv_str >= COUNT) konv_str=realloc(konv_str,sizeof(char*)*(COUNT+len_konv_str));
                if (fl_lp==1)
                {
                    n_commands++;
                    konv_str[len_konv_str]=strdup(lp);
                    len_konv_str++;
                    if(len_konv_str >= COUNT) konv_str=realloc(konv_str,sizeof(char*)*(COUNT+len_konv_str));
                }
            }
            konv_str[len_konv_str]=NULL;


            if (fork()==0)
            {
                /*PipeN(konv_str, len_konv_str);*/
                for(int i=0; i<len_konv_str; i++)
                    free(konv_str[i]);
                free(konv_str);
                exit(0);
            }
            else
                wait(0);

            for(int i=0; i<len_konv_str; i++)
                free(konv_str[i]);
            free(konv_str);
        }


        if (len_word!=0 && len_konv_str==0)
        {
            if (strcmp(str[0], "exit") == 0) 
            {
                del();
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
                    ch_inout();
                    execvp(str[0], str);
                    perror("execvp's error");
                    exit(0);
                }
                    else 
                    wait(0);
            }
            if(fl_end==1) 
            {
                exit(0);
                del();
            }
        }
        if(fl_per1!=0) free(word_per1);
        del();
    }
}
