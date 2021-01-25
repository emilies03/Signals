#include<stdio.h>
#include<stdlib.h>
#include<math.h>

unsigned long long bin_to_uint64();
double uint64_to_bin();

int xor(unsigned long long prs1, unsigned long long prs2, int in_array[128]){
    int i, final_bit;
    int array1[64], array2[64];
    unsigned long long num1, num2;
    unsigned long long signal1_xored_prs1, signal2_xored_prs2;
    double average_bit_1, average_bit_2;

    // split 128 bit in_array into 2, 64bit arrays
    for (i=0; i<128; i++){
        if(i<64){
            array1[63-i] = in_array[127-i];
        }
        else{
            array2[63-i+64] = in_array[127-i];
        }

    }

    // convert binary 64bit arrays into uint64
    num1 = bin_to_uint64(array1);
    num2 = bin_to_uint64(array2);

    // Xor signals with corresponding PRS
    signal1_xored_prs1 = prs1^num1;
    signal2_xored_prs2 = prs2^num2;

    // find average binary number of xored signals
    average_bit_1 = uint64_to_bin(signal1_xored_prs1);
    average_bit_2 = uint64_to_bin(signal2_xored_prs2);

    // assign bit based on average bit (average_bit > 0.5 = 1; average_bit < 0.5 =0)
    if(((average_bit_1+average_bit_2)/2) >= 0.5){
        final_bit = 1;
    }
    else
    {
        final_bit = 0;
    }

return final_bit;
}

double uint64_to_bin(unsigned long long xored_prs_input){
    int i, remainder, sum = 0;
    for (i = 0; i < 64; i++){
        remainder = xored_prs_input%2;
        sum += remainder;
        if(remainder == 0){
            xored_prs_input = xored_prs_input/2;
        }
        else if (remainder == 1){
            xored_prs_input = xored_prs_input/2;
        }
    }
    return (double)sum/64;
}

unsigned long long bin_to_uint64(int binary_array[64]){
    int i = 0;
    unsigned long n, sum = 0;
    for (i = 0; i < 64; i++)
    {
        n = pow(2,(double)i);
        sum += (n * binary_array[63-i]);
    }
    return sum;
}



/* prs is a uint64, which can be directly xored with another uint64 to return an uint64 ans
        - would need to convert array of 128 1s/0s into 2 uint64 numbers 
        - could then xor them together
        - answer would be the either the representation for 0 or the representation for 1
*/
