#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int main(){

    //Sets order of filter
    int filter_order = 40;
    int numbers[filter_order+1];
    int i;

    //This array contains the final hilbert filter coefficients
    float hilbert_filter_coefficients[filter_order+1];
    
 

    //Loop puts order numbers in each array slot
    for(i=0; i<=filter_order; i++){
        numbers[i] = i - filter_order/2;
    }

    //Places zeros in even slots and calculates Hilbert coefficient in odd slots
    for(i=0; i<= filter_order; i++){
        if(numbers[i] % 2 == 0){
            hilbert_filter_coefficients[i] = 0;
        }
        else{
            hilbert_filter_coefficients[i] = 2/(numbers[i]*M_PI);
        }

        //uncomment to print filter coefficients to terminal

        // printf("%f", hilbert_filter_coefficients[i]);
        // printf("\n");
    }

    exit(0);

}