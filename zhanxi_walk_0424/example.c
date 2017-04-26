#include <stdio.h>
#include <mraa/i2c.h>
#include "LSM9DS0.h"

int main() {
	data_t accel_data, gyro_data, mag_data;
	data_t gyro_offset;
	int16_t temperature;
	float a_res, g_res, m_res;
	mraa_i2c_context accel, gyro, mag;
	accel_scale_t a_scale = A_SCALE_4G;
	gyro_scale_t g_scale = G_SCALE_245DPS;
	mag_scale_t m_scale = M_SCALE_2GS;

	//initialize sensors, set scale, and calculate resolution.
	accel = accel_init();
	set_accel_scale(accel, a_scale);	
	a_res = calc_accel_res(a_scale);
	
	gyro = gyro_init();
	set_gyro_scale(gyro, g_scale);
	g_res = calc_gyro_res(g_scale);
	
	mag = mag_init();
	set_mag_scale(mag, m_scale);
	m_res = calc_mag_res(m_scale);

	gyro_offset = calc_gyro_offset(gyro, g_res);
	
	printf("x: %f y: %f z: %f\n", gyro_offset.x, gyro_offset.y, gyro_offset.z);

	printf("\n\t\tAccelerometer\t\t\t||");
	printf("\t\t\tGyroscope\t\t\t||");
	printf("\t\t\tMagnetometer\t\t\t||");
	printf("\tTemperature\n");
	
	//Read the sensor data and print them.
	while(1) {
		accel_data = read_accel(accel, a_res);
		gyro_data = read_gyro(gyro, g_res);
		mag_data = read_mag(mag, m_res);
		temperature = read_temp(accel); //you can put mag as the parameter too.
  		printf("X: %f\t Y: %f\t Z: %f\t||", accel_data.x, accel_data.y, accel_data.z);
  		printf("\tX: %f\t Y: %f\t Z: %f\t||", gyro_data.x - gyro_offset.x, gyro_data.y - gyro_offset.y, gyro_data.z - gyro_offset.z);
		printf("\tX: %f\t Y: %f\t Z: %f\t||", mag_data.x, mag_data.y, mag_data.z);
		printf("\t%ld\n", temperature);
		usleep(100000);
	}	
	return 0;	
}
