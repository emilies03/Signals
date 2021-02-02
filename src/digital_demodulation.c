#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include <stdbool.h>
#include <time.h>
#include <fftw3.h>
#include "hilbert_filter.c"
#include "kaiser_window.c"
#include "convolution.c"
#include "xor.c"

#define filter_order 40
#define beta 4.54

int hilbert_filter();
int convolution();
int block_convolution();
int phase_detection();
int get_phase();
int load_prs();
uint64_t * prs_code;
int xored_output;

fftw_plan filter_plan, signal_plan, inverse_plan;

int main(int argc, char *argv[])
{	
	// Timing code to remove
	clock_t t;
	t = clock();

	FILE *fin1, *fin2, *fout;
	float modulated_signal[2048];
	double modulated_signal_in[2088] = {0};
	int bit_count = 0;
	int sample_bits[8];
	float elements = (filter_order/2)+16;
	int required_blocks = ceil( elements/16 );
	float modulated_signal_segment[filter_order+1] = {0};
	float modulated_signal_imaginary[16] = {0};
	float tail[filter_order] = {0};
	int iterations = 0;
	int prs_signal[128];
	int previous_prs_signal[128];
	int current_prs_signal[128];
	int current_bit = 0;
	prs_code = (uint64_t *) malloc(sizeof(uint64_t)*2);
	signed total_bits_value = 0;

	double *fft_windowed_filter_coefficients;
	fft_windowed_filter_coefficients = fftw_malloc((2088)*sizeof(double));
	double *fft_modulated_signal_segment;
	fft_modulated_signal_segment = fftw_malloc((2088)*sizeof(double));
	double *fft_result;
	fft_result = fftw_malloc((2088)*sizeof(double));
	double *inverse_fft_result;
	inverse_fft_result = fftw_malloc((2088)*sizeof(double));
	
	//printf("Get hilbert filter coefficients\n");
	float hilbert_filter_coefficients[filter_order+1];
	hilbert_filter(filter_order, hilbert_filter_coefficients);

	//printf("Get Kaiser windowing coefficients\n");
	float kaiser_filter_coefficients[filter_order+1];
	kaiser_window(filter_order, kaiser_filter_coefficients, beta);

	//printf("Apply kaiser windowing to hilbert filter coefficients\n");
	double windowed_filter_coefficients[2088] = {0};

	for(int i=0; i<= filter_order; i++)
	{
		windowed_filter_coefficients[i] = kaiser_filter_coefficients[i] * hilbert_filter_coefficients[i];
	};	

	for(int i=0; i<2088; i++){
		printf("%f\n", windowed_filter_coefficients[i]);
	}

	signal_plan = fftw_plan_r2r_1d(2088, modulated_signal_in, fft_modulated_signal_segment, FFTW_R2HC, FFTW_ESTIMATE);
	filter_plan = fftw_plan_r2r_1d(2088, windowed_filter_coefficients, fft_windowed_filter_coefficients, FFTW_R2HC, FFTW_ESTIMATE);
	inverse_plan = fftw_plan_r2r_1d(2088, fft_result, inverse_fft_result, FFTW_HC2R, FFTW_ESTIMATE);
	fftw_execute(filter_plan);


	//printf("Opening prs signal: %s\n", argv[2]);
	fin2=fopen(argv[2],"rb");
	if(fin2 == NULL) {
		printf("ERROR: prs signal %s does not exist\n", argv[2]);
		exit(1);
	}
	//printf("Opened prs signal successfully\n");	
	while (fread(prs_code, sizeof(uint64_t), 2, fin2));
	fclose(fin2);
	
	//printf("Opening modulated signal: %s\n", argv[1]);

	fin1=fopen(argv[1],"rb");
	if(fin1 == NULL) {
		printf("ERROR: modulated signal %s does not exist\n", argv[1]);
		exit(1);
	}
	//printf("Opened modulated signal successfully\n");	

	//printf("Opening output %s file\n", argv[3]);	
	fout=fopen(argv[3],"w+b");
	if(fout == NULL) {
		printf("ERROR: output file %s cannot be created\n", argv[3]);
		exit(1);
	}
	//printf("Opened output file successfully\n");	

	while(fread(modulated_signal, sizeof(float), 2048, fin1))
	{	
		for(int i=0; i<2048; i++){
			modulated_signal_in[i] = modulated_signal[i];
		}

		for (int j=0; j<128; j++)
		{			
			for(int i=0;i<16;i++)
			{
				modulated_signal_segment[i] = modulated_signal[j*16+i];
			} 

			fftw_execute(signal_plan);

			fft_result[0] = fft_modulated_signal_segment[0]*fft_windowed_filter_coefficients[0];

			for(int i=1;i<(filter_order+1+1)/2;i++) 
			{
				fft_result[i] = fft_modulated_signal_segment[i]*fft_windowed_filter_coefficients[i]-fft_modulated_signal_segment[filter_order+1-i]*fft_windowed_filter_coefficients[filter_order+1-i]; //real
				fft_result[filter_order+1-i] = fft_modulated_signal_segment[filter_order+1-i]*fft_windowed_filter_coefficients[i]+fft_modulated_signal_segment[i]*fft_windowed_filter_coefficients[filter_order+1-i]; //imag
			}	

			// not necessary for even filter order
			if(filter_order+1 % 2 == 0)
			{
				fft_result[(filter_order+1)/2] = fft_modulated_signal_segment[(filter_order+1)/2]*fft_windowed_filter_coefficients[(filter_order+1)/2];
			}
			
			fftw_execute(inverse_plan);

			for(int i=0; i<16; i++){
				modulated_signal_imaginary[i] = inverse_fft_result[i] + tail[i];
			}
			for(int i=0; i<9; i++){
				tail[i] = tail[i+16] +  inverse_fft_result[i+16];
			}
			for(int i=9; i<25; i++){
				tail[i] = inverse_fft_result[i+16];
			}


			//block_convolution(filter_order, next_modulated_signal_segment, windowed_filter_coefficients, tail, modulated_signal_imaginary);

			int bit = get_phase(filter_order, modulated_signal_segment, modulated_signal_imaginary, current_bit);	
			
			current_prs_signal[j] = ~bit;
			
			iterations += 1;
		}

		if(iterations>128)
		{
			for (int k=0; k<128; k++)
			{
				if (k < 128-required_blocks)
				{
					prs_signal[k] = previous_prs_signal[k+required_blocks];
				}
				else
				{
					prs_signal[k] = current_prs_signal[k-(128-required_blocks)];
				}
			}
	
			xored_output = xor(prs_code[1],prs_code[0],prs_signal);
			//fwrite(&xored_output, sizeof(signed char), 1, fout);
			
			sample_bits[bit_count] = xored_output;
			bit_count++;
			total_bits_value = 0;
			uint8_t decimal = 0;
			if (bit_count > 7) 
			{	
				if (sample_bits[0]==1)
				{
					int inversed_bits[8] ={0};
					for (int i=1; i<8; i++)
					{
						decimal = decimal +(sample_bits[i]*pow(2,(7-i)));
					}
					decimal -= 1;
					for (int i=0; i<7; i++)
					{
						if (decimal!=0)
						{
							inversed_bits[7-i] = (decimal-1)%2;
							decimal = decimal/2;
						}
						else
						{
							inversed_bits[7-i]=1;
						}
						 
					}
					for (int i=1; i<8; i++)
					{
						total_bits_value = total_bits_value +(inversed_bits[i]*pow(2,(7-i)));
					}
					total_bits_value = -total_bits_value;
				}
				else
				{
					for (int i=1; i<8; i++)
					{
						total_bits_value = total_bits_value +(sample_bits[i]*pow(2,(7-i)));
					}
				}
								
				bit_count =0;

				// write to file
				total_bits_value = (signed char)total_bits_value;

				fwrite(&total_bits_value, sizeof(signed char), 1, fout);
				total_bits_value = (signed)total_bits_value;
				total_bits_value = 0;
			}
		
		}
	}

	// Timing code to remove 
	t = clock() - t;
	double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("Code took %f seconds to execute \n", time_taken);

	fclose(fin1);
	fclose(fout);

	printf("Output has been written to %s successfully \n", argv[3]);

	exit(0);	

}