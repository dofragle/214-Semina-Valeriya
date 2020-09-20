#include <stdio.h>
#include <stdlib.h>

char *rmas = NULL, *mmas = NULL;
int main() {
	int size=0, rlen = 0, mlen = 0, i;
	char ch;

	while ((ch = getchar())!= EOF){
		if (ch == '\n') {
			 if (mlen < rlen){
				 mmas = malloc(0) ;
				 mmas = malloc(rlen);
				 mlen = rlen;
				 rlen = 0;
				 for (i=0; i<mlen; i++){
					 mmas[i]=rmas[i];
				 }
			 }
		}
			else {
				if(rlen==size){
					size = 2*size+1;
					rmas = realloc(rmas, size);
					if (rmas == NULL){
						fprintf(stderr, "error");
						return 1;
					}
				}
				rmas[rlen]=ch;
				rlen++;
		  }
	}
	for (i=0; i<(mlen); i++){
		putchar(mmas[i]);
	}
	return 0;
}
