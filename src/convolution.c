#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>



int get_phase(float real_detect_in[2048], float imaginary_detect_in[2048], int prs_signal[128])
{   	
	float modulated_signal_env[16] = {0};
	int current_bit = prs_signal[127];
	// printf("%d \n", current_bit);

	for(int j=0; j<128; j++){
	
		for(int i = 0; i<16; i++)
		{
			modulated_signal_env[i] = sqrt(imaginary_detect_in[i+j*16]*imaginary_detect_in[i+j*16] + real_detect_in[i+j*16]*real_detect_in[i+j*16]);

			if(modulated_signal_env[i] < 0.6)
			{	
				if(current_bit == 0){
					current_bit = 1;
				}
				else{
					current_bit = 0;
				}
			}
		}
		//printf("%d \n", current_bit);

		printf("%lf ", real_detect_in[j*16+1]);
		printf("%lf			", real_detect_in[j*16+3]);
		printf("%lf ", imaginary_detect_in[j*16+1]);
		printf("%lf			", imaginary_detect_in[j*16+3]);
		printf("%lf ", modulated_signal_env[1]);
		printf("%lf	", modulated_signal_env[3]);
		printf("\n");

		prs_signal[j] = current_bit;
		//printf("%d\n", prs_signal[j]);
	
	}
				
	return 1;
}