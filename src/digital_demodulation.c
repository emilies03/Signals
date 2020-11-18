#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include "hilbert_filter.c"

static int load_files();
int hilbert_filter();
const int filter_order = 40;


int main(){

	//load_files();

	float hilbert_filter_coefficients[filter_order+1];
	hilbert_filter(filter_order, hilbert_filter_coefficients);

	

	for(int i=0; i<= filter_order; i++){
	printf("%f", hilbert_filter_coefficients[i]);
    printf("\n");
	}


	// apply kaiser window

	// hilbert filter to get phase of each 16 points

	// times kaiser by hilbert

	// compare phase of consectutive bits

	// inverse XOR

	// group 8 bits into value 

	exit(0); // all is good
}


int load_files()
{
	FILE *fin1, *fin2, *fout;
	float in1, in2, out;

	// open input files as binary read-only
	fin1=fopen("data/modulated_diff.dat","rb");
	printf("Opening modulated_diff\n");
	if(fin1 == NULL) {
		printf("ERROR: modulated_diff does not exist\n");
		exit(1);
	}
	printf("modulated_diff opened succefully\n");

	fin2=fopen("data/modulated_sum.dat","rb");
	printf("Opening modulated_sum\n");
	if(fin2 == NULL) {
		printf("ERROR: modulated_sum does not exist\n");
		exit(1);
	}
	printf("modulated_suff opened successfully\n");

	// open output files as binary - overwrite the file if it alredy exists
	fout=fopen("data/output","w+b");
	if(fout == NULL) {
		printf("ERROR: output cannot be created\n");
		exit(1);
	}

	printf("processing ...\n");

	// keep reading a float in each file until the end
	while(fread(&in1, sizeof(float), 1, fin1) && fread(&in2, sizeof(float), 1, fin2)){
      // add the inputs and write the sum to the output
      out = in1+in2;
		fwrite(&out, sizeof(float), 1, fout);
	}

	printf("done\n");

	// close the files
	fclose(fin1);
	fclose(fin2);
	fclose(fout);

	return 1;
}