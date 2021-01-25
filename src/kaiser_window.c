#include<stdio.h>
#include<stdlib.h>
#include<math.h>

float bessel();
int factorial();

int kaiser_window(int filter_order, float kaiser_filter_coefficients[filter_order+1], float beta){
    
    // use kaiser window definition from MATLAB online rather than definition in handouts

    int n;
    double x;

    //printf("finding kaiser window coefficients\n");
    float denominator = bessel(beta);
    for(int i=0; i<=filter_order; i++){
        n = i - filter_order/2;
        x = beta * sqrt(1-pow((double)n/20,2));
        float numerator = bessel(x);
        kaiser_filter_coefficients[i] = numerator/denominator;
    }
    return 1;
}

float bessel(double x)
{
    /*  the zeroth order modified bessel function of the first kind 
    using 0<=k<=10  */
    float sum=0; 
    double numerator;
    double factorial_k = 1;

    for(int k=1; k <=10; k++){
        factorial_k = factorial(k);
        numerator = pow((x/2.0),k);
        sum += pow((numerator/factorial_k),2);
    }

    return sum + 1;
}

int factorial(int x)
{
    double factorial = 1;

    for(int i=1; i<=x; i++){
        factorial *= i;
    }
    return factorial;
}