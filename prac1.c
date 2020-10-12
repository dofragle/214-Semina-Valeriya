#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ORIG_LENGTH 40

char *w = NULL;//слово из букв и цифр
char w_p[2]; //слово-синтаксис
char c;
int wcount = 0;

//дерево с 4 компонентами
struct tnode {
  char *word;
  int count;
  struct tnode *left;
  struct tnode *right;
};

struct tnode *root1 = NULL;
struct tnode *root2 = NULL;

//добавление узла в дерево №1
struct tnode *addtree1(struct tnode *p, char *w)
{
    int cond;
    if(p == NULL)
    {
        p = (struct tnode *)malloc(sizeof(struct tnode));
        p->word = strdup(w);
        p->count = 1;
        p->left = p->right = NULL;
    }
    else if((cond = strcmp(w, p->word)) == 0)
        p->count++;
    else if(cond < 0)
        p->left = addtree1(p->left, w);
    else
        p->right = addtree1(p->right, w);
  return p;
}
//добавление узла в дерево №2
struct tnode *addtree2(struct tnode *p, char *w, int c)
{
    if (p == NULL)
    {
        p = (struct tnode *)malloc(sizeof(struct tnode));
        p->word = strdup(w);
        p->count = c;
        p->left = p->right = NULL;
    }
    else if(c < p->count)
        p->left = addtree2(p->left, w, c);
    else if(c > p->count)
        p->right = addtree2(p->right, w, c);
    else
    {
       struct tnode*f;
       f = (struct tnode *)malloc(sizeof(struct tnode));
       f->word = strdup(w);
       f->count = c;
       f->left = p->left;
       f->right = NULL;
       p->left = f;
    }
    return p;
}
//копирование дерева №1 в дерево №2
void tree1totree2(struct tnode *p)
{
    if(p != NULL)
    {
        tree1totree2(p->left);
        root2 = addtree2(root2, p->word, p->count);
        tree1totree2(p->right);
    }
}

//удаление дерева
void treedel(struct tnode* p) {
    if (p != NULL){
        treedel(p -> right);
        treedel(p -> left);
        free(p -> word);
        free(p);
    }
}

//печать дерева
void treeprint(struct tnode *p)
{
    if(p != NULL)
    {
        treeprint(p->right);
        printf("%s %d %f\n ", p->word, p->count, (double)p->count/wcount);
        treeprint(p->left);
    }
}


//чтение слов (либо 2, либо 1)
void readword() 
{
    int len = 0;
    while ((c = getchar()) != EOF && !ispunct(c) && !isspace(c))
    {
        w[len] = c;
        len++;
        if (len >= ORIG_LENGTH) w = realloc(w, ORIG_LENGTH + len);
    }
    w[len]='\0';
    if(len!=0)
    {
        wcount++;
        root1 = addtree1(root1, w);
    }
    if (ispunct(c))
    {
        w_p[0] = c;
        w_p[1] = '\0';
        root1 = addtree1(root1, w_p);
        wcount++;
    }
}

int main(int argc, char *argv[])
{
    //формат ввода и вывода
    char *str = "-i";
    if (argc == 3)
    {
        if (!strcmp(argv[1], str))
            freopen(argv[2], "r", stdin);
        else
            freopen(argv[2], "w", stdout);
    }
    else if (argc == 5)
    {
        if (!strcmp(argv[1], str))
        {
            freopen(argv[2], "r", stdin);
            freopen(argv[4], "w", stdout);
        }
        else
        {
            freopen(argv[4], "r", stdin);
            freopen(argv[2], "w", stdout);
        }
    }

    //чтение и добавление слов в дерево №1
    do
    {
        w = malloc(ORIG_LENGTH);
        readword();
        free(w);
    } while (c != EOF);

    //копирование в дерево №2
    tree1totree2(root1);
    //печать
     treeprint(root2);
    //удаление деревьев
    treedel(root1);
    treedel(root2);
    //закрытие файлов
    fclose(stdout); 
    fclose(stdin);
    return 0;
}
