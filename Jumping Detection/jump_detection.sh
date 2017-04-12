#!/bin/bash

echo ""
echo ">>>>COMPILING<<<<"
make clean
make


echo ""
echo ">>>>EXECUTING stride_jump_detection on jump_low data set<<<<"
./stride_jump_detection \
	jump_low_data.csv \
	jump_low_peaksAndTroughs.csv \
	jump_low_strides.csv \
	0.8 \
	0.8 \
	0.8 \
	50.0 \
	30.0 \
	90.0 \
	jump_high_training_file.txt \
	jumps_low.csv \
	1.0 \


echo ""
echo ">>>>EXECUTING stride_jump_detection on jump_high data set<<<<"
./stride_jump_detection \
	jump_high_data.csv \
	jump_high_peaksAndTroughs.csv \
	jump_high_strides.csv \
	0.8 \
	0.8 \
	0.8 \
	50.0 \
	30.0 \
	90.0 \
	jump_low_training_file.txt \
	jumps_high.csv \
	2.0 \
	