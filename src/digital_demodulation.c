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
const int original_signal_length = 98304016;
int load_files();

float* modulated_diff;
float* modulated_sum;


int main(){

	modulated_diff = (float*) malloc(sizeof(float)*98304016);
	modulated_sum = (float*) malloc(sizeof(float)*98304016);

	printf("Loading modulated sum\n");
	load_files(modulated_sum, 1);

	printf("Loading modulated diff\n");
	load_files(modulated_diff, 0);

/*	printf("Modulated sum is: \n"); 
    for (int i = 0; i < 30; ++i) { 
        printf("%.30f, \n", modulated_sum[98304016-i]);
    };
	printf("Modulated diff is: \n"); 
    for (int i = 0; i < 30; ++i) { 
        printf("%.30f, \n", modulated_diff[98304016-i]);
    };
*/
	printf("Get hilbert filter coefficients\n");
	float hilbert_filter_coefficients[filter_order+1];
	hilbert_filter(filter_order, hilbert_filter_coefficients);

/*	printf("Cam coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", hilbert_filter_coefficients[i]);
    printf("\n");
	}
*/
	printf("Get Kaiser windowing coefficients\n");
	float kaiser_filter_coefficients[filter_order+1];
	kaiser_window(filter_order, kaiser_filter_coefficients);

/*	printf("Charlie coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", kaiser_filter_coefficients[i]);
    printf("\n");
	}
*/
	printf("Apply kaiser windowing to hilbert filter coefficients\n");
	float windowed_filter_coefficients[filter_order+1];
	for(int i=0; i<= filter_order; i++)
	{
		windowed_filter_coefficients[i] = kaiser_filter_coefficients[i] * hilbert_filter_coefficients[i];
	};	

/*	printf("Charlie-cam coefficients\n");
	for(int i=0; i<= filter_order; i++){
	printf("%f,", windowed_filter_coefficients[i]);
    printf("\n");
	}
*/

	float convolution_output[16+filter_order];
	float modulated_sum_segment[16];
	float tail[40] ={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	float modulated_sum_imaginary[16];

	for(int i=0;i<16;i++){
		modulated_sum_segment[i] = modulated_sum[i];
	}

	convolution(filter_order+1, 16, windowed_filter_coefficients,
		modulated_sum_segment, convolution_output);	

	for(int i=0; i<40; i++){
		convolution_output[i] = convolution_output[i] + tail[i];
	}

//    for (int i = 0; i < 16+filter_order; i++) { 
//        printf("%.30f, \n", convolution_output[i]);
//    };

	for(int i=20; i<36; i++){
		modulated_sum_imaginary[i] = convolution_output[i] + tail[i-20];
	}
	for(int i=16; i<56; i++){
		tail[i] = convolution_output[i];
	}

//    for (int i = 0; i < 16+filter_order; i++) { 
//        printf("%.30f, \n", convolution_output[i]);
//    };

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