#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <sys/file.h>
#include <unistd.h>
#include <mraa/aio.h>
#include "floatfann.h"

#include "jump_detection.h"

#define BUFF_SIZE 1024

/* sets first <n> values in <*arr> to <val> */
void clear_buffer(double *arr, double val, int n) 
{
	int i;
	for (i = 0; i < n; i++) {
		arr[i] = val;
	}
}


/* Caculates mean of first <n> samples in <*arr> */
double calculate_mean(double *arr, int n)
{
	double total;
	int i;

	total = 0.0f;
	for (i = 0; i < n; i++) {
		total += arr[i];
	}

	return total/((double) n);
}

/* Finds all peaks and troughs using relative thresholding */
int find_peaks_and_troughs(
		double *arr, 	// signal 
		int n_samples, 	// number of samples present in the signal
		double E, 	// threshold for peak detection
		// arrays that will store the indicies of the located
		// peaks and troughs
		double *P, double *T,
		// number of peaks (n_P) and number of troughs (n_T)
		// found in the data set *arr
		int *n_P, int *n_T
		)
{
	int a, b, i, d, _n_P, _n_T;

	i = -1; d = 0; a = 0; b = 0;
	_n_P = 0; _n_T = 0;

	clear_buffer(P, 0.0f, n_samples);
	clear_buffer(T, 0.0f, n_samples);

	while (i != n_samples) {
		i++;
		if (d == 0) {
			if (arr[a] >= (arr[i] + E)) {
				d = 2;
			} else if (arr[i] >= (arr[b] + E)) {
				d = 1;
			}
			if (arr[a] <= arr[i]) {
				a = i;
			} else if (arr[i] <= arr[b]) {
				b = i;
			}
		} else if (d == 1) {
			if (arr[a] <= arr[i]) {
				a = i;
			} else if (arr[a] >= (arr[i] + E)) {
				/*
				 * Peak has been detected.
				 * Add index at detected peak
				 * to array of peak indicies
				 * increment count of peak indicies
				 */
				P[_n_P] = a;
				_n_P++;
				b = i;
				d = 2;
			}
		} else if (d == 2) {
			if (arr[i] <= arr[b]) {
				b = i;
			} else if (arr[i] >= (arr[b] + E)) {
				/*
				 * Trough has been detected.
				 * Add index at detected trough
				 * to array of trough indicies
				 * increment count of trough indicies
				 */
				T[_n_T] = b;
				_n_T++;
				a = i;
				d = 1;
			}
		}
	}

	(*n_P) = _n_P;
	(*n_T) = _n_T;
	return 0;
}


/* Takes in jump characteristics to determine the type of jump */
void jump_neural_network()
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

    double *rise_time, *fall_time, *air_time, *magnitude;
    int num_low_jump[2] = {0}, num_high_jump[2] = {0};
	int total_low_jumps = 0, total_high_jumps = 0;

    fann_type *calc_out;
    fann_type input[4];
    struct fann *ann;
    
    ann = fann_create_from_file("jump_train_data.net");

    // Allocate memory
    ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
    memset(ifile_name, 0, BUFF_SIZE);
    //snprintf(ifile_name, BUFF_SIZE, "stride_test_data.txt";
	num_samples = (int *) malloc(sizeof(int));
	num_inputs = (int *) malloc(sizeof(int));
	num_outputs = (int *) malloc(sizeof(int));
	rise_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	fall_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	air_time = (double *) malloc(sizeof(double) * N_SAMPLES);
	magnitude = (double *) malloc(sizeof(double) * N_SAMPLES);
    
	int numTrainingFiles = 1;
    for (k = 0; k < numTrainingFiles; k++) {

		// Open the files containing the jump characteristics
		switch(k) {
			case 0:
					snprintf(ifile_name, BUFF_SIZE, "jump_training_file.txt");
					//printf("Attempting to read from file \'%s\'.\n", ifile_name);
					fp = fopen(ifile_name, "r");
					if (fp == NULL) {
						fprintf(stderr, "Failed to read from file \'%s\'.\n", ifile_name);	
						exit(EXIT_FAILURE);
					}
					break;
			case 1:
					snprintf(ifile_name, BUFF_SIZE, "jump_training_file_HIGH.txt");
					//printf("Attempting to read from file \'%s\'.\n", ifile_name);
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
		printf("Jumping training file %d has %d samples\n", k+1, N_SAMPLES);

		while ((read = getline(&line, &len, fp)) != -1) {
			rv = sscanf(line, "%lf %lf %lf %lf\n\n", &rise_time[i], &fall_time[i], &air_time[i], &magnitude[i]);
			if (rv != 4) {
				fprintf(stderr,
					"%s %d \'%s\'. %s.\n",
					"Failed to read line",
					i,
					line,
					"Exiting"
					 );
				exit(EXIT_FAILURE);	
			}
			//getline(&line, &len, fp);   // Discard the line with 1s and -1s
			i++;
		}
		fclose(fp);

		// Determine the output of the neural network
		for (i = 0; i < N_SAMPLES; i++) {
			input[0] = rise_time[i];
			input[1] = fall_time[i];
			input[2] = air_time[i];
			input[3] = magnitude[i];
			calc_out = fann_run(ann, input);

			if (calc_out[0] > calc_out[1])
				num_low_jump[k]++;
			else if (calc_out[1] > calc_out[0])
				num_high_jump[k]++;
			
			//printf("%f %f\n", calc_out[0], calc_out[1]);
		}
    }
	
	// Display the confusion matrix
	/*
	printf("\n           Low Jump   High Jump\n");
	for (k = 0; k < numTrainingFiles; k++) {
		switch(k)
		{
			case 0: printf("Low Jump  ");
					break;
					
			case 1: printf("High Jump ");
					break;
		}
		printf("[   %d           %d    ]\n", num_low_jump[k], num_high_jump[k]);
	}
	printf("\n");
	*/
	
	// Display the results
	total_low_jumps = num_low_jump[0] + num_low_jump[1];
	total_high_jumps = num_high_jump[0] + num_high_jump[1];
	printf("%d low jumps and %d high jumps detected\n\n", total_low_jumps, total_high_jumps);

    fann_destroy(ann);
}


/* Jump detection algorithm */
void jump_detection(const char *fname)
{
	/* Generic variables */
	int i, j, idx;
	int rv;
	/* Variables for reading file line by line */
	//char *ifile_name;
	char *ofile_pt_name, *train_file_name, *ofile_jump_name;
	FILE *fpp, *fpi, *fpt, *fpj;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int N_SAMPLES;
	char delete_command[1024];
	int fd;

	/* Variables for storing the data and storing the return values */
	double *t_accel, *t_gyro, *x_accel, *y_accel, *z_accel; 	// variables for data collected from input file
	double *x_gyro, *y_gyro, *z_gyro;
	double *t, *x; 			// pointers to data of interest
	double pk_threshold;	// pk-threshold value
       	
	/* Variables for peak-trough detection */	
	double *P_i; 	// indicies of each peak found by peak detection
	double *T_i; 	// indicies of each trough found by trough detection
	double *PT_i;   // indices of each peak and trough found by peak and trough detection
	double *J_i;    // indicies of each jump
	int n_P; 		// number of peaks
	int n_T; 		// number of troughs
	int n_PT;       // number of peaks and troughs combined
	int n_S; 		// number of strides
	int n_J; 	    // number of jumps
	
	// Centers
	double center;
	double center1 = 0;
	double center2 = -1.1;
	double center3 = -0.3;
	
	// Stride time is the minimum time spacing between peaks and troughs 
	double stride_time = 0.20;
	
	// Threshold Values
    double threshold_accelX = 0.75;          
    double threshold_accelY = 0.80;         
    double threshold_accelZ = 0.80;
    double threshold_gyroX  = 50.0;     
    double threshold_gyroY  = 30.0;         
    double threshold_gyroZ  = 90.0;

	// Jump detection constraints
	double jump_time        = 0.30;
	double jump_range_low   = 1.50;
	double jump_range_high  = 16.0;

	// Mode determines which data set we are using
	int mode = 0;
	
	// Allocate memory for the file names
	//ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
	//memset(ifile_name, 0, BUFF_SIZE);
	ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
	memset(ofile_pt_name, 0, BUFF_SIZE);
	train_file_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
	memset(train_file_name, 0, BUFF_SIZE);
	ofile_jump_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
	memset(ofile_jump_name, 0, BUFF_SIZE);
	
	// File names
	//ifile_name = "data.csv";
	ofile_pt_name = "jump_peaksAndTroughs.csv";
	ofile_jump_name = "jumps.csv";
	if (mode == 1)
	{
		ofile_pt_name = "jump_low_peaksAndTroughs.csv";
		ofile_jump_name = "jumps_low.csv";
		train_file_name = "jump_training_file_LOW.txt";
	}
	else if (mode == 2)
	{
		ofile_pt_name = "jump_high_peaksAndTroughs.csv";
		ofile_jump_name = "jumps_high.csv";
		train_file_name = "jump_training_file_HIGH.txt";
	}
	else
	{
		ofile_pt_name = "jump_peaksAndTroughs.csv";
		ofile_jump_name = "jumps.csv";
		train_file_name = "jump_training_file.txt";
	}
	//printf("Files are %s, %s, %s, %s\n", ifile_name, ofile_pt_name, ofile_jump_name, train_file_name);

	/*
	 * set this to 0 so that the code can function without
	 * having to actually performing stride detection
	 * from peaks and troughs
	 */
	n_PT = 0;
	n_J = 0;
	
	/* // NO LONGER USING INPUT ARGUMENTS
	// Allocate memory for the file names
	if (argc != 15)
	{
		ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ifile_name, 0, BUFF_SIZE);
		ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_pt_name, 0, BUFF_SIZE);
		train_file_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(train_file_name, 0, BUFF_SIZE);
		ofile_jump_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_jump_name, 0, BUFF_SIZE);
	}
	
		
	// Read the values from the shell script
	else {
		ifile_name = argv[1];
		ofile_pt_name = argv[2];
		stride_time = atof(argv[3]);
		threshold_accelX = atof(argv[4]);
		threshold_accelY = atof(argv[5]);
		threshold_accelZ = atof(argv[6]);
		threshold_gyroX = atof(argv[7]);
		threshold_gyroY = atof(argv[8]);
		threshold_gyroZ = atof(argv[9]);
		train_file_name = argv[10];
		ofile_jump_name = argv[11];
		jump_range_low = atof(argv[12]);
		jump_range_high = atof(argv[13]);
		mode = atof(argv[14]);
	}
	*/

	/*printf("Arguments used:\n\t%s=%s\n\t%s=%s\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%s\n\t%s=%s\n\t%s=%lf\n\t%s=%lf\n",
			"ifile_name", ifile_name,
			"ofile_peak_trough_name", ofile_pt_name,
			"stride_time", stride_time,
			"threshold_accelX", threshold_accelX,
			"threshold_accelY", threshold_accelY,
			"threshold_accelZ", threshold_accelZ,
			"threshold_gyroX", threshold_gyroX,
			"threshold_gyroY", threshold_gyroY,
			"threshold_gyroZ", threshold_gyroZ,
			"train_file_name", train_file_name,
			"ofile_jump_name", ofile_jump_name,
			"jump_range_low", jump_range_low,
			"mode", mode
	      );
	*/
	
	// Clear out the buffer
	memset(delete_command, 0, 1024);
	
	// Open the current data file of interest
	fpi = fopen(fname, "r");
	printf("Opening current data file %s\n", fname);
	if (fpi == NULL) 
	{
		fprintf(stderr, "Failed to open file \'%s\'. Exiting.\n", fname);
		exit(EXIT_FAILURE);
	}
		
	// Acquire the lock for the file in case the producer is still writing to it
	fd = fileno(fpi);
	flock(fd, LOCK_EX);
	
	
	/* NO LONGER USED FOR REAL-TIME SYSTEM */
	
	// open the input file 
	//printf("Attempting to read from file \'%s\'.\n", ifile_name);
	/*
	fpi = fopen(ifile_name, "r");
	if (fpi == NULL) {
		fprintf(stderr, 
				"Failed to read from file \'%s\'.\n", 
				ifile_name
		       );
		exit(EXIT_FAILURE);
	}
	*/

	/* count the number of lines in the file */
	read = getline(&line, &len, fpi); //discard header of file
	N_SAMPLES = 0;
	while ((read = getline(&line, &len, fpi)) != -1) {
		N_SAMPLES++;
	}

	/* go back to the start of the file so that the data can be read */
	rewind(fpi);
	read = getline(&line, &len, fpi); //discard header of file

	/* start reading the data from the file into the data structures */
	i = 0;
	t_accel = (double *) malloc(sizeof(double) * N_SAMPLES);
	x_accel = (double *) malloc(sizeof(double) * N_SAMPLES);
	y_accel = (double *) malloc(sizeof(double) * N_SAMPLES);
	z_accel = (double *) malloc(sizeof(double) * N_SAMPLES);
	t_gyro = (double *) malloc(sizeof(double) * N_SAMPLES);
	x_gyro = (double *) malloc(sizeof(double) * N_SAMPLES);
	y_gyro = (double *) malloc(sizeof(double) * N_SAMPLES);
	z_gyro = (double *) malloc(sizeof(double) * N_SAMPLES);
	t = (double *) malloc(sizeof(double) * N_SAMPLES);
	x = (double *) malloc(sizeof(double) * N_SAMPLES);
	
	while ((read = getline(&line, &len, fpi)) != -1) 
	{
		/* parse the data */
		rv = sscanf(line, "%lf,%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", &t_accel[i], &t_gyro[i], &x_accel[i], &y_accel[i],
															   &z_accel[i], &x_gyro[i], &y_gyro[i], &z_gyro[i]);
		if (rv != 8) {
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
	fclose(fpi);
	
	if (line) 
		free(line);
	
	/* 
	 * From selected thresholds, 
	 * find indicies of peaks
	 * find indicies of troughs
	 */
	P_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	T_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	PT_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	J_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	
	/* open the output file to write the peak and trough data */
		//printf("Attempting to write to file \'%s\'.\n", ofile_pt_name);
		fpp = fopen(ofile_pt_name, "w");
		if (fpp == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_pt_name
				   );
			exit(EXIT_FAILURE);
		}
		
	/* open the output file to write the jump data */
		//printf("Attempting to write to file \'%s\'.\n", ofile_jump_name);
		fpj = fopen(ofile_jump_name, "w");
		if (fpj == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_jump_name
				   );
			exit(EXIT_FAILURE);
		}
	
	/* open the output file to write the features of jumps */
		//printf("Attempting to write to file \'%s\'.\n", train_file_name);
		fpt = fopen(train_file_name, "w");
		if (fpt == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					train_file_name
				   );
			exit(EXIT_FAILURE);
		}
	
	int num_axis = 1;
	for (j = 0; j < num_axis; j++) //runs 6 times for accel (3) and gyro (3)
	{
		// Reset number of strides and jumps back to 0 for each iteration
		n_J = 0;
		
		switch(j)
		{//x -> column from data
			case 0: t = t_accel;
					x = x_accel;
					center = center2;
					pk_threshold = threshold_accelX;
					//printf("Accel X\n");
					break;
			
			case 1:	x = y_accel;
					center = center1;
					pk_threshold = threshold_accelY;
					//printf("Accel Y\n");
					break;
			
			case 2: x = z_accel;
					center = center3;
					pk_threshold = threshold_accelZ;
					//printf("Accel Z\n");
					break;
			
			case 3: t = t_gyro;
					x = x_gyro;
					center = center1;
					pk_threshold = threshold_gyroX;
					//printf("Gyro X\n");
					break;
			
			case 4:	x = y_gyro;
					center = center1;
					pk_threshold = threshold_gyroY;
					//printf("Gyro Y\n");
					break;
			
			case 5: x = z_gyro;
					center = center1;
					pk_threshold = threshold_gyroZ;
					//printf("Gyro Z\n");
					break;
		}
		
		rv = find_peaks_and_troughs(
				x, 
				N_SAMPLES, 
				pk_threshold, 
				P_i, T_i, 
				&n_P, &n_T);
		if (rv < 0) {
			fprintf(stderr, "find_peaks_and_troughs failed\n");
			exit(EXIT_FAILURE);
		}

		/* 
		 * Insert your algorithm to convert from a series of peak-trough
		 * indicies, to a series of indicies that indicate the start
		 * of a stride.
		 */

		int k, idx_next;
		int n_P_new = n_P;
		int n_T_new = n_T;
		// stride_time defined at top
        
		// Process Peaks
		for (i = 0; i < n_P_new-1; i++)
		{
			idx = (int) P_i[i];
			idx_next = (int) P_i[i+1];
			if (t[idx_next] - t[idx] < stride_time) // Peaks need to be at least some time apart
			{
				// Save the higher peak, then shift elements left
				if (x[idx] > x[idx_next])    // first peak is higher
					for (k = 0; k < n_P_new-i-2; k++)
					{
						P_i[i+k+1] = P_i[i+k+2];
					}
				else     // second peak is higher
					for (k = 0; k < n_P_new-i-1; k++)
					{
						P_i[i+k] = P_i[i+k+1];
					}

				i--; // Still need to look at the current peak in next iteration
				n_P_new--; // One less peak now
					
			}
		}
		//printf("%d peaks and ", n_P - n_P_new);
		n_P = n_P_new;

		// Process Troughs 
		for (i = 0; i < n_T_new-1; i++)
		{
			idx = (int) T_i[i];
			idx_next = (int) T_i[i+1];
			if (t[idx_next] - t[idx] < stride_time) // Troughs need to be at least some time apart
			{
				// Save the lower trough, then shift elements left
				if (x[idx] < x[idx_next])     // first trough is lower
					for (k = 0; k < n_T_new-i-2; k++)
					{
						T_i[i+k+1] = T_i[i+k+2];
					}
				else     // second trough is lower
					for (k = 0; k < n_T_new-i-1; k++)
					{
						T_i[i+k] = T_i[i+k+1];
					}

				i--; // still need to look at the current trough in next iteration
				n_T_new--; // One less trough now
					
			}
		}
		//printf("%d troughs removed\n", n_T - n_T_new);
		n_T = n_T_new;
         
	
	//**********************************************START OF JUMP DETECTION ALGORITHM**************************************************************************************************
	
		if (j == 0)     // Look at Accel X only
		{
			// A jump is defined by a rise and a fall
			// The rise and fall are both each defined by a trough followed by a peak
			// They must meet the timing constraint
			// The jump must have a minimum magnitude
			
			int peak_index, trough_index;
			double jump_mag = 0;                   // difference between peak and trough magnitude
			//jump_time defined at top             // timing constraint; time between peak and trough
			//jump_range_low defined at top        // magnitude constraint, low
			//jump_range_high defined at top       // magnitude constraint, high 
			
			// Sort the peaks and troughs in chronological order (by index) and place them into one array
			n_PT = 0;	
			i = 0;
			k = 0;
			
			//printf("Jump detection started with %d peaks and %d troughs.\n", n_P, n_T);
			while (i < n_P || k < n_T)
			{
				//printf("i = %d, k = %d\n", i, k);
				if (i < n_P)
					peak_index = (int) P_i[i];
				if (k < n_T)
					trough_index = (int) T_i[k];
				
				if (i < n_P && peak_index < trough_index)
				{
					PT_i[n_PT] = peak_index;
					n_PT++;
					i++;
				}
				else if (k < n_T)
				{
					PT_i[n_PT] = trough_index;
					n_PT++;
					k++;
				}
				else
				{
					i++;
					k++;
				}
				//printf("i = %d, k = %d\n", i, k);
			}
			
			// Print out sorted peaks and troughs
			/* for (i = 0; i < n_PT; i++)
			{
				printf("%lf\n", PT_i[i]);
			} */
			
			// Extract the relevant jump data by finding where the rise and fall occur
			for (i = 0; i < n_PT-1; i++)
			{	
				idx = (int) PT_i[i];
				idx_next = (int) PT_i[i+1];
				
				// Must be a trough followed by a peak
				if ((x[idx] < center) && (x[idx_next] > center))     
				{
					// Must satisfy timing and threshold requirements
					jump_mag = x[idx_next] - x[idx];
					if (((t[idx_next] - t[idx]) < jump_time) && (jump_mag > jump_range_low) && (jump_mag < jump_range_high))
					{
						J_i[n_J] = idx;
						J_i[n_J+1] = idx_next;
						n_J += 2;
					}
				}
				
			}
			
			printf("Jump detection success with %d jumps detected.\n", n_J/4);
		}
		
		
		//**********************************************END OF JUMP DETECTION ALGORITHM****************************************************************************************************

		
		//**********************************************WRITING RESULTS TO FILES***********************************************************************************************************

		// Label the data sets
		switch(j)
		{
			case 0: fprintf(fpp, "Accel X\n");
					break;
			case 1: fprintf(fpp, "Accel Y\n");
					break;
			case 2: fprintf(fpp, "Accel Z\n");
					break;
			case 3: fprintf(fpp, "Gyro X\n");
					break;
			case 4: fprintf(fpp, "Gyro Y\n");
					break;
			case 5: fprintf(fpp, "Gyro Z\n");
					break;
		}
		
		fprintf(fpp, "P_i,P_t,P_x,T_i,T_t,T_p\n");
		for (i = 0; i < n_P || i < n_T; i++) {
			/* Only peak data if there is peak data to write */
			if (i < n_P) {
				idx = (int) P_i[i];
				fprintf(fpp, "%d,%20.10lf,%lf,",
						idx,
						t[idx],
						x[idx]
					   );
			} else {
				fprintf(fpp, ",,,");
			}
			/* Only trough data if there is trough data to write */
			if (i < n_T) {
				idx = (int) T_i[i];
				fprintf(fpp, "%d,%20.10lf,%lf\n",
						idx,
						t[idx],
						x[idx]
					   );
			} else {
				fprintf(fpp, ",,\n");
			}
		}
		fprintf(fpp, "\n\n\n");
		fclose(fpp);
		
		// Label the data sets for jumps
		switch(j)
		{
			case 0: fprintf(fpj, "Accel X\n");
					break;
			case 1: fprintf(fpj, "Accel Y\n");
					break;
			case 2: fprintf(fpj, "Accel Z\n");
					break;
			case 3: fprintf(fpj, "Gyro X\n");
					break;
			case 4: fprintf(fpj, "Gyro Y\n");
					break;
			case 5: fprintf(fpj, "Gyro Z\n");
					break;
		}
		
		// Write jump data if available
		fprintf(fpj, "J_i,J_t,J_x\n");
		for (i = 0; i < n_J; i++) {
			idx = (int) J_i[i];
			fprintf(fpj, "%d,%20.10lf,%lf\n",
					idx,
					t[idx],
					x[idx]
				   );
		}
		fprintf(fpj, "\n\n\n");
		fclose(fpj);
		
		// Compute and write the jump features to the training file
		if (j == 0)   // Jump algorithm applied to Accel X only
		{
			int num_jumps = n_J/4;
			int num_features = 4;
			int num_outputs = 2;
			fprintf(fpt, "%d %d %d\n", num_jumps, num_features, num_outputs);
			
			// Compute the rise time, fall time, air time, and magnitude of jump
			double rise_time, fall_time, air_time, magnitude;
			int rise_idx_trough, rise_idx_peak, fall_idx_trough, fall_idx_peak;
			double time_scale_factor = 1.00;
			double time_offset = 0.00;
			double magnitude_scale_factor = 1.00;
			double magnitude_offset = 0.00;
			
			for (i = 0; i < n_J-3; i = i + 4)
			{
				rise_idx_trough = (int) J_i[i];
				rise_idx_peak = (int) J_i[i+1];
				fall_idx_trough = (int) J_i[i+2];
				fall_idx_peak = (int) J_i[i+3];
				
				// Jump Characteristics
				rise_time = time_scale_factor * (t[rise_idx_peak] - t[rise_idx_trough]) + time_offset;
				fall_time = time_scale_factor * (t[fall_idx_peak] - t[fall_idx_trough]) + time_offset;
				air_time = time_scale_factor * (t[fall_idx_trough] - t[rise_idx_peak]) + time_offset;
				magnitude = magnitude_scale_factor * (x[rise_idx_peak] - x[rise_idx_trough]) + time_offset;
			
				// Write all metrics into the training file
				fprintf(fpt, "%10.8lf %10.8lf %10.8lf %10.8lf\n", rise_time, fall_time, air_time, magnitude);
				// This section is needed only to generate a new file to train the neural network
				/*
				if (mode == 1)     // Low Jump
				{
					fprintf(fpt, "%d %d\n", 1, -1);
				}
				else if (mode == 2)     // High Jump
				{
					fprintf(fpt, "%d %d\n", -1, 1);
				}
				*/
			}
		}
		fclose(fpt);
	}
	
	// Perform neural network analysis
	jump_neural_network();
	
	// Delete the current data file to not process it again in the future
	//printf("Deleting file \'%s\'\n", fname);
	sprintf(delete_command, "rm %s", fname);
	system(delete_command);
	
	//printf("Full execution successful.\n\n");
}



//****************************************************************************************************************************************************************

// MAIN LOOP

int main()
{
	FILE *fp;
	int fd;
	char *line = NULL;
	char *fname;
	size_t len = 0;
	ssize_t read;

	while(1) {
		
		system("ls -1 data_*.csv > fnames.txt");

		// open the file that contains the file names that we need to inspect
		fp = fopen("fnames.txt", "r");
		//printf("Opening \'fnames.txt\' to read each file.\n");
		if (fp == NULL) {
			fprintf(stderr, "failed to open file\n");
			exit(EXIT_FAILURE);
		}

		// extract each file name by reading the file line-by-line
		while ((read = getline(&line, &len, fp)) != -1) {
			//printf("Discovered file \'%s\'", line);
			
			// strip newline from end of line read from file
			fname = line;
			fname[strlen(line)-1] = 0;

			// process file with fname
			//printf("Processing file \'%s\'\n", fname);
			jump_detection(fname);

			sleep(1);
		}
		
		// close the file that contains other filenames
		fclose(fp);

		// delete this file from the system
		//printf("system(\"rm fnames.txt\")\n");
		system("rm fnames.txt");

		sleep(2);
	}

	return 0;
}
