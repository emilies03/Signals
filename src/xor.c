#include<stdio.h>
#include<stdlib.h>
#include<math.h>

unsigned long long bin_to_uint64();
float uint64_to_bin();

int xor(unsigned long long* prs1, unsigned long long* prs2){
    float average_bit;

    int prs_test;
    int input_test[16] = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0};
    int xored_prs_input;
    unsigned long long input_test_single;
    int array1[64], array2[64];

    for (i=0; i<128; i++){
        if(i<64){
            array1[64-i] = in_array[128-i]
        }
        else{
            array2[64-i] = in_array[128-i]
        }

    }

    printf("about to convert input from binary to base 10\n");
    input_test_single = bin_to_uint64(array);

    prs_test = 1;

    xored_prs_input = prs_test^input_test_single;

    average_bit = uint64_to_bin(xored_prs_input);



    printf("input test single: %llu\n", input_test_single);

    printf("prs: %d\n", prs_test);
    printf("xored_prs_input: %d\n", xored_prs_input);
    printf("average bit = %f\n",average_bit);
    printf("prs = %llu\n", prs);



return 1;
}

float uint64_to_bin(int xored_prs_input){
    int i, remainder, sum = 0;
    int array_bin[10];
    for (i = 0; i < 11; i++){
        remainder = xored_prs_input%2;
        sum += remainder;
        array_bin[i-10] = remainder;
        if(xored_prs_input%2 == 0){
            xored_prs_input = xored_prs_input/2;
        }
        else{
            xored_prs_input = xored_prs_input/2 - 0.5;
        }
        printf("%i\n", array_bin[i-10]);
    }
    return (double)sum/11;
}

unsigned long long bin_to_uint64(int binary_array[10]){
    int i, n = 0;
    unsigned long long sum = 0;
    for (i = 0; i <= 9; i++) {
        n = pow(2,(double)i);
        sum += (n * binary_array[9-i]);
    }
    return sum;
}



/* prs is a uint64, which can be directly xored with another uint64 to return an uint64 ans
        - would need to convert array of 128 1s/0s into 2 uint64 numbers 
        - could then xor them together
        - answer would be the either the representation for 0 or the representation for 1
*/
