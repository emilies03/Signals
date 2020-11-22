#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "hilbert_filter.c"
#include "kaiser_window.c"
#include "convolution.c"

int hilbert_filter();
int kaiser_window();
int convolution();
const int filter_order = 40;
int load_files();

float* modulated_diff;
float* modulated_sum;


int main(){

	float sig1[4] = {1.1,2.2,3.3,4.4};
	float sig2[12] = {17.2,19.54,0.8,1.1,0.96,1,1,1,1,1,1,1};
	convolution(4,12,sig1,sig2);

/*
	modulated_diff = (float*) malloc(sizeof(float)*98304016);
	modulated_sum = (float*) malloc(sizeof(float)*98304016);

	printf("Loading modulated sum\n");
	load_files(modulated_sum, 1);

	printf("Loading modulated diff\n");
	load_files(modulated_diff, 0);

	printf("Modulated sum is: \n"); 
    for (int i = 0; i < 30; ++i) { 
        printf("%.30f, \n", modulated_sum[98304016-i]);
    };
	printf("Modulated diff is: \n"); 
    for (int i = 0; i < 30; ++i) { 
        printf("%.30f, \n", modulated_diff[98304016-i]);
    };

	float hilbert_filter_coefficients[filter_order+1];
	hilbert_filter(filter_order, hilbert_filter_coefficients);

	printf("Cam coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", hilbert_filter_coefficients[i]);
    printf("\n");
	}

	float kaiser_filter_coefficients[filter_order+1];
	kaiser_window(filter_order, kaiser_filter_coefficients);

	printf("Charlie coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", kaiser_filter_coefficients[i]);
    printf("\n");
	}

	float windowed_filter_coefficients[filter_order+1];
	for(int i=0; i<= filter_order; i++)
	{
		windowed_filter_coefficients[i] = kaiser_filter_coefficients[i] * hilbert_filter_coefficients[i];
	};	
*/
/*	printf("Charlie-cam coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", windowed_filter_coefficients[i]);
    printf("\n");
	}
*/
	// compare phase of consectutive bits

	// inverse XOR

	// group 8 bits into value 

	exit(0); // all is good
}


int load_files(float* modulated_array, int load_choice)
{
	FILE *fin1;
	if(load_choice==1)
	{
		fin1=fopen("data/modulated_sum.dat","rb");
		printf("Opening modulated_sum\n");
		if(fin1 == NULL) {
			printf("ERROR: modulated_sum does not exist\n");
			exit(1);
		}
		printf("Open modulated_sum successfully\n");
	}
	else
	{
		// open input files as binary read-only
		fin1=fopen("data/modulated_diff.dat","rb");
		printf("Opening modulated_diff\n");
		if(fin1 == NULL) {
			printf("ERROR: modulated_diff does not exist\n");
			exit(1);
		}
		printf("Opened modulated_diff successfully\n");
	}
	
	// reading 98304016 floats in file
	fread(modulated_array, sizeof(float), 98304016, fin1);

	printf("Modulated signal loading complete\n");

	// close the files
	fclose(fin1);

	return 1;

}