#ifndef MY_FUNCTION_WALK_RUN_H
#define MY_FUNCTION_WALK_RUN_H

/* for file and terminal I/O */
#include <stdio.h>
/* for string manip */
#include <string.h>
/* for exit() */
#include <stdlib.h>
/* for fabsf() */
#include <math.h>

#define BUFF_SIZE 1024

void clear_buffer(double *arr, double val, int n);

double calculate_mean(double *arr, int n);

double calculate_min(double *arr, int n);

double calculate_max(double *arr, int n);

double calculate_range(double *arr, int n);

double calculate_mean_righthalf(double *arr, int n);

double calculate_dc_offset(double *arr, int n);

double calculate_std(double *arr, int n);

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
		);
double *stride_segmentation(double *arr, int n);
double *stride_segmentation_3(double *arr, int n);

#endif

