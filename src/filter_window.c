#include<stdio.h>
#include<stdlib.h>
#include<math.h>

float bessel();
int factorial();

int kaiser_and_hilbert(int filter_order, double windowed_filter_coefficients[2088], float beta){
    
    // use kaiser window definition from MATLAB online rather than definition in handouts
    int n;
    double x, hilbert, kaiser;


    float denominator = bessel(beta);
    for(int i=0; i<=filter_order; i++){

        // Hilbert filter:
        n = i - filter_order/2;
        if(n % 2 == 0)
        {
            hilbert = 0;
        }
        else
        {
            hilbert = 2/(n*M_PI);
        }

        // Kaiser window:
        x = beta * sqrt(1-pow((double)n/20,2));
        float numerator = bessel(x);
        kaiser = numerator/denominator;

        windowed_filter_coefficients[i]= kaiser * hilbert;

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