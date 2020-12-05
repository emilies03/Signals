#include<stdio.h>
#include<stdlib.h>
#include<math.h>

unsigned long long bin_to_uint64();
double uint64_to_bin();

int xor(unsigned long long prs1, unsigned long long prs2, int in_array[128]){
    int i, final_bit;
    float average_bit;

    unsigned int prs_test = 2;
    int input_test[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0};
    unsigned int xored_prs_input;
    unsigned long long input_test_single;
    int array1[64], array2[64];
    unsigned long long num1, num2;
    unsigned long long signal1_xored_prs1, signal2_xored_prs2;
    double average_bit_1, average_bit_2;


    //checking prs read in

    printf("prs1 = %llu",prs1);
    printf("prs2 = %llu",prs2);
    // printf("\n");
    // split 128 bit in_array into 2, 64bit arrays
    for (i=0; i<128; i++){
        if(i<64){
            array1[63-i] = in_array[127-i];
           printf(" %i,", array1[63-i]);
        }
        else{
            array2[63-i+64] = in_array[127-i];
           printf("| %i ", array2[63-i+64]);
        }

    }
    printf("\n");

    // convert binary 64bit arrays into uint64
    num1 = bin_to_uint64(array1);
    num2 = bin_to_uint64(array2);

    // Xor signals with corresponding PRS
    signal1_xored_prs1 = prs1^num1;
    signal2_xored_prs2 = prs2^num2;

//    printf("signal1_xored_prs1 = %llu\n",signal1_xored_prs1);
//    printf("signal2_xored_prs2 = %llu\n",signal2_xored_prs2);

    // find average binary number of xored signals
    average_bit_1 = uint64_to_bin(signal1_xored_prs1);
    average_bit_2 = uint64_to_bin(signal2_xored_prs2);

    // printf("average bit 1 = %f\n", average_bit_1);
    // printf("average bit 2 = %f\n", average_bit_2);

    // assign bit based on average bit (average_bit > 0.5 = 1; average_bit < 0.5 =0)
    if(((average_bit_1+average_bit_2)/2) >= 0.5){
        final_bit = 1;
    }

    if(((average_bit_1+average_bit_2)/2) < 0.5){
        final_bit = 0;
    }


    

    // printf("about to convert input from binary to base 10\n");
    // input_test_single = bin_to_uint64(input_test);
    // printf("input test signal = %lu\n", input_test_single);

    // prs_test = 0;

    // xored_prs_input = prs_test^input_test_single;
    // printf("input test single: %llu\n", input_test_single);

    // printf("prs: %d\n", prs_test);
    // printf("xored_prs_input: %d\n", xored_prs_input);
    // average_bit = uint64_to_bin(xored_prs_input);
    // printf("average bit = %f\n",average_bit);
    // printf("prs_test = %i\n", prs_test);



return final_bit;
}

double uint64_to_bin(unsigned long long xored_prs_input){
    int i, remainder, sum = 0;
    int array_bin[64];
    for (i = 0; i < 64; i++){
    //    printf("xored prs input = %llu\n",xored_prs_input);
        remainder = xored_prs_input%2;
        sum += remainder;
        array_bin[i-63] = remainder;
        if(remainder == 0){
            xored_prs_input = xored_prs_input/2;
        }
        else if (remainder == 1){
            xored_prs_input = xored_prs_input/2;
        }
    //    printf("%i = %i\n", i,remainder);
    }
    for (i = 0; i < 64; i++){
        // printf("%i",array_bin[i]);
    }
    return (double)sum/64;
}

unsigned long long bin_to_uint64(int binary_array[64]){
    int i = 0;
    unsigned long n = 0;
    unsigned long sum = 0;
    for (i = 0; i < 64; i++) {
        n = pow(2,(double)i);
       printf(" n = %llu ",n);
       printf(" binarray_array %i = %i ", i,binary_array[63-i]);
        sum += (n * binary_array[63-i]);
       printf("%llu",n * binary_array[63-i]);
       printf("\n");
    }
    printf("bin to unit64 in the conversion function = %lu\n",sum);
    printf("sum +1000, %lu\n",sum+1000);
    return sum;
}



/* prs is a uint64, which can be directly xored with another uint64 to return an uint64 ans
        - would need to convert array of 128 1s/0s into 2 uint64 numbers 
        - could then xor them together
        - answer would be the either the representation for 0 or the representation for 1
*/
