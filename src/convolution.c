#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>


int convolution(int length_signal_1, int length_signal_2, float signal_1[length_signal_1], float signal_2[length_signal_2], float output[length_signal_1+length_signal_2-1])
{
    int i, j, track;
    float sum;

    for(i=0; i<length_signal_1+length_signal_2-1; i++){
        sum = 0;
        track = i;
        for(j=0; j<length_signal_2; j++)
		{
            if(track>=0 && track<length_signal_1)
			{
            sum += signal_1[track]*signal_2[j];
            }
            track -= 1;
        }
        output[i] = sum;
    }

return 1;
}


int block_convolution(int filter_order, float modulated_signal_segment[16], float windowed_filter_coefficients[filter_order+1], 
    float tail[filter_order], float modulated_signal_imaginary[16])
{
    float convolution_output[filter_order+16];
    convolution(filter_order+1, 16, windowed_filter_coefficients, modulated_signal_segment, convolution_output);

    for(int i=0; i<filter_order; i++){
			convolution_output[i] = convolution_output[i] + tail[i];
		}

    for(int i=0; i<16; i++){
			modulated_signal_imaginary[i] = convolution_output[i];
		} 

    for(int i=0; i<filter_order; i++){
			tail[i] = convolution_output[i+16];
		}
    
    return 1;
}


int phase_detection(float modulated_signal_phase[16], int current_bit)
{   
    for(int i=0; i<16; i++){
		if(modulated_signal_phase[i] < 0.6){
			current_bit = ~current_bit;
		}
    }

	return current_bit;
}


int get_phase(int filter_order, int required_blocks, float modulated_signal_segment[required_blocks*16],
	float modulated_signal_imaginary[16], float tail[filter_order],	float modulated_signal_phase[16],
	float previous_imaginary_block[16], float previous_phase[16], int iterations, float windowed_filter_coefficients[filter_order+1], int current_bit)
{   
		float next_modulated_signal_segment[16];
		for(int i=0; i<16; i++){
			next_modulated_signal_segment[i] = modulated_signal_segment[i+(required_blocks-1)*16];
		}

		int imaginary_split = (filter_order/2)%16;

		block_convolution(filter_order, next_modulated_signal_segment, windowed_filter_coefficients, tail, modulated_signal_imaginary);
		if(iterations > required_blocks-2)
		{
			for(int i = 0; i<16; i++)
			{
				if (modulated_signal_segment[i]==0)
				{
					modulated_signal_phase[i] = 0;
				}
				else
				{
					for(int x=0; x<16-imaginary_split; x++){
						modulated_signal_phase[x] = sqrt(previous_imaginary_block[x+imaginary_split]*previous_imaginary_block[x+imaginary_split] + modulated_signal_segment[x]*modulated_signal_segment[x]);
					}
					for(int x=16-imaginary_split; x<16; x++){
						modulated_signal_phase[x] = sqrt(modulated_signal_imaginary[x-(16-imaginary_split)]*modulated_signal_imaginary[x-(16-imaginary_split)] + modulated_signal_segment[x]*modulated_signal_segment[x]);
					}
				}
				
			}
				
		}
		
		if(iterations > required_blocks-1){
			return phase_detection(modulated_signal_phase, current_bit);
		}

        return 1;
}