#include <stdio.h>
#include <unistd.h>
#include <mraa/aio.h>

int main() 
{

	int i, j, success_flag;
	int location;
	int output[5];
	int temp0, temp1, temp2;
	float v0, v1, v2;
	uint16_t value0, value1, value2;
	mraa_aio_context lightsensor0, lightsensor1, lightsensor2;

	lightsensor0 = mraa_aio_init(0);
	lightsensor1 = mraa_aio_init(1);
	lightsensor2 = mraa_aio_init(2);

	// train data store to test_data.txt
	FILE *fp;
	fp = fopen("./test_data.txt", "wb");
	fprintf(fp, "250\t3\t5\n");

	// generate the test_data.txt output
	for (location = 0; location < 5; location++) {
		if (location != 4) {
			printf("Please position the Test Object to cast a shadow on region (%d).\n", location);
			printf("While you are casting this shadow and while the shadow is stable, please press the return key to capture data.\n");
		} else {
			printf("Please do not cast any shadow on the sensors and press the return key\n");
		}

		do {
			success_flag = getchar();
		} while (success_flag != '\n');
		
		for (i = 0; i < 5; i++) {
			output[i] = -1;
		}
		output[location] = 1;

		// collect 50 data per area
		for (j = 0; j < 50; j++) {

			temp0 = 0;
			temp1 = 0;
			temp2 = 0;

			// accurate the reading by using average of 50 readings
			for (i = 0; i < 50; i++) {
				temp0 += mraa_aio_read(lightsensor0);
				temp1 += mraa_aio_read(lightsensor1);
				temp2 += mraa_aio_read(lightsensor2);
				usleep(5000);
			}

			value0 = temp0 / 50;
			value1 = temp1 / 50;
			value2 = temp2 / 50;

			printf("%5d, %5d, %5d\n", value0, value1, value2);

			// transfer raw data within in the range 0 - 1, easy for FANN to train
			v0 = (float)value0 / 1000;
			v1 = (float)value1 / 1000;
			v2 = (float)value2 / 1000;
			
			//write input, output to test_data.txt as training file format
			fprintf(fp, "%f\t%f\t%f\n", v0, v1, v2);
			fprintf(fp, "%d\t%d\t%d\t%d\t%d\n", output[0], output[1], output[2], output[3], output[4]);
		}
	}

	// close everything
	fclose(fp);
	
	mraa_aio_close(lightsensor0);
	mraa_aio_close(lightsensor1);
	mraa_aio_close(lightsensor2);
	
	return 0;
}
