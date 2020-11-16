#include<stdio.h>
#include<stdlib.h>

int main(){

    int i;
    int filter_order = 40;
    int numbers[filter_order+1];

    for(i=0; i<=filter_order; i++){
        numbers[i] = i - filter_order/2;
        printf("%d", numbers[i]);
        printf("\n");
    }

    exit(0);

}