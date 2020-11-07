#include<stdio.h>
#include<stdlib.h>

int main(){

	FILE *fin1, *fin2, *fout;
	float in1, in2, out;

	// open input files as binary read-only
	fin1=fopen("data/prs_diff.dat","rb");
	if(fin1 == NULL) {
		printf("ERROR: %s does not exist\n","prs_diff");
		exit(1);
	}

	fin2=fopen("data/prs_sum.dat","rb");
	if(fin2 == NULL) {
		printf("ERROR: %s does not exist\n","prs_sum");
		exit(1);
	}

	// open output files as binary - overwrite the file if it alredy exists
	fout=fopen("data/output","w+b");
	if(fout == NULL) {
		printf("ERROR: %s cannot be created\n","output");
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

	exit(0); // all is good
}