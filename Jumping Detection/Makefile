all: stride_jump_detection

stride_jump_detection: stride_jump_detection.c
	gcc -lmraa -o stride_jump_detection stride_jump_detection.c

clean:
	rm -rf stride_jump_detection *~
	rm -rf jump_low_peaksAndTroughs.csv
	rm -rf jump_low_strides.csv
	rm -rf jumps_low.csv
	rm -rf jump_low_training_file.txt
	rm -rf jump_high_peaksAndTroughs.csv
	rm -rf jump_high_strides.csv
	rm -rf jumps_high.csv
	rm -rf jump_high_training_file.txt
