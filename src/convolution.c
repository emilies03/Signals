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
        for(j=0; j<length_signal_2; j++){
            if(track>=0 && track<length_signal_1){
            sum += signal_1[track]*signal_2[j];
            }
            track -= 1;
        }
        output[i] = sum;
    }

/*
    printf("convolution: \n"); 
    for (int i = 0; i < length_signal_1+length_signal_2-1; ++i) { 
        printf("%f, \n", convolution[i]);
    };
*/

return 1;
}


int block_convolution(float modulated_sum_segment[16], float windowed_filter_coefficients[41], 
    float tail[40], float modulated_sum_imaginary[16], int filter_order)
{
    float convolution_output[56];
    convolution(filter_order+1, 16, windowed_filter_coefficients, modulated_sum_segment, convolution_output);

    for(int i=0; i<40; i++){
			convolution_output[i] = convolution_output[i] + tail[i];
		}

    for(int i=0; i<16; i++){
			modulated_sum_imaginary[i] = convolution_output[i];
		} 

    for(int i=0; i<40; i++){
			tail[i] = convolution_output[i+16];
		}
    
    return 1;
}


int phase_detection(float modulated_sum_phase[16], float previous_phase[16])
{   
    float difference = 0;
    for(int i=0; i<16; i++){
        difference += fabs(modulated_sum_phase[0]-previous_phase[0]);
    }

	if(difference < (8*M_PI)){
		return 0;
	}
	if(difference >= (8*M_PI)){
		 return 1;
	}
}


int get_phase(float modulated_sum_segment[16], float modulated_sum_imaginary[16], float tail[40],
	float modulated_sum_phase[16], float first_real_block[16], float second_real_block[16],
	float previous_imaginary_block[16], float previous_phase[16], int iterations, float windowed_filter_coefficients[41],
    int filter_order)
{   

    for(int i=0;i<16;i++){
			previous_imaginary_block[i] = modulated_sum_imaginary[i];
			first_real_block[i] = second_real_block[i];
			second_real_block[i] = modulated_sum_segment[i];
		} 

		block_convolution(modulated_sum_segment, windowed_filter_coefficients, tail, modulated_sum_imaginary, 
		filter_order);


		if(iterations>2){
			for(int i=0;i<16;i++){
			previous_phase[i] = modulated_sum_phase[i];
			} 
		}

		if(iterations > 1){
			for(int i = 0; i<16; i++){
				if (modulated_sum_segment[i]==0)
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
		
		if(iterations > 2){
			phase_detection(modulated_sum_phase, previous_phase);
		}

        return 1;
}