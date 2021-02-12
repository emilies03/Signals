// Group 12
// Emilie Stone
// Cameron Gibb
// Charlie Hardwick

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <string.h>
#include<math.h>
#include <stdbool.h>
#include <fftw3.h>
#include "filter_window.c"
#include "convolution.c"
#include "xor.c"

#define filter_order 40
#define beta 4.54

int kaiser_and_hilbert(int, double*, float);
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

	FILE *fin1, *fin2, *fout;
	float modulated_signal[2048];
	double modulated_signal_in[2088] = {0};
	int bit_count = 0;
	int sample_bits[8];
	float modulated_signal_imaginary[2048] = {0};
	float previous_imaginary[2048] = {0};
	float tail[filter_order] = {0};
	int iterations = 0;
	int prs_signal[128] = {0};
	prs_code = (uint64_t *) malloc(sizeof(uint64_t)*2);

	float real_detect_in[2048] = {0};
	float imag_detect_in[2048] = {0};

	float prev_real_in[2048] = {0};
	float real_in[2048] = {0};
	
	double *fft_windowed_filter_coefficients, *fft_modulated_signal_segment, *fft_result, *inverse_fft_result;

	fft_windowed_filter_coefficients = fftw_malloc((2088)*sizeof(double));
	fft_modulated_signal_segment = fftw_malloc((2088)*sizeof(double));
	fft_result = fftw_malloc((2088)*sizeof(double));
	inverse_fft_result = fftw_malloc((2088)*sizeof(double));

	double windowed_filter_coefficients[2088] = {0};
	kaiser_and_hilbert(filter_order, windowed_filter_coefficients, beta);

	signal_plan = fftw_plan_r2r_1d(2088, modulated_signal_in, fft_modulated_signal_segment, FFTW_R2HC, FFTW_ESTIMATE);
	filter_plan = fftw_plan_r2r_1d(2088, windowed_filter_coefficients, fft_windowed_filter_coefficients, FFTW_R2HC, FFTW_ESTIMATE);
	inverse_plan = fftw_plan_r2r_1d(2088, fft_result, inverse_fft_result, FFTW_HC2R, FFTW_ESTIMATE);
	
	// fft transfor of filter variables
	fftw_execute(filter_plan);

	// open and read the PRS file
	fin2=fopen(argv[2],"rb");
	if(fin2 == NULL) 
	{
		printf("ERROR: prs signal %s does not exist\n", argv[2]);
		exit(1);
	}
	while (fread(prs_code, sizeof(uint64_t), 2, fin2));
	fclose(fin2);

	// open and read the modulated signal
	fin1=fopen(argv[1],"rb");
	if(fin1 == NULL) 
	{
		printf("ERROR: modulated signal %s does not exist\n", argv[1]);
		exit(1);
	}
	
	// open the output file to write to
	fout=fopen(argv[3],"w+b");
	if(fout == NULL) 
	{
		printf("ERROR: output file %s cannot be created\n", argv[3]);
		exit(1);
	}

	while(fread(modulated_signal, sizeof(float), 2048, fin1))
	{	
		// padding signal with 40 trailing 0s for convolution
		for(int i=0; i<2048; i++)
		{
			modulated_signal_in[i] = modulated_signal[i];
		}

		// fft transform modulated signal
		fftw_execute(signal_plan);

		// multiply fft transformed signals
		fft_result[0] = fft_modulated_signal_segment[0]*fft_windowed_filter_coefficients[0];
		for(int i=1;i<(2088+1)/2;i++) 
		{
			fft_result[i] = fft_modulated_signal_segment[i]*fft_windowed_filter_coefficients[i]-fft_modulated_signal_segment[2088-i]*fft_windowed_filter_coefficients[2088-i]; //real
			fft_result[2088-i] = fft_modulated_signal_segment[2088-i]*fft_windowed_filter_coefficients[i]+fft_modulated_signal_segment[i]*fft_windowed_filter_coefficients[2088-i]; //imag
		}	
		// not necessary for even filter order
		if(2088 % 2 == 0)
		{
			fft_result[2088/2] = fft_modulated_signal_segment[2088/2]*fft_windowed_filter_coefficients[2088/2];
		}
		

		// transform result back to time domain
		fftw_execute(inverse_plan);

		// need to divide inverse fft by 2088
		// add tail and save new tail
		for(int i=0; i<filter_order; i++)
		{
			inverse_fft_result[i] = (inverse_fft_result[i]/2088)+ tail[i];
			tail[i] = inverse_fft_result[i+2048]/2088;
		}

		// set previous and current imaginary
    	for(int i=0; i<filter_order; i++)
		{
			previous_imaginary[i] = modulated_signal_imaginary[i];
			modulated_signal_imaginary[i] = inverse_fft_result[i];
		} 
    	for(int i=filter_order; i<2048; i++)
		{
			previous_imaginary[i] = modulated_signal_imaginary[i];
			modulated_signal_imaginary[i] = inverse_fft_result[i]/2088;
		} 

		// take last 2002 and first 36
		for(int i=0; i<2028-16; i++)
		{
			imag_detect_in[i] = previous_imaginary[i+20+16];
			real_detect_in[i] = prev_real_in[i+16];
		}
		for(int i=0; i<20+16; i++)
		{
			imag_detect_in[i+2028-16] = modulated_signal_imaginary[i];
			real_detect_in[i+2028-16] = real_in[i];
		}
		
		iterations += 1;
		if(iterations>1)
		{	

			get_phase(real_detect_in, imag_detect_in, prs_signal);
			
			xored_output = xor(prs_code[1],prs_code[0],prs_signal);
			sample_bits[bit_count] = xored_output;
			bit_count++;


			signed total_bits_value = 0;
			// when you have 8 bits, covert to number through 2s complement
			if (bit_count > 7) 
			{	
				// if first bit is negative, inverse the bits, convert to int and add 1 then make negative 
				if (sample_bits[0]==1)
				{
					for (int i=1; i<8; i++)
					{
						if(sample_bits[i] ==0)
						{
							sample_bits[i]=1;
						}
						else
						{
							sample_bits[i]=0;
						}

					}
				}
				for (int i=1; i<8; i++)
				{
					total_bits_value = total_bits_value +(sample_bits[i]*pow(2,(7-i)));
				}
				
				if (sample_bits[0]==1)
				{
					total_bits_value += 1;
					total_bits_value = -total_bits_value;
				}
								
				bit_count =0;

				// write to file
				total_bits_value = (signed char)total_bits_value;
				fwrite(&total_bits_value, sizeof(signed char), 1, fout);
				total_bits_value = (signed)total_bits_value;
				total_bits_value = 0;
			}
		
		}

		for(int i=0; i<2048; i++)
		{
			prev_real_in[i] = real_in[i];
			real_in[i] = modulated_signal[i];
		}
	}

	fclose(fin1);
	fclose(fout);

	printf("Output has been written to %s successfully \n", argv[3]);

	exit(0);	

}