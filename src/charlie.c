//to compile: gcc -o charlie charlie.c
//to run: ./charlie


#include<stdio.h>
#include<stdlib.h>

#include<math.h>

int* imp;
double* response;
double* w_response;

double lowpass(double n, double theta_0);
double hamming(double n, int N);

int main(int argc, char** argv){
    int n, i, N; 
    double ans;

    n=40;
    imp = malloc(n);
    response = malloc(n);
    w_response = malloc(n);

    // Get the elements of the array 
    for (i = 0; i < n; ++i) { 
        imp[i] = i+1; 
    }; 

    // Apply lowpass filter to each of the n value in impulse response (imp)
    for (i = 0; i < n; ++i) {
        response[i] = lowpass(imp[i],(M_PI/4)); 
    };

    // Apply hamming window to each of the n values from lowpass filter response
    for (i = 0; i < n; ++i) {
        w_response[i] = hamming(imp[i],40) * response[i]; 
    };

    // Print the elements of the array 
    printf("The elements of the array are: "); 
    for (i = 0; i < n; ++i) { 
        printf("%g, ", w_response[i]);
    };

    return 0;

}

double lowpass(double n, double theta_0){
    double ans;
    ans = theta_0/M_PI * sin(n*theta_0)/(n*theta_0);
    return ans;
}

double hamming(double n, int N){
    double ans;
    ans = 0.54 - (0.46*cos((2*M_PI*n)/N));
    return ans;
}