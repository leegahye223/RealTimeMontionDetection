// gcc -lmraa -o data_collection_realtime data_collection_realtime.c LSM9DS0.c 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
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

int main() {
    
    mraa_i2c_context 	accel, gyro;
    accel_scale_t 		a_scale = A_SCALE_8G;
    gyro_scale_t 		g_scale = G_SCALE_500DPS;
    
    data_t              accel_data, gyro_data;
    float               a_res, g_res;
    
    char                *file_name;
    FILE                *fp;
    struct timeval 		tv_before, tv_after;
    
    //pointers for storing data in buffer1
    float *b_x, *b_y, *b_z, *b_gx, *b_gy, *b_gz;
    double *b_t_before, *b_t_after;
    
    int buffer_size = 2000;
    int num_b = 0;//number of data in buffer in current round
    int i;
    
    file_name = "data.csv";
    
    b_t_before = (double *) malloc(sizeof(double) * buffer_size);
    b_t_after = (double *) malloc(sizeof(double) * buffer_size);
    b_x = (float *) malloc(sizeof(float) * buffer_size);
    b_y = (float *) malloc(sizeof(float) * buffer_size);
    b_z = (float *) malloc(sizeof(float) * buffer_size);
    b_gx = (float *) malloc(sizeof(float) * buffer_size);
    b_gy = (float *) malloc(sizeof(float) * buffer_size);
    b_gz = (float *) malloc(sizeof(float) * buffer_size);
    
    
    //initialize sensors, set scale, and calculate resolution.
    accel = accel_init();
    set_accel_scale(accel, a_scale);
    a_res = calc_accel_res(a_scale);
    
    gyro = gyro_init();
    set_gyro_scale(gyro, g_scale);
    g_res = calc_gyro_res(g_scale);
    
    while (run_flag) {
        
        gettimeofday(&tv_before, NULL);
        accel_data = read_accel(accel, a_res);
        gyro_data = read_gyro(gyro, g_res);
        gettimeofday(&tv_after, NULL);
        
        b_x[num_b] = accel_data.x/8;
        b_y[num_b] = accel_data.y/8;
        b_z[num_b] = accel_data.z/8;
        b_gx[num_b] = gyro_data.x/500;
        b_gy[num_b] = gyro_data.y/500;
        b_gz[num_b] = gyro_data.z/500;
        b_t_before[num_b] = parse_tv(&tv_before);
        b_t_after[num_b] = parse_tv(&tv_after);
        
        if (num_b < buffer_size - 1)
        {
            num_b++;
        }
        else
        {
            
            fp = fopen(file_name, "r");
            

            if (fp == NULL)
            {
                
                fp = fopen(file_name, "w");
            
                fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%s\n",
                        "timestamp_before",
                        "timestamp_after",
                        "accel_x", "accel_y", "accel_z",
                        "gyro_x", "gyro_y", "gryo_z"
                        );
            
            
            
                for (i = 0; i < num_b; i++)
                {
                    fprintf(fp, "%20.10lf, %20.10lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf, %8.4lf\n", b_t_before[i], b_t_after[i], b_x[i], b_y[i], b_z[i], b_gx[i], b_gy[i], b_gz[i]);
                }
            
                fclose(fp);
                
                num_b = 0;
                continue;

            }
            else
            {
                printf("The file was not removed by the main program\n");
                exit(EXIT_FAILURE);
            }
        
        }
        
    usleep(500);
    }
}









