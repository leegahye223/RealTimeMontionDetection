#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <string.h>
#include <math.h>
#include <mraa/i2c.h>
#include <sys/time.h>
#include "LSM9DS0.h"
#define MILLION 1000000.0f

static volatile int run_flag = 1;
const double SAMPLE_TIME = 5.00;   // Samples 5 seconds of data at a time
const int SLEEP_TIME = 200;
void int_handler(int sig)
{
	run_flag = 0;
}

double parse_tv(struct timeval *tv)
{
    return (double) tv->tv_sec + (double) tv->tv_usec/MILLION;
}

FILE* open_file(void)
{
	FILE *fp;
	char *fname;
	int fd;

	fname = malloc(sizeof(char) * 1024);
	memset(fname, 0, 1024);
	sprintf(fname, "data_%ld.csv", time(NULL));
	//printf("Opening file \'%s\'.\n", fname);
	
	fp = fopen(fname, "w");

	if (fp == NULL) {
		fprintf(stderr, "Failed to open file \'%s\'. Exiting.\n", fname);
		exit(EXIT_FAILURE);
	}

	fd = fileno(fp);
	flock(fd, LOCK_EX);

	printf("Acquired lock for file \'%s\'.\n", fname);

	free(fname);
	return fp;
}

int main() 
{
	FILE *fp;

	signal(SIGINT, int_handler);
	
	int i;
	mraa_i2c_context 	accel, gyro;
	accel_scale_t 		a_scale = A_SCALE_8G;
	gyro_scale_t 		g_scale = G_SCALE_500DPS;
	
	data_t              accel_data, gyro_data;
	float               a_res, g_res;

	double b_t_before, b_t_after, b_x, b_y, b_z, b_gx, b_gy, b_gz;
	struct timeval tv_start, tv_before, tv_after;
	//initialize sensors, set scale, and calculate resolution.
	accel = accel_init();
	set_accel_scale(accel, a_scale);	
	a_res = calc_accel_res(a_scale);

	gyro = gyro_init();
	set_gyro_scale(gyro, g_scale);
	g_res = calc_gyro_res(g_scale);

	while (run_flag) {
		fp = open_file();
		gettimeofday(&tv_start, NULL);
		
		//printf("Started writing to file.\n");
		fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s\n",
		"timestamp_before",
		"timestamp_after",
		"accel_x", "accel_y", "accel_z",
		"gyro_x", "gyro_y", "gryo_z"
		);
		
		b_t_after = 0;
		while (b_t_after < SAMPLE_TIME) {
			//fprintf(fp, "%ld\n", i*time(NULL));
			
			//get data from sensors
			gettimeofday(&tv_before, NULL);
			accel_data = read_accel(accel, a_res);
			gyro_data = read_gyro(gyro, g_res);
			gettimeofday(&tv_after, NULL);
			
			//put data to variables 
			b_x = accel_data.x;
			b_y = accel_data.y;
			b_z = accel_data.z;
			b_gx = gyro_data.x;
			b_gy = gyro_data.y;
			b_gz = gyro_data.z;
			//printf ("gyro,  %8.4lf", b_gz);
			b_t_before = parse_tv(&tv_before) - parse_tv(&tv_start);
			b_t_after = parse_tv(&tv_after) - parse_tv(&tv_start);
			
			fprintf(fp, "%20.10lf, %20.10lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf\n",
			b_t_before, b_t_after, b_x, b_y, b_z, b_gx, b_gy, b_gz);
			usleep(SLEEP_TIME); 
		} 

		printf("Finished writing to file.\n");
		fclose(fp);
	}

	exit(EXIT_SUCCESS);
}
