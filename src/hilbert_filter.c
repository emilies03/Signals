#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int hilbert_filter(int filter_order, float hilbert_filter_coefficients[filter_order+1]){

    //Sets order of filter
    int numbers[filter_order+1];
    int i;

    //Places zeros in even slots and calculates Hilbert coefficient in odd slots
    for(i=0; i<= filter_order; i++){

        numbers[i] = i - filter_order/2;

        if(numbers[i] % 2 == 0){
            hilbert_filter_coefficients[i] = 0;
        }
        else{
            hilbert_filter_coefficients[i] = 2/(numbers[i]*M_PI);
        }

    }

    return 1;

}