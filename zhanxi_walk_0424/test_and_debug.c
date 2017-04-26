// for testing


// check period
// map gyro z to accel y


#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

double find_range(double *arr, int n)
{
	double min=0;
	double max=0;
	double range = 0;
	for (int i=0;i<n;i++)
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

printf("The max is %10.8lf \n", max);
printf("The min is %10.8lf \n", min);
	printf("The range is %10.8lf \n", range);
	return range;

}

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

int main()
{
			int i, j, idx;
	int rv;
		double *t_accel, *t_gyro, *x_accel, *y_accel, *z_accel; 	// variables for data collected from input file
	double *x_gyro, *y_gyro, *z_gyro;
char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int N_SAMPLES;
	char *ifile_name, *ofile_pt_name, *ofile_st_name, *train_file_name;
	FILE *fpp, *fps, *fpi, *fpt;
	double *t, *x; // pointers to data of interest

	ifile_name = "50sec_0405.csv";

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

	find_range(y_accel, N_SAMPLES);
	double offset_value;
	offset_value = calculate_dc_offset (x_gyro, N_SAMPLES);
	printf("The dc offset is %10.8lf \n", offset_value);



	return 0;
}