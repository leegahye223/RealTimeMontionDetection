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

    double *minima_ay, *maxima_ay, *period_ay, *minima_gz, *maxima_gz, *period_gz;
    int num_walkspeed1[4] = {0}, num_walkspeed2[4] = {0}, num_walkspeed3[4] = {0}, num_walkspeed4[4] = {0};

    fann_type *calc_out;
    fann_type input[6];
    struct fann *ann;
    
    ann = fann_create_from_file("train_data.net");

    // Allocate memory
    ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
    memset(ifile_name, 0, BUFF_SIZE);
    //snprintf(ifile_name, BUFF_SIZE, "stride_test_data.txt";
	num_samples = (int *) malloc(sizeof(int));
	num_inputs = (int *) malloc(sizeof(int));
	num_outputs = (int *) malloc(sizeof(int));
	minima_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			maxima_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			period_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			minima_gz = (double *) malloc(sizeof(double) * N_SAMPLES);
			maxima_gz = (double *) malloc(sizeof(double) * N_SAMPLES);
			period_gz = (double *) malloc(sizeof(double) * N_SAMPLES);	
    
    for (k = 0; k < 4; k++) {

		// Open the file containing the stride characteristics
		switch(k) {
			case 0:
					snprintf(ifile_name, BUFF_SIZE, "test_file1.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 1:
					snprintf(ifile_name, BUFF_SIZE, "test_file2.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 2:
					snprintf(ifile_name, BUFF_SIZE, "test_file3.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 3:
					snprintf(ifile_name, BUFF_SIZE, "test_file4.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
		}

		// Read the data from the file into the data structures
		i = 0;
		getline(&line, &len, fp);
		sscanf(line, "%d %d %d\n", num_samples, num_inputs, num_outputs);
		N_SAMPLES = *num_samples;
		printf("Training file %d has %d samples\n", k+1, N_SAMPLES);

		while ((read = getline(&line, &len, fp)) != -1) {
		rv = sscanf(line, "%lf %lf %lf %lf %lf %lf\n", &minima_ay[i], &maxima_ay[i], &period_ay[i],&minima_gz[i], &maxima_gz[i], &period_gz[i]);
		read = getline(&line, &len, fp); // skip a line

		if (rv != 6) {
			fprintf(stderr,
				"%s %d \'%s\'. %s.\n",
				"Failed to read line",
				i,
				line,
				"Exiting"
				 );
			exit(EXIT_FAILURE);	
		}
		i++;
		}
		fclose(fp);

		// Determine the output of the neural network
		for (i = 0; i < N_SAMPLES; i++) {
			input[0] = minima_ay[i];
			input[1] = maxima_ay[i];
			input[2] = period_ay[i];
			input[3] = minima_gz[i];
			input[4] = maxima_gz[i];
			input[5] = period_gz[i];
			calc_out = fann_run(ann, input);

			if (calc_out[0] > 0)
				num_walkspeed1[k]++;
			else if (calc_out[1] > 0)
				num_walkspeed2[k]++;
			else if (calc_out[2] > 0)
				num_walkspeed3[k]++;
			else if (calc_out[3] > 0)
				num_walkspeed4[k]++;
			
			printf("%f %f %f %f\n", calc_out[0], calc_out[1], calc_out[2], calc_out[3]);
		}
    }
	
	// Display the confusion matrix
	for (k = 0; k < 4; k++) {
		printf("[%d %d %d %d]\n", num_walkspeed1[k], num_walkspeed2[k], num_walkspeed3[k], num_walkspeed4[k]);
	}

    fann_destroy(ann);
    return 0;
}
