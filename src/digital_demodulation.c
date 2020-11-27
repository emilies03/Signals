#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include "hilbert_filter.c"
#include "kaiser_window.c"
#include "convolution.c"

int hilbert_filter();
int kaiser_window();
int convolution();
int block_convolution();
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

	float modulated_sum_segment[16];
	float modulated_sum_imaginary[16];
	float tail[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	float modulated_sum_phase[16];
	float first_real_block[16];
	float second_real_block[16];
	float previous_imaginary_block[16];
	float previous_phase[16];
	int iterations = 0;

	for(int j=0; j<20; j++)
	{

		for(int i=0;i<16;i++){
			previous_imaginary_block[i] = modulated_sum_imaginary[i];
			first_real_block[i] = second_real_block[i];
			second_real_block[i] = modulated_sum_segment[i];

			//this line should be replaced with the new input
			modulated_sum_segment[i] = modulated_sum[j*16+i];
		} 

		block_convolution(modulated_sum_segment, windowed_filter_coefficients, tail, modulated_sum_imaginary, 
		filter_order);

		iterations++;

		if(iterations>3){
			for(int i=0;i<16;i++){
			previous_phase[i] = modulated_sum_phase[i];
			} 
		}

		if(iterations > 2){
			for(int i = 0; i<16; i++){
				if (modulated_sum[i]==0)
				{
					printf("REAL PART IS ZERO!!!!!!!!!!\n");
					modulated_sum_phase[i] = 0;
				}
				else
				{
					for(int x=0; x<12; x++){
						modulated_sum_phase[x] = atan2(previous_imaginary_block[x+4], first_real_block[x]);
					}
					for(int x=12; x<16; x++){
						modulated_sum_phase[x] = atan2(modulated_sum_imaginary[x-12], first_real_block[x]);
					}
				}
				
			}

			/*
			for (int x = 0; x < 16; x++) 
			{ 
				printf("%.30f, %.30f\n", previous_phase[x], modulated_sum_phase[x]);
			}; 
			printf("\n\n");
			*/
				
		}
		
		if(iterations > 3){
			float difference = modulated_sum_phase[0]-previous_phase[0];
			if(difference > -0.5 && difference<0.5){
				printf("0\n");
			}
			if(difference>=0.5 || difference<=-0.5){
				printf("1\n");
			}
		}
		
		
		
	
	}

	/*
	for (int i = 0; i < 48; i++) 
	{ 
		printf("%.30f, \n", modulated_sum_imaginary[i+20]);
	};

	printf("\n\n\n");

	for (int i = 0; i < 48; i++) 
	{ 
		printf("%.30f, \n", modulated_sum[i]);
	}; */

	/*
	for(int i=0; i<320; i++){
		if (modulated_sum[i]==0)
		{
			printf("REAL PART IS ZERO!!!!!!!!!!\n");
			modulated_sum_phase[i] = 0;
		}
		else
		{
			modulated_sum_phase[i] = atan2(modulated_sum_imaginary[i+20], modulated_sum[i]);
		}
	}
	*/

	/*	
	printf("\n");
		for (int i = 0; i<320; i++) { 
			if (i%16==0)
			{printf("\n\n");}
			printf("%.30f, \n", modulated_sum_phase[i]);
		}; */

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