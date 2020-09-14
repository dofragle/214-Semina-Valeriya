#include <stdio.h>

int main () {
    int n;
    printf("enter the amount of numbers \n");
    scanf("%d", &n);

    int i, a[n];
    for(i = 0 ; i < n; i++) {                  // adding
        scanf("%d", &a[i]);
    }

    for(int i=1;i<n;i++)                      // insertion sort
	for(int j=i;j>0 && a[j-1]>a[j];j--)
			a[j-1] ^= a[j] ^= a[j-1] ^= a[j]; // a XOR b XOR a = b

    for(i = 0 ; i < n; i++) {                 // printing
        printf("%d ", a[i]);
    }

    return 0;
}
