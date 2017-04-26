/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

#include "functions.h"

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

