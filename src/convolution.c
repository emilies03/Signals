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