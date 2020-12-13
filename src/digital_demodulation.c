#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include <stdbool.h>
#include "hilbert_filter.c"
#include "kaiser_window.c"
#include "convolution.c"
#include "xor.c"

int hilbert_filter();
int convolution();
int block_convolution();
int phase_detection();
int get_phase();
int load_prs();
const float beta = 4.54;
const int filter_order = 40;
const int original_signal_length = 98304016;
uint64_t * prs_code;
int xored_output;

//new branch

int main(int argc, char *argv[])
{
	FILE *fin1, *fin2, *fout;
	float modulated_signal[2048];
	int bit_count = 0;
	int sample_bits[8];
	float elements = (filter_order/2)+16;
	int required_blocks = ceil( elements/16 );
	float modulated_signal_segment[required_blocks*16];
	float modulated_signal_imaginary[16] = {0};
	float tail[filter_order];
	float modulated_signal_phase[16] = {0};
	float previous_imaginary_block[16] = {0};
	float previous_phase[16] = {0};
	int iterations = 0;
	int prs_signal[128];
	int previous_prs_signal[128];
	int current_prs_signal[128];
	prs_code = (uint64_t *) malloc(sizeof(uint64_t)*2);
	signed total_bits_value = 0;
	
	
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

	printf("Opening prs signal: %s\n", argv[2]);
	fin2=fopen(argv[2],"rb");
	if(fin2 == NULL) {
		printf("ERROR: prs signal %s does not exist\n", argv[2]);
		exit(1);
	}
	printf("Opened prs signal successfully\n");	
	while (fread(prs_code, sizeof(uint64_t), 2, fin2));
	
	printf("Opening modulated signal: %s\n", argv[1]);

	fin1=fopen(argv[1],"rb");
	if(fin1 == NULL) {
		printf("ERROR: modulated signal %s does not exist\n", argv[1]);
		exit(1);
	}
	printf("Opened modulated signal successfully\n");	

	printf("Opening output %s file\n", argv[3]);	
	fout=fopen(argv[3],"w+b");
	if(fout == NULL) {
		printf("ERROR: output file %s cannot be created\n", argv[3]);
		exit(1);

	}
	printf("Opened output file successfully\n");	

	while(fread(modulated_signal, sizeof(float), 2048, fin1))
	{	
		for (int j=0; j<128; j++)
		{			
			for(int i=0;i<16;i++)
			{
				previous_imaginary_block[i] = modulated_signal_imaginary[i];
				for(int x=0; x<required_blocks-1; x++)
				{
					modulated_signal_segment[i+x*16] = modulated_signal_segment[i+(x+1)*16];
				}
				modulated_signal_segment[i+(required_blocks-1)*16] = modulated_signal[j*16+i];
			} 

			int bit = get_phase(filter_order, required_blocks, modulated_signal_segment, modulated_signal_imaginary, tail,
				modulated_signal_phase, previous_imaginary_block, previous_phase, iterations, windowed_filter_coefficients);	
			

			previous_prs_signal[j] = current_prs_signal[j];
			current_prs_signal[j] = bit;
			
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

			/////////////// !!!!!!!!!! UPSAMPLE/DOWNSAMPLE !!!!!!!!!!
				// write to file
				total_bits_value = (signed char)total_bits_value;

				fwrite(&total_bits_value, sizeof(signed char), 1, fout);
				total_bits_value = (signed)total_bits_value;
				total_bits_value = 0;
			}
		}
	
	}

	fclose(fin1);
	fclose(fout);

	exit(0);	

}