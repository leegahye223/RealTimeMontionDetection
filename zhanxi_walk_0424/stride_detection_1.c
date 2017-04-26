// gcc -lmraa -lm -o stride_detection_1 stride_detection_1.c
// gcc -Wall -lmraa -lm -o stride_detection_1 stride_detection_1.c functions_neural_network.c -lfann

/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

#include <signal.h>
#include "functions_neural_network.h"


#define BUFF_SIZE 1024




sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig)
{
    if (sig == SIGINT)
        run_flag = 0;
}

/*
 * sets first <n> values in <*arr> to <val>
 */
void clear_buffer(double *arr, double val, int n) 
{
	int i;
	for (i = 0; i < n; i++) {
		arr[i] = val;
	}
}

/*
 * Caculates mean of first <n> samples in <*arr>
 */
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

double calculate_min(double *arr, int n)
{
	double min=0;
	double max=0;
	double range = 0;
	int i=0;
	for (i=0;i<n;i++)
	{
	   if (i==0){
	   	min=arr[i];
	    max=arr[i];
	   }
	   else{
	    if (arr[i]<min) min=arr[i];
	    if (arr[i]>max) max=arr[i];
	   }

	}
	return min;
}

double calculate_max(double *arr, int n)
{
	double min=0;
	double max=0;
	double range = 0;
	int i = 0;
	for (i=0;i<n;i++)
	{
	   if (i==0){
	   	min=arr[i];
	    max=arr[i];
	   }
	   else{
	    if (arr[i]<min) min=arr[i];
	    if (arr[i]>max) max=arr[i];
	   }

	}
	return max;
}

double calculate_range(double *arr, int n)
{
	double min=0;
	double max=0;
	double range = 0;
	int i = 0;
	for (i=0;i<n;i++)
	{
	   if (i==0){
	   	min=arr[i];
	    max=arr[i];
	   }
	   else{
	    if (arr[i]<min) min=arr[i];
	    if (arr[i]>max) max=arr[i];
	   }

	}
	range = max - min;

	//printf("The max is %10.8lf \n", max);
	//printf("The min is %10.8lf \n", min);
	//printf("The range is %10.8lf \n", range);
	return range;

}

double calculate_mean_righthalf(double *arr, int n)
{
	double total;
	int i;

	total = 0.0f;
	for (i = 0; i < n; i++) {
		if (i >= n/2)
		total += arr[i];
	}

	return 2 * total/((double) n);
}

double calculate_dc_offset(double *arr, int n)
{
	double RawData;
	double SmoothData;
	double *filered_signal;
	double LPF_Beta = 0.001; // 0<ß<1
							 // ß -> 0 smoother
							 // 0.001 for calculating de offset
	int i = 0;

	filered_signal = (double *) malloc(sizeof(double) * n);

    // LPF: Y(n) = (1-ß)*Y(n-1) + (ß*X(n))) = Y(n-1) - (ß*(Y(n-1)-X(n)));
    while(i <= n){
       // Function that brings Fresh Data into RawData
       RawData = arr[i];
       SmoothData = SmoothData - (LPF_Beta * (SmoothData - RawData));
       filered_signal[i] = SmoothData;
       i++;
    }

    // write filtered data to file
    char *file_name = "testLPF0001x.csv";
    FILE *fpt;

    printf("Attempting to write to file \'%s\'.\n", file_name);
		fpt = fopen(file_name, "w");
		if (fpt == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					file_name
				   );
			exit(EXIT_FAILURE);
		}

	fprintf(fpt, "S_i,S_t,S_x\n");
		for (i = 0; i < n; i++) {
			fprintf(fpt, "%20.10lf\n",
					filered_signal[i]
				   );
		}
		fprintf(fpt, "\n\n\n");

		fclose(fpt);

	// calculate offset
    return calculate_mean_righthalf(filered_signal, n);
}



double calculate_std(double *arr, int n)
{
	double mean = 0.0;
	double std = 0.0;
	int i=0;

    mean = calculate_mean(arr,n);

    for(i=0; i<n; ++i)
        std += pow(arr[i] - mean, 2);

    return sqrt(std/n);
}

int 
find_peaks_and_troughs(
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

	// overwirte relative threshold to a ratio of the range
	E = 0.4 * calculate_range(arr, n_samples);   

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


// divede each stride into 4 segments
// each segment return min, max, mean, std
double *stride_segmentation(double *arr, int n)
{
	int n_inseg = floor(n/4);
	int i = 0;

	// segment 1: arr[0] to arr[n_inseg-1]
	// segment 2: arr[n_inseg] to arr[2*n_inseg-1]
	// segment 3: arr[2*n_inseg] to arr[3*n_inseg-1]
	// segment 4: arr[3*n_inseg] to arr[n-1]

	double *segment_features; // output
	double *segment1, *segment2, *segment3, *segment4;
	segment_features = (double *) malloc(sizeof(double) * 20);
	segment1 = (double *) malloc(sizeof(double) * (n_inseg));
	segment2 = (double *) malloc(sizeof(double) * (n_inseg));
	segment3 = (double *) malloc(sizeof(double) * (n_inseg));
	segment4 = (double *) malloc(sizeof(double) * (n_inseg));

	for (i=0;i<n_inseg;i++)
	{
		segment1[i] = arr[i];
	}

	for (i=0;i<n_inseg;i++)
	{
		segment2[i] = arr[i+n_inseg];
	}

	for (i=0;i<n_inseg;i++)
	{
		segment3[i] = arr[i+2*n_inseg];
	}

	for (i=0;i<n_inseg;i++)
	{
		segment4[i] = arr[i+3*n_inseg];
	}

	segment_features[0] =  calculate_min(segment1, n_inseg);
	segment_features[1] =  calculate_max(segment1, n_inseg);
	segment_features[2] =  calculate_mean(segment1, n_inseg);
	segment_features[3] =  calculate_std(segment1, n_inseg);

	segment_features[4] =  calculate_min(segment2, n_inseg);
	segment_features[5] =  calculate_max(segment2, n_inseg);
	segment_features[6] =  calculate_mean(segment2, n_inseg);
	segment_features[7] =  calculate_std(segment2, n_inseg);

	segment_features[8] =  calculate_min(segment3, n_inseg);
	segment_features[9] =  calculate_max(segment3, n_inseg);
	segment_features[10] =  calculate_mean(segment3, n_inseg);
	segment_features[11] =  calculate_std(segment3, n_inseg);

	segment_features[12] =  calculate_min(segment4, n_inseg);
	segment_features[13] =  calculate_max(segment4, n_inseg);
	segment_features[14] =  calculate_mean(segment4, n_inseg);
	segment_features[15] =  calculate_std(segment4, n_inseg);

	return segment_features;
}

// divede each stride into 3 segments
// each segment return min, max, mean, std
double *stride_segmentation_3(double *arr, int n)
{
	int n_inseg = floor(n/3);
	int i = 0;

	// segment 1: arr[0] to arr[n_inseg-1]
	// segment 2: arr[n_inseg] to arr[2*n_inseg-1]
	// segment 3: arr[2*n_inseg] to arr[3*n_inseg-1]
	// segment 4: arr[3*n_inseg] to arr[n-1]

	double *segment_features; // output
	double *segment1, *segment2, *segment3;
	segment_features = (double *) malloc(sizeof(double) * 12);
	segment1 = (double *) malloc(sizeof(double) * (n_inseg));
	segment2 = (double *) malloc(sizeof(double) * (n_inseg));
	segment3 = (double *) malloc(sizeof(double) * (n_inseg));

	for (i=0;i<n_inseg;i++)
	{
		segment1[i] = arr[i];
	}

	for (i=0;i<n_inseg;i++)
	{
		segment2[i] = arr[i+n_inseg];
	}

	for (i=0;i<n_inseg;i++)
	{
		segment3[i] = arr[i+2*n_inseg];
	}

	segment_features[0] =  calculate_min(segment1, n_inseg);
	segment_features[1] =  calculate_max(segment1, n_inseg);
	segment_features[2] =  calculate_mean(segment1, n_inseg);
	segment_features[3] =  calculate_std(segment1, n_inseg);

	segment_features[4] =  calculate_min(segment2, n_inseg);
	segment_features[5] =  calculate_max(segment2, n_inseg);
	segment_features[6] =  calculate_mean(segment2, n_inseg);
	segment_features[7] =  calculate_std(segment2, n_inseg);

	segment_features[8] =  calculate_min(segment3, n_inseg);
	segment_features[9] =  calculate_max(segment3, n_inseg);
	segment_features[10] =  calculate_mean(segment3, n_inseg);
	segment_features[11] =  calculate_std(segment3, n_inseg);

	return segment_features;
}




int main(int argc, char **argv)
{
	/* Generic variables */
	int i, j, idx;
	int rv;
	/* Variables for reading file line by line */
	char *ifile_name, *ofile_pt_name, *ofile_st_name, *train_file_name;
	FILE *fpp, *fps, *fpi, *fpt;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int N_SAMPLES;

	/* Variables for storing the data and storing the return values */
	double *t_accel, *t_gyro, *x_accel, *y_accel, *z_accel; 	// variables for data collected from input file
	double *x_gyro, *y_gyro, *z_gyro;
	double *t, *x; // pointers to data of interest
	double pk_threshold;	// pk-threshold value
       	/* Variables for peak-trough detection */	
	double *P_i; 	// indicies of each peak found by peak detection
	double *T_i; 	// indicies of each trough found by trough detection
	double *S_i; 	// indicies of the start of each stride
	int n_P; 	// number of peaks
	int n_T; 	// number of troughs
	int n_S; 	// number of strides
	
	// Centers
	double center;
	double center1 = 0;
	double center2 = -1.1;
	double center3 = -0.3;
	
	// Threshold Values
    double threshold_accelX;          
    double threshold_accelY;         
    double threshold_accelZ;
    double threshold_gyroX;     
    double threshold_gyroY;         
    double threshold_gyroZ; 

	// Mode determines which data set we are using
	double mode;

	/*
	 * set this to 0 so that the code can function without
	 * having to actually performing stride detection
	 * from peaks and troughs
	 */
	n_S = 0; 
	
	/*
	 * Check if the user entered the correct command line arguments
	 * Usage: 
	 * ./extract_stride_data <ifile_name> <output_peaks> <output_strides>
	 * 				<threshold_value_double>
	 * Or 
	 * ./extract_stride_data
	 */

	/*
	if (argc != 12) {
		ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ifile_name, 0, BUFF_SIZE);
		snprintf(ifile_name, 
				BUFF_SIZE, 
				"Acceleration_Walk_Dataset.csv"
			);
		ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_pt_name, 0, BUFF_SIZE);
		snprintf(ofile_pt_name, BUFF_SIZE, "acceleration_output.csv");
		ofile_st_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_st_name, 0, BUFF_SIZE);
		snprintf(ofile_st_name, BUFF_SIZE, "acceleration_strides.csv");
		train_file_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(train_file_name, 0, BUFF_SIZE);
		pk_threshold = 6.7;
	} else {
		*/
		ifile_name = argv[1];
		ofile_pt_name = argv[2];
		ofile_st_name = argv[3];
		threshold_accelX = atof(argv[4]);
		threshold_accelY = atof(argv[5]);
		threshold_accelZ = atof(argv[6]);
		threshold_gyroX = atof(argv[7]);
		threshold_gyroY = atof(argv[8]);
		threshold_gyroZ = atof(argv[9]);
		train_file_name = argv[10];
		mode = atof(argv[11]);
	//}
/*
	printf("Arguments used:\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n",
			"ifile_name", ifile_name,
			"ofile_peak_trough_name", ofile_pt_name,
			"ofile_stride_name", ofile_st_name,
			"threshold_accelX", threshold_accelX,
			"threshold_accelY", threshold_accelY,
			"threshold_accelZ", threshold_accelZ,
			"threshold_gyroX", threshold_gyroX,
			"threshold_gyroY", threshold_gyroY,
			"threshold_gyroZ", threshold_gyroZ,
			"train_file_name", train_file_name
	      );

	      */
 while(run_flag)
    {
	/* open the input file */
	printf("Waiting for file \'%s\'.\n", ifile_name);
	fpi = fopen(ifile_name, "r");
	usleep(200000);
	if (fpi != NULL) 
	{
		
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

	
	while ((read = getline(&line, &len, fpi)) != -1) {
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


	// ==========================================================================
	// if need to convert from 6G to 8G
	if (mode == 1||mode == 2||mode == 3||mode == 4)
	{
	int ii = 0;
	for (ii = 0; ii < N_SAMPLES; ii++)
	{
		double temp = 0;
		temp = x_accel[ii]*6/8;
		x_accel[ii] = temp;

		temp = y_accel[ii]*6/8;
		y_accel[ii] = temp;

		temp = z_accel[ii]*6/8;
		z_accel[ii] = temp;

		temp = x_gyro[ii]*6/8;
		x_gyro[ii] = temp;

		temp = y_gyro[ii]*6/8;
		y_gyro[ii] = temp;

		temp = z_gyro[ii]*6/8;
		z_gyro[ii]= temp;
	}
}
	// ==========================================================================

	fclose(fpi);
    //input file closed
	/* 
	 * From selected thresholds, 
	 * find indicies of peaks
	 * find indicies of troughs
	 */
	P_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	T_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	S_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	
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
	
	/* open the output file to write the stride data */
		//printf("Attempting to write to file \'%s\'.\n", ofile_st_name);
		fps = fopen(ofile_st_name, "w");
		if (fps == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_st_name
				   );
			exit(EXIT_FAILURE);
		}
		
	/* open the output file to write the features of strides */
		//printf("Attempting to write to file \'%s\'.\n", train_file_name);
		fpt = fopen(train_file_name, "w");
		if (fpt == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					train_file_name
				   );
			exit(EXIT_FAILURE);
		}
	
	double *minima_ay, *maxima_ay, *period_ay, *minima_gz, *maxima_gz, *period_gz;

			minima_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			maxima_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			period_ay = (double *) malloc(sizeof(double) * N_SAMPLES);
			minima_gz = (double *) malloc(sizeof(double) * N_SAMPLES);
			maxima_gz = (double *) malloc(sizeof(double) * N_SAMPLES);
			period_gz = (double *) malloc(sizeof(double) * N_SAMPLES);	
	
	int n_samples_ay = 0, n_samples_gz = 0;

	for (j = 0; j < 6; j++) //runs 6 times for accel and gyro
	{
		n_S = 0; // Number of strides equals to 0 for each iteration
		switch(j)
		{//x -> column from data
			case 0: t = t_accel;
					x = x_accel;
					center = center2;
					pk_threshold = threshold_accelX;
					break;
			
			case 1:	x = y_accel;
					center = center1;
					pk_threshold = threshold_accelY;
					break;
			
			case 2: x = z_accel;
					center = center3;
					pk_threshold = threshold_accelZ;
					break;
			
			case 3: t = t_gyro;
					x = x_gyro;
					center = center1;
					pk_threshold = threshold_gyroX;
					break;
			
			case 4:	x = y_gyro;
					center = center1;
					pk_threshold = threshold_gyroY;
					break;
			
			case 5: x = z_gyro;
					center = center1;
					pk_threshold = threshold_gyroZ;
					break;
		}

		// int idx_45;

		// for (idx_45 = 0; idx_45 < N_SAMPLES; idx_45++) {
		// 	printf("%20.10lf\n", t[idx_45]);
		// }
		
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

		/* DO NOT MODIFY ANYTHING BEFORE THIS LINE */

		/* 
		 * Insert your algorithm to convert from a series of peak-trough
		 * indicies, to a series of indicies that indicate the start
		 * of a stride.
		 */

		int k, idx_next;
		double stride_time = 0.5;
		int n_P_new = n_P;
		int n_T_new = n_T;
        
		// Process Peaks
		for (i = 0; i < n_P_new-1; i++)
		{
			idx = (int) P_i[i];
			idx_next = (int) P_i[i+1];
			if (t[idx_next] - t[idx] < stride_time) // Strides need to be at least 0.5 second apart
			{
				// Eliminate the second peak by shifting remaining elements to the left
				for (k = 0; k < n_P_new-i-2; k++)
				{
					P_i[i+k+1] = P_i[i+k+2];
				}

				i--; // Still need to look at the current peak in next iteration
				n_P_new--; // One less peak now
					
			}
		}
		//printf("%d peaks removed\n", n_P - n_P_new);
		n_P = n_P_new;

		// Process Troughs 
		for (i = 0; i < n_T_new-1; i++)
		{
			idx = (int) T_i[i];

			if (fabs(x[idx] - center) < 0.02)
			{
				for (k = 0; k < n_T_new-i-1; k++)
				{
					T_i[i+k] = T_i[i+k+1];
				}
				//i--;
				n_T_new--;
			}

			idx_next = (int) T_i[i+1];
			if (t[idx_next] - t[idx] < stride_time) // Strides need to be at least 0.5 second apart
			{
				// Eliminate the second peak by shifting remaining elements to the left
				for (k = 0; k < n_T_new-i-2; k++)
				{
					T_i[i+k+1] = T_i[i+k+2];
				}

				i--; // still need to look at the current trough in next iteration
				n_T_new--; // One less trough now
					
			}
		}

		// add one more condition
		// delete peaks or troughs that are too close to offset value


		//printf("%d troughs removed\n", n_T - n_T_new);
		n_T = n_T_new;
         
         
		// Identify Strides
		// Strides are defined by a trough and peak in this order
		// A stride starts before a trough and ends after a peak
		
		int peak_index, trough_index;
		int edge_found;
		double time_threshold = 0.01;
		double value_threshold = 0.1; 

		for (i = 0; i < n_P; i++) // There can be at most n_P strides
		{
			trough_index = (int) T_i[i];
			peak_index = (int) P_i[i];

			// For acceleration
			if (trough_index < peak_index)
			{
				// Find where the stride starts by finding a value before the trough
				edge_found = 0;
				k = 0;
				while (!edge_found && (trough_index-k > 0))
				{	
					if ((fabsf(x[trough_index-k]-center) < value_threshold) && (t[trough_index] - t[trough_index-k] > time_threshold))
					{
						S_i[n_S] = trough_index - k;
						n_S++;
						edge_found = 1;
					}
					k++;	
				}

				// Find where the stride ends by finding a value after the peak
				edge_found = 0;
				k = 0;
				while (!edge_found && (peak_index+k < N_SAMPLES))
				{
					if ((fabsf(x[peak_index+k]-center) < value_threshold) && (t[peak_index+k] - t[peak_index] > time_threshold))
					{
						S_i[n_S] = peak_index + k;
						n_S++;
						edge_found = 1;
					}
					k++;	
				}
			}

			// For deceleration
			else
			{
				// Find where the stride begins by finding a value before the peak
				edge_found = 0;
				k = 0;
				while (!edge_found && (peak_index-k > 0))
				{
					if ((fabsf(x[peak_index-k]-center) < value_threshold) && (t[peak_index] - t[peak_index-k] > time_threshold))
					{
						S_i[n_S] = peak_index - k;
						n_S++;
						edge_found = 1;
					}
					k++;	
				}

				// Find where the stride ends by finding a value after the trough
				edge_found = 0;
				k = 0;
				while (!edge_found && (trough_index+k < N_SAMPLES))
				{	
					if ((fabsf(x[trough_index+k]-center) < value_threshold) && (t[trough_index+k] - t[trough_index] > time_threshold))
					{
						S_i[n_S] = trough_index + k;
						n_S++;
						edge_found = 1;
					}
					k++;	
				}

			}

		}


		/* DO NOT MODIFY ANYTHING AFTER THIS LINE */

		/* Write the stride features to the training file */
			int num_samples = n_P-1;
			int num_features = 34;
			int num_speeds = 7;
			double scale_slope = 1.00; // change from 0.25 to 1 because it's already normalized
			double scale_offset = 0.00;
			double period_slope = 0.40; // nomalize the period
			double period_offset = 0.00; // change from -1.30 to 0

			int stride_idx1 = 0, stride_idx2 = 0;
			int peak_idx1 = 0, peak_idx2 = 0;
			int trough_idx = 0, trough_idx1 = 0, trough_idx2 = 0;


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
		//fclose(fpp);

		// Label the data sets strides
		switch(j)
		{
			case 0: fprintf(fps, "Accel X\n");
					break;
			case 1: fprintf(fps, "Accel Y\n");
					break;
			case 2: fprintf(fps, "Accel Z\n");
					break;
			case 3: fprintf(fps, "Gyro X\n");
					break;
			case 4: fprintf(fps, "Gyro Y\n");
					break;
			case 5: fprintf(fps, "Gyro Z\n");
					break;
		}
		
		fprintf(fps, "S_i,S_t,S_x\n");
		for (i = 0; i < n_S; i++) {
			idx = (int) S_i[i];
			fprintf(fps, "%d,%20.10lf,%lf\n",
					idx,
					t[idx],
					x[idx]
				   );
		}
		fprintf(fps, "\n\n\n");
		//fclose(fps);
	


		if (j == 1) // For accel Y only because walking in a straight line
		{
			n_samples_ay = n_P-1;
			// Write the minima, maxima, and period of the stride to the training file in this order
			k = 0;

			for (i = 0; (i < n_P-1) && (k < n_S-1); i++)
			{
				//stride_idx1 = (int) S_i[k];
				//stride_idx2 = (int) S_i[k+1];
				peak_idx1 = (int) P_i[i];
				peak_idx2 = (int) P_i[i+1];
				trough_idx = (int) T_i[i];
				
				minima_ay[i] = scale_slope * x[trough_idx] + scale_offset;                            // minima is the trough
				maxima_ay[i] = scale_slope * x[peak_idx1] + scale_offset;                             // maxima is the peak
				period_ay[i] = period_slope * (t[peak_idx2] - t[peak_idx1]) + period_offset;          // period is the time from peak to peak
				if (period_ay[i] > 1) period_ay[i] = 1;  // in case period value exceeds 1
				
				k = k + 1;
			}
		}

		if (j == 5) // map gyro Z to accel Y
		{
			
			// Write the minima, maxima, and period of the stride to the training file in this order
			k = 0;

				// print header
				num_samples = n_P-1;
				if (num_samples > n_samples_ay) num_samples = n_samples_ay;
				fprintf(fpt, "%d %d %d", num_samples, num_features, num_speeds);

			
			for (i = 0; (i < n_P-1) && (k < n_S-1); i++)
			{
				//stride_idx1 = (int) S_i[k];
				//stride_idx2 = (int) S_i[k+1];
				peak_idx1 = (int) P_i[i];
				peak_idx2 = (int) P_i[i+1];
				trough_idx = (int) T_i[i];
				trough_idx1 = (int) T_i[i];
				trough_idx2 = (int) T_i[i+1];
				
				minima_gz[i] = scale_slope * x[trough_idx] + scale_offset;                            // minima is the trough
				maxima_gz[i] = scale_slope * x[peak_idx1] + scale_offset;                             // maxima is the peak
				period_gz[i] = period_slope * (t[trough_idx2] - t[trough_idx1]) + period_offset;          // period is the time from trough to trough
				if (period_gz[i] > 1) period_gz[i] = 1;  // in case period exceeds 1

				if (minima_ay[i] == 0 && maxima_ay[i]==0 && period_ay[i] == 0) continue;

				// stride segmentation
				double *segment_start = z_gyro, *segment_map = y_accel;
				int m;
				for (m = 0; m <= trough_idx1; m++)
				{
					segment_start++;
					segment_map++;
				}

				int segment_length = 0;
				segment_length = trough_idx2 - trough_idx1 +1;

				// do i need to allocate memory here?
				double *segmentation_result;
				segmentation_result = stride_segmentation(segment_start, segment_length);


				// map gyro_z to y_accel
				// y_accel[trough_idx1] to y_accel[trough_idx2]
				double *segmentation_map_result;
				segmentation_map_result = stride_segmentation_3(segment_map, segment_length);


				fprintf(fpt, "\n%10.8lf %10.8lf %10.8lf %10.8lf %10.8lf %10.8lf", minima_ay[i], maxima_ay[i], period_ay[i], minima_gz[i], maxima_gz[i], period_gz[i]);
				int k=0;
				for(k=0; k<16;k++) fprintf(fpt, " %10.8lf", segmentation_result[k]);
				for(k=0; k<12;k++) fprintf(fpt, " %10.8lf", segmentation_map_result[k]);
				fprintf(fpt, "\n");

			
				if (mode == 1) // walking speed 1
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", 1, -1, -1, -1, -1, -1, -1);
				}
				
				else if (mode == 2) // walking speed 2
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, 1, -1, -1, -1, -1, -1);
				}
				
				else if (mode == 3) // walking speed 3
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, -1, 1, -1, -1, -1, -1);
				}
				
				else if (mode == 4) // walking speed 4
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, -1, -1, 1, -1, -1, -1);
				}

				else if (mode == 5) // walking speed 2
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, -1, -1, -1, 1, -1, -1);
				}
				
				else if (mode == 6) // walking speed 3
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, -1, -1, -1, -1, 1, -1);
				}
				
				else if (mode == 7) // walking speed 4
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", -1, -1, -1, -1, -1, -1, 1);
				}
				else
				{
					fprintf(fpt, "%d %d %d %d %d %d %d", 0, 0, 0, 0, 0, 0, 0);
				}				
				k = k + 1;
			}
		}
	}
    fclose(fpp);
    fclose(fps);
	fclose(fpt);
	remove("data.csv");
	printf("Stride detection applied to 6 degrees successfully.\n");

	neural_network_result("trd.txt");
}
}
	return 0;
}
