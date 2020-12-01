#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include "hilbert_filter.c"
#include "kaiser_window.c"
#include "convolution.c"

int hilbert_filter();
int convolution();
int block_convolution();
int phase_detection();
int get_phase();
const float beta = 4.54;
const int filter_order = 40;

int main(int argc, char *argv[]){
	FILE *fin1, *fout;
	float modulated_signal[2048];
	int bit_count = 0;
	int sample_bits[8];

	printf("Get hilbert filter coefficients\n");
	float hilbert_filter_coefficients[filter_order+1];
	hilbert_filter(filter_order, hilbert_filter_coefficients);

	printf("Get Kaiser windowing coefficients\n");
	float kaiser_filter_coefficients[filter_order+1];
	kaiser_window(filter_order, kaiser_filter_coefficients, beta);

	printf("Apply kaiser windowing to hilbert filter coefficients\n");
	float windowed_filter_coefficients[filter_order+1];
	for(int i=0; i<= filter_order; i++)
	{
		windowed_filter_coefficients[i] = kaiser_filter_coefficients[i] * hilbert_filter_coefficients[i];
	};	

	printf("Opening modulated signal\n");

	fin1=fopen(argv[1],"rb");
	if(fin1 == NULL) {
		printf("ERROR: modulated signal does not exist\n");
		exit(1);
	}
	printf("Opened modulated signal successfully\n");	

	printf("Opening output file\n");	
	fout=fopen("data/Output","w+b");
	if(fout == NULL) {
		printf("ERROR: output file cannot be created\n");
		exit(1);
	}
	printf("Opened output file successfully\n");	

	while(fread(modulated_signal, sizeof(float), 2048, fin1))
	{

		int required_blocks = ceil( ((filter_order/2)+16)/16 );
		float next_modulated_signal_segment[16] = {0};
		float modulated_signal_segment[required_blocks*16] = {0};

		float modulated_signal_block[16] = {0};
		float modulated_signal_imaginary[16] = {0};
		float tail[40] = {0};
		float modulated_signal_phase[16] = {0};
		float previous_imaginary_block[16] = {0};
		float previous_phase[16] = {0};
		int iterations = 0;
		int result;
		int prs_signal[128];

		for(int j=0; j<128; j++)
		{

			
			for(int i=0;i<16;i++)
			{
				previous_imaginary_block[i] = modulated_signal_imaginary[i];
				modulated_signal_segment[i] = modulated_signal_segment[i+16];
				modulated_signal_segment[i+16] = next_modulated_signal_segment[i];

				//this line should be replaced with the new input
				modulated_signal_block[i] = modulated_signal[j*16+i];
			} 


			get_phase(required_blocks, modulated_signal_segment, next_modulated_signal_segment, modulated_signal_imaginary, tail,
						modulated_signal_phase, previous_imaginary_block, previous_phase, iterations, windowed_filter_coefficients,
						filter_order);
			iterations += 1;	
		
		}

		// pass prs_signal to charlies function here!!!
		
		int bit = 1;
		int total_bits_value = 0;
		sample_bits[bit_count] = bit;
		bit_count++;
		if (bit_count > 7) 
		{


			// convert to signed int
			// is this the right sig bit first?
			for (int i=0; i<7; i++)
			{
				total_bits_value = total_bits_value +(sample_bits[i]*pow(2,i));
			}
			if (sample_bits[7]==1)
			{
				total_bits_value = -total_bits_value;
			}
			bit_count =0;

		/////////////// !!!!!!!!!! UPSAMPLE/DOWNSAMPLE !!!!!!!!!!

			// write to file
			total_bits_value = (signed char)total_bits_value;
			fwrite(&total_bits_value, sizeof(signed char), 1, fout);
		}
	
	}

	fclose(fin1);
	fclose(fout);

	exit(0);
}