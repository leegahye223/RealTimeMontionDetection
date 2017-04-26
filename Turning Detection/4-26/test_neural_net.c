#include <unistd.h>
#include <mraa/aio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "floatfann.h"

#define BUFF_SIZE 1024

int main()
{
    int i, k;
    int rv;
    char *ifile_name;
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int N_SAMPLES = 300;
    int *num_samples, *num_inputs, *num_outputs;

    //double *rise_time, *fall_time, *air_time, *magnitude;
    double *avg_gymag_stride; 
    //int num_low_jump[2] = {0}, num_high_jump[2] = {0};
    int num_straight[3] = {0}, num_right_turn[3] = {0}, num_left_turn[3] = {0};
    fann_type *calc_out;
    fann_type input[1];
    struct fann *ann;
    
    ann = fann_create_from_file("train_data.net");

    // Allocate memory
    ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
    memset(ifile_name, 0, BUFF_SIZE);
    //snprintf(ifile_name, BUFF_SIZE, "stride_test_data.txt";
	num_samples = (int *) malloc(sizeof(int));
	num_inputs = (int *) malloc(sizeof(int));
	num_outputs = (int *) malloc(sizeof(int));
	avg_gymag_stride = (double *) malloc(sizeof(double) * N_SAMPLES);

	//rise_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	//fall_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	//air_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	//magnitude = (double *) malloc(sizeof(double) * N_SAMPLES);
    
    for (k = 0; k < 2; k++) {

		// Open the files containing the jump characteristics
		switch(k) {
			case 0:
					snprintf(ifile_name, BUFF_SIZE, "training_file_turn_data1.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 1:
					snprintf(ifile_name, BUFF_SIZE, "training_file_turn_data2.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
		}

		// Read the data from the file into the data structures
		i = 0;
		getline(&line, &len, fp);
		sscanf(line, "%d %d %d\n", num_samples, num_inputs, num_outputs);
		N_SAMPLES = *num_samples;
		printf("Training file %d has %d samples\n", k+1, N_SAMPLES);

		while ((read = getline(&line, &len, fp)) != -1) {
			rv = sscanf(line, "%lf\n\n", &avg_gymag_stride[i]);
			if (rv != 1) {
				fprintf(stderr,
					"%s %d \'%s\'. %s.\n",
					"Failed to read line",
					i,
					line,
					"Exiting"
					 );
				exit(EXIT_FAILURE);	
			}
			getline(&line, &len, fp);   // Discard the line with 1s and -1s
			i++;
		}
		fclose(fp);

		// Determine the output of the neural network
		for (i = 0; i < N_SAMPLES; i++) {
			input[0] = avg_gymag_stride[i];
			//input[0] = rise_time[i];
			//input[1] = fall_time[i];
			//input[2] = air_time[i];
			//input[3] = magnitude[i];
			calc_out = fann_run(ann, input);

			if (calc_out[0] > 0)
				num_straight[k]++;
				//num_low_jump[k]++;
			else if (calc_out[1] > 0)
				num_left_turn[k]++;
				//num_high_jump[k]++;
			else if (calc_out[2] > 0)
				num_right_turn[k]++; 
			
			//printf("%f %f\n", calc_out[0], calc_out[1]);
		}
    }
	
	// Display the confusion matrix
	printf("\n");
	for (k = 0; k < 2; k++) {
		switch(k)
		{	
			case 0: printf("Left Turn ");
					break;
			case 1: printf("Right Turn ");
					break; 
		}
		printf("[%d   %d    %d]\n", num_straight[k], num_left_turn[k], num_right_turn[k]);
	}
	printf("\n");

    fann_destroy(ann);
    return 0;
}