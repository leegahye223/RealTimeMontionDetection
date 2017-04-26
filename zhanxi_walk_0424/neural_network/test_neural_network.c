#include <unistd.h>
#include <mraa/aio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "floatfann.h"

#define BUFF_SIZE 1024

int find_max_index(fann_type *arr, int n)
{
	double min=0;
	double max=0;
	int max_index = 0;

	int i = 0;
	for (i=0;i<n;i++)
	{
	   if (i==0){
	   	min=arr[i];
	    max=arr[i];
	    max_index = i; 
	   }
	   else{
	    if (arr[i]<min) min=arr[i];
	    if (arr[i]>max) 
	    	{
	    		max=arr[i];
	    		max_index = i; 
	    	}
	   }

	}
	return max_index;
}

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

    //==============  gyro-z segmentation ========================================
    double *min_seg1, *max_seg1, *mean_seg1, *std_seg1;
    double *min_seg2, *max_seg2, *mean_seg2, *std_seg2;
    double *min_seg3, *max_seg3, *mean_seg3, *std_seg3;
    double *min_seg4, *max_seg4, *mean_seg4, *std_seg4;

    //==============  accel-y segmentation ========================================
    double *ay_min_seg1, *ay_max_seg1, *ay_mean_seg1, *ay_std_seg1;
    double *ay_min_seg2, *ay_max_seg2, *ay_mean_seg2, *ay_std_seg2;
    double *ay_min_seg3, *ay_max_seg3, *ay_mean_seg3, *ay_std_seg3;
    

    int num_walkspeed1[7] = {0}, num_walkspeed2[7] = {0}, num_walkspeed3[7] = {0}, num_walkspeed4[7] = {0};
    int num_runspeed1[7] = {0}, num_runspeed2[7] = {0}, num_runspeed3[7] = {0};

    fann_type *calc_out;
    fann_type input[34];
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

			min_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			max_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			mean_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			std_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);

			min_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			max_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			mean_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			std_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);

			min_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			max_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			mean_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			std_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);

			min_seg4 = (double *) malloc(sizeof(double) * N_SAMPLES);
			max_seg4 = (double *) malloc(sizeof(double) * N_SAMPLES);
			mean_seg4 = (double *) malloc(sizeof(double) * N_SAMPLES);
			std_seg4 = (double *) malloc(sizeof(double) * N_SAMPLES);

			ay_min_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_max_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_mean_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_std_seg1 = (double *) malloc(sizeof(double) * N_SAMPLES);

			ay_min_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_max_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_mean_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_std_seg2 = (double *) malloc(sizeof(double) * N_SAMPLES);

			ay_min_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_max_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_mean_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);
			ay_std_seg3 = (double *) malloc(sizeof(double) * N_SAMPLES);


    
    for (k = 0; k < 7; k++) {

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
			case 4:
					snprintf(ifile_name, BUFF_SIZE, "test_file5.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 5:
					snprintf(ifile_name, BUFF_SIZE, "test_file6.txt");
					printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 6:
					snprintf(ifile_name, BUFF_SIZE, "test_file7.txt");
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
		rv = sscanf(line, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", 
			&minima_ay[i], &maxima_ay[i], &period_ay[i],&minima_gz[i], &maxima_gz[i], &period_gz[i], 
			&min_seg1[i], &max_seg1[i], &mean_seg1[i], &std_seg1[i],
			&min_seg2[i], &max_seg2[i], &mean_seg2[i], &std_seg2[i],
			&min_seg3[i], &max_seg3[i], &mean_seg3[i], &std_seg3[i],
			&min_seg4[i], &max_seg4[i], &mean_seg4[i], &std_seg4[i],
			&ay_min_seg1[i], &ay_max_seg1[i], &ay_mean_seg1[i], &ay_std_seg1[i],
			&ay_min_seg2[i], &ay_max_seg2[i], &ay_mean_seg2[i], &ay_std_seg2[i],
			&ay_min_seg3[i], &ay_max_seg3[i], &ay_mean_seg3[i], &ay_std_seg3[i]
		);
/*
		rv = printf("%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf\n", 
			minima_ay[i], maxima_ay[i], period_ay[i],minima_gz[i], maxima_gz[i], period_gz[i], 
			min_seg1[i], max_seg1[i], mean_seg1[i], std_seg1[i],
			min_seg2[i], max_seg2[i], mean_seg2[i], std_seg2[i],
			min_seg3[i], max_seg3[i], mean_seg3[i], std_seg3[i],
			min_seg4[i], max_seg4[i], mean_seg4[i], std_seg4[i]
		);*/

		read = getline(&line, &len, fp); // skip a line

		if (rv != 34) {
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

			input[6] = min_seg1[i]; input[7] = max_seg1[i]; input[8] = mean_seg1[i]; input[9] = std_seg1[i];
			input[10] = min_seg2[i]; input[11] = max_seg2[i]; input[12] = mean_seg2[i]; input[13] = std_seg2[i];
			input[14] = min_seg3[i]; input[15] = max_seg3[i]; input[16] = mean_seg3[i]; input[17] = std_seg3[i];
			input[18] = min_seg4[i]; input[19] = max_seg4[i]; input[20] = mean_seg4[i]; input[21] = std_seg4[i];

			input[22] = ay_min_seg1[i]; input[23] = ay_max_seg1[i]; input[24] = ay_mean_seg1[i]; input[25] = ay_std_seg1[i];
			input[26] = ay_min_seg2[i]; input[27] = ay_max_seg2[i]; input[28] = ay_mean_seg2[i]; input[29] = ay_std_seg2[i];
			input[30] = ay_min_seg3[i]; input[31] = ay_max_seg3[i]; input[32] = ay_mean_seg3[i]; input[33] = ay_std_seg3[i];			

			calc_out = fann_run(ann, input);

			// is NOT calc_out[i] > 0
			// it IS max(calc_out)

			int index = 0;
			index = find_max_index(calc_out,7);

			if (index == 0)
				num_walkspeed1[k]++;
			else if (index == 1)
				num_walkspeed2[k]++;
			else if (index == 2)
				num_walkspeed3[k]++;
			else if (index == 3)
				num_walkspeed4[k]++; 
			else if (index == 4)
				num_runspeed1[k]++;
			else if (index == 5)
				num_runspeed2[k]++;
			else if (index == 6)
				num_runspeed3[k]++; 
			
			printf("%f %f %f %f %f %f %f\n", calc_out[0], calc_out[1], calc_out[2], calc_out[3], calc_out[4], calc_out[5], calc_out[6]);
		}
    }
	
	// Display the confusion matrix
	for (k = 0; k < 7; k++) {
		printf("[%d %d %d %d %d %d %d]\n", 
			num_walkspeed1[k], num_walkspeed2[k], num_walkspeed3[k], num_walkspeed4[k],
			num_runspeed1[k], num_runspeed2[k], num_runspeed3[k]);
	}

    fann_destroy(ann);
    return 0;
}
