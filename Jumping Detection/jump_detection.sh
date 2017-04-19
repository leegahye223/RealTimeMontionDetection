#!/bin/bash

echo ""
echo ">>>>COMPILING<<<<"
make clean
make


echo ""
echo ">>>>EXECUTING jumping on jump_low data set<<<<"
./jumping \
	jump_low_data.csv \
	jump_low_peaksAndTroughs.csv \
	0.12 \
	0.75 \
	0.8 \
	0.8 \
	50.0 \
	30.0 \
	90.0 \
	jump_low_training_file.txt \
	jumps_low.csv \
	1.50 \
	1.0 \


echo ""
echo ">>>>EXECUTING jumping on jump_high data set<<<<"
./jumping \
	jump_high_data.csv \
	jump_high_peaksAndTroughs.csv \
	0.3 \
	1.3 \
	0.8 \
	0.8 \
	50.0 \
	30.0 \
	90.0 \
	jump_high_training_file.txt \
	jumps_high.csv \
	4.00 \
	2.0 \
	