#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <mraa/i2c.h>
#include <sys/time.h>
#include "LSM9DS0.h"

#define MILLION 1000000.0f

sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig) 
{
	if (sig == SIGINT)
		run_flag = 0;
}

double parse_tv(struct timeval *tv)
{
	return (double) tv->tv_sec + (double) tv->tv_usec/MILLION;
}

int main(int argc, char **argv) {
	mraa_i2c_context 	accel, gyro;
	accel_scale_t 		a_scale = A_SCALE_6G;
	gyro_scale_t 		g_scale = G_SCALE_500DPS;
	
	data_t 			accel_data, gyro_data;
	float 			a_res, g_res;

	char			*file_name;
	FILE			*fp;
	struct timeval 		tv_before, tv_after;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <file_name>\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, &do_when_interrupted);
	
	file_name = argv[1];
	fp = fopen(file_name, "w");
	if (fp == NULL) {
		fprintf(stderr, "Failed to open file \'%s\'. Exiting.\n",
				file_name
		       );
		exit(EXIT_FAILURE);
	}

	//initialize sensors, set scale, and calculate resolution.
	accel = accel_init();
	set_accel_scale(accel, a_scale);	
	a_res = calc_accel_res(a_scale);

	gyro = gyro_init();
	set_gyro_scale(gyro, g_scale);
	g_res = calc_gyro_res(g_scale);

	fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s\n",
			"timestamp_before",
			"timestamp_after",
			"accel_x", "accel_y", "accel_z",
			"gyro_x", "gyro_y", "gryo_z"
	       );
	//Read the sensor data and print them.
	while (run_flag) {
		gettimeofday(&tv_before, NULL);
		accel_data = read_accel(accel, a_res);
        accel_data = accel_data;
		gyro_data = read_gyro(gyro, g_res);
        gyro_data = gyro_data;
		gettimeofday(&tv_after, NULL);
		fprintf(fp, "%20.10lf,%20.10lf,",
			       	parse_tv(&tv_before),
				parse_tv(&tv_after)
		       );
		fprintf(fp, "%8.4lf,%8.4lf,%8.4lf,",
				accel_data.x/6,
				accel_data.y/6,
				accel_data.z/6
		       );
		fprintf(fp, "%8.4lf,%8.4lf,%8.4lf\n",
				gyro_data.x/500,
				gyro_data.y/500,
				gyro_data.z/500
		      );
		usleep(200);
	}	
	fclose(fp);
	return 0;	
}
