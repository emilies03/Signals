#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>



int get_phase(int filter_order, float modulated_signal_segment[filter_order+1], float modulated_signal_imaginary[16], int current_bit)
{   	
	float modulated_signal_env[16] = {0};
	
	for(int i = 0; i<16; i++)
	{
		modulated_signal_env[i] = sqrt(modulated_signal_imaginary[i]*modulated_signal_imaginary[i] + modulated_signal_segment[i]*modulated_signal_segment[i]);

		if(modulated_signal_env[i] < 0.6)
		{
			current_bit = ~current_bit;
		}
	}
				
	return current_bit;
}