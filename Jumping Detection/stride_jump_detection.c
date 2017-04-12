/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

#define BUFF_SIZE 1024

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

int main(int argc, char **argv)
{
	/* Generic variables */
	int i, j, idx;
	int rv;
	/* Variables for reading file line by line */
	char *ifile_name, *ofile_pt_name, *ofile_st_name, *train_file_name, *ofile_jump_name;
	FILE *fpp, *fps, *fpi, *fpt, *fpj;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int N_SAMPLES;

	/* Variables for storing the data and storing the return values */
	double *t_accel, *t_gyro, *x_accel, *y_accel, *z_accel; 	// variables for data collected from input file
	double *x_gyro, *y_gyro, *z_gyro;
	double *t, *x; 			// pointers to data of interest
	double pk_threshold;	// pk-threshold value
       	
	/* Variables for peak-trough detection */	
	double *P_i; 	// indicies of each peak found by peak detection
	double *T_i; 	// indicies of each trough found by trough detection
	double *PT_i;   // indices of each peak and trough found by peak and trough detection
	double *S_i; 	// indicies of the start of each stride
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
	
	// Threshold Values
    double threshold_accelX;          
    double threshold_accelY;         
    double threshold_accelZ;
    double threshold_gyroX;     
    double threshold_gyroY;         
    double threshold_gyroZ; 
	double jump_threshold;

	// Mode determines which data set we are using
	double mode;

	/*
	 * set this to 0 so that the code can function without
	 * having to actually performing stride detection
	 * from peaks and troughs
	 */
	n_PT = 0;
	n_S = 0; 
	n_J = 0;
	
	// Allocate memory for the file names
	if (argc != 13)
	{
		ifile_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ifile_name, 0, BUFF_SIZE);
		ofile_pt_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_pt_name, 0, BUFF_SIZE);
		ofile_st_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_st_name, 0, BUFF_SIZE);
		train_file_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(train_file_name, 0, BUFF_SIZE);
		ofile_jump_name = (char *) malloc(sizeof(char) * BUFF_SIZE);
		memset(ofile_jump_name, 0, BUFF_SIZE);
	}
		
	// Read the values from the shell script
	else {
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
		ofile_jump_name = argv[11];
		mode = atof(argv[12]);
	}

	printf("Arguments used:\n\t%s=%s\n\t%s=%s\n\t%s=%s\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%lf\n\t%s=%s\n\t%s=%s\n\t%s=%lf\n",
			"ifile_name", ifile_name,
			"ofile_peak_trough_name", ofile_pt_name,
			"ofile_stride_name", ofile_st_name,
			"threshold_accelX", threshold_accelX,
			"threshold_accelY", threshold_accelY,
			"threshold_accelZ", threshold_accelZ,
			"threshold_gyroX", threshold_gyroX,
			"threshold_gyroY", threshold_gyroY,
			"threshold_gyroZ", threshold_gyroZ,
			"train_file_name", train_file_name,
			"ofile_jump_name", ofile_jump_name,
			"mode", mode
	      );

	/* open the input file */
	printf("Attempting to read from file \'%s\'.\n", ifile_name);
	fpi = fopen(ifile_name, "r");
	if (fpi == NULL) {
		fprintf(stderr, 
				"Failed to read from file \'%s\'.\n", 
				ifile_name
		       );
		exit(EXIT_FAILURE);
	}

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
	fclose(fpi);
    //input file closed
	/* 
	 * From selected thresholds, 
	 * find indicies of peaks
	 * find indicies of troughs
	 */
	P_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	T_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	PT_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	S_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	J_i = (double *) malloc(sizeof(double) * N_SAMPLES);
	
	/* open the output file to write the peak and trough data */
		printf("Attempting to write to file \'%s\'.\n", ofile_pt_name);
		fpp = fopen(ofile_pt_name, "w");
		if (fpp == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_pt_name
				   );
			exit(EXIT_FAILURE);
		}
	
	/* open the output file to write the stride data */
		printf("Attempting to write to file \'%s\'.\n", ofile_st_name);
		fps = fopen(ofile_st_name, "w");
		if (fps == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_st_name
				   );
			exit(EXIT_FAILURE);
		}
		
	/* open the output file to write the features of strides */
		printf("Attempting to write to file \'%s\'.\n", train_file_name);
		fpt = fopen(train_file_name, "w");
		if (fpt == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					train_file_name
				   );
			exit(EXIT_FAILURE);
		}
		
	/* open the output file to write the jump data */
		printf("Attempting to write to file \'%s\'.\n", ofile_jump_name);
		fpj = fopen(ofile_jump_name, "w");
		if (fpj == NULL) {
			fprintf(stderr, 
					"Failed to write to file \'%s\'.\n", 
					ofile_jump_name
				   );
			exit(EXIT_FAILURE);
		}
	
	
	for (j = 0; j < 6; j++) //runs 6 times for accel and gyro
	{
		// Reset number of strides and jumps back to 0 for each iteration
		n_S = 0;
		n_J = 0;
		
		switch(j)
		{//x -> column from data
			case 0: t = t_accel;
					x = x_accel;
					center = center2;
					pk_threshold = threshold_accelX;
					printf("Accel X\n");
					break;
			
			case 1:	x = y_accel;
					center = center1;
					pk_threshold = threshold_accelY;
					printf("Accel Y\n");
					break;
			
			case 2: x = z_accel;
					center = center3;
					pk_threshold = threshold_accelZ;
					printf("Accel Z\n");
					break;
			
			case 3: t = t_gyro;
					x = x_gyro;
					center = center1;
					pk_threshold = threshold_gyroX;
					printf("Gyro X\n");
					break;
			
			case 4:	x = y_gyro;
					center = center1;
					pk_threshold = threshold_gyroY;
					printf("Gyro Y\n");
					break;
			
			case 5: x = z_gyro;
					center = center1;
					pk_threshold = threshold_gyroZ;
					printf("Gyro Z\n");
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

		/* 
		 * Insert your algorithm to convert from a series of peak-trough
		 * indicies, to a series of indicies that indicate the start
		 * of a stride.
		 */

		int k, idx_next;
		double stride_time = 0.3;
		int n_P_new = n_P;
		int n_T_new = n_T;
        
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
		printf("%d peaks and ", n_P - n_P_new);
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
		printf("%d troughs removed\n", n_T - n_T_new);
		n_T = n_T_new;
         
		 
	//**********************************************START OF STRIDE DETECTION ALGORITHM**************************************************************************************************
         
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
		printf("Stride detection success.\n");
		
	//**********************************************END OF STRIDE DETECTION ALGORITHM**************************************************************************************************
	
	
	//**********************************************START OF JUMP DETECTION ALGORITHM**************************************************************************************************
	
	if (j == 0)     // Look at Accel X only
	{
		// A jump is defined by a rise and a fall
		// The rise and fall are both each defined by a trough followed by a peak
		// They must meet the timing constraint
		// The jump must have a minimum magnitude
		
		int peak_index_next = 0, trough_index_next = 0;
		int jump_started = 0;
		double jump_time = 0.25;
		double jump_threshold = 4.00;
		
		// Combine peaks and troughs in chronological order into one array
		n_PT = 0;
		for (i = 0; i < n_P-1 || i < n_T-1; i++)
		{
			if (i < n_P-1)
			{
				peak_index = (int) P_i[i];
				peak_index_next = (int) P_i[i+1];
			}
			if (i < n_T-1)
			{
				trough_index = (int) T_i[i];
				trough_index_next = (int) T_i[i+1];
			}
			
			if (t[peak_index] < t[trough_index])     // Add the peak
			{
				PT_i[n_PT] = peak_index;
				n_PT++;
				if (t[trough_index] < t[peak_index_next])     // Add the trough
				{
					PT_i[n_PT] = trough_index;
					n_PT++;
				}
			}
			else     // Add the trough
			{
				PT_i[n_PT] = trough_index;
				n_PT++;
				if (t[peak_index] < t[trough_index_next])     // Add the peak
				{
					PT_i[n_PT] = peak_index;
					n_PT++;
				}
			}
		}
		
		// Extract the relevant jump data by finding where the rise and fall occur
		for (i = 0; i < n_PT-1; i++)
		{
			idx = (int) PT_i[i];
			idx_next = (int) PT_i[i+1];
			
			// Must be a trough followed by a peak
			if ((x[idx] < center) && (x[idx_next] > center))     
			{
				// Must satisfy timing and threshold requirements
				// The trough and peak representing the "fall" is not added to the jump data
				if ((!jump_started && (t[idx_next] - t[idx] < jump_time) && (x[idx_next] - x[idx] > jump_threshold)))
				{
					J_i[n_J] = idx;
					J_i[n_J+1] = idx_next;
					n_J += 2;
					jump_started = 1;
				}
				else
					jump_started = 0;
			}
		}
		
		printf("Jump detection success.\n");
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

		// Label the data sets for strides
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
		
		// Write stride data if available
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
		
		/* Write the stride features to the training file */
		if (j == 1) // For accel Y only because walking in a straight line
		{
			int num_samples = n_P-1;
			int num_features = 3;
			int num_speeds = 4;
			double scale_slope = 0.25;
			double scale_offset = 0.00;
			double period_slope = 1.00;
			double period_offset = -1.30;
			fprintf(fpt, "%d %d %d", num_samples, num_features, num_speeds);
			
			// Write the minima, maxima, and period of the stride to the training file in this order
			k = 0;
			int stride_idx1 = 0, stride_idx2 = 0;
			int peak_idx1 = 0, peak_idx2 = 0;
			int trough_idx = 0;
			double minima, maxima, period;
			
			for (i = 0; (i < n_P-1) && (k < n_S-1); i++)
			{
				//stride_idx1 = (int) S_i[k];
				//stride_idx2 = (int) S_i[k+1];
				peak_idx1 = (int) P_i[i];
				peak_idx2 = (int) P_i[i+1];
				trough_idx = (int) T_i[i];
				
				minima = scale_slope * x[trough_idx] + scale_offset;                            // minima is the trough
				maxima = scale_slope * x[peak_idx1] + scale_offset;                             // maxima is the peak
				period = period_slope * (t[peak_idx2] - t[peak_idx1]) + period_offset;          // period is the time from peak to peak
				//period = t[stride_idx2] - t[stride_idx1];
				
				fprintf(fpt, "\n%10.8lf %10.8lf %10.8lf\n", minima, maxima, period);
			
				if (mode == 1) // walking speed 1
				{
					fprintf(fpt, "%d %d %d %d", 1, -1, -1, -1);
				}
				
				else if (mode == 2) // walking speed 2
				{
					fprintf(fpt, "%d %d %d %d", -1, 1, -1, -1);
				}
				
				else if (mode == 3) // walking speed 3
				{
					fprintf(fpt, "%d %d %d %d", -1, -1, 1, -1);
				}
				
				else if (mode == 4) // walking speed 4
				{
					fprintf(fpt, "%d %d %d %d", -1, -1, -1, 1);
				}
				
				k = k + 2;
			}
		}
	}
    fclose(fpp);
    fclose(fps);
	fclose(fpt);
	fclose(fpj);
	printf("\nFull execution successful.\n\n");

	return 0;
}
