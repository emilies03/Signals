#include<stdio.h>
#include<stdlib.h>
#include<math.h>

int kaiser_window(int filter_order, float kaiser_filter_coefficients[filter_order+1])
{
    int i, n;

    for(i=0; i<=filter_order; i++){
        n = i - filter_order/2;
        kaiser_filter_coefficients[i] = 0.54 + (0.46*cos((2*M_PI*n)/filter_order));
    }
    return 1;

}