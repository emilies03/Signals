#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

int convolution(int filter_order, float signal_1[filter_order+1], float signal_2[filter_order+1])
{
    int i, j;
    float sum;
    float reversed_signal_1[filter_order+1];

    float convolution[2*filter_order+1];

    for (i = 0; i <= (filter_order+1)/2; i++) {
        reversed_signal_1[i] = signal_1[filter_order-i];
        reversed_signal_1[filter_order-i] = signal_1[i];
    }

    for (i=0; i<(2*filter_order)+1; i++)
    {
        sum =0;
        for (j=0; j<=i; j++)
        {
            printf("conv:");
            sum += reversed_signal_1[j]*signal_2[filter_order-j];
            printf("%f, \n", reversed_signal_1[j]);
            printf("%f, \n", signal_2[filter_order-j]);
            printf("%f, \n", sum);
        }
        convolution[i] = sum;
    }

    printf("convolution: \n"); 
    for (int i = 0; i < 2*filter_order+1; ++i) { 
        printf("%f, \n", convolution[i]);
    };
return 1;
}