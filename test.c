#include <stdio.h>
#include <stdlib.h>

int main(){
    int i = 0;
    char *s = malloc(10);
    free(s);
    return 0;
}
