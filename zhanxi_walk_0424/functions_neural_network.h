#ifndef MY_FUNCTIONS_NEURAL_NETWORK_H
#define MY_FUNCTIONS_NEURAL_NETWORK_H

#include <unistd.h>
#include <mraa/aio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "floatfann.h"

#define BUFF_SIZE 1024

int find_max_index(fann_type *arr, int n);

void neural_network_result(char *ifile_name);

#endif

