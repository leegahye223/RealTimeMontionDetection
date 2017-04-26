#!/bin/bash

if [ $1 ]; then
		threshold=$1
else
		threshold=6
fi

echo ""
echo ">>>>COMPILING<<<<"
make clean
make


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_1 data set<<<<"
./stride_detection \
	50sec_0405.csv \
	walk_speed_1_peaksAndTroughs.csv \
	walk_speed_1_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file1.txt \
	1 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_2 data set<<<<"
./stride_detection \
	35sec_0405.csv \
	walk_speed_2_peaksAndTroughs.csv \
	walk_speed_2_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file2.txt \
	2 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_3 data set<<<<"
./stride_detection \
	25sec_0405.csv \
	walk_speed_3_peaksAndTroughs.csv \
	walk_speed_3_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file3.txt \
	3 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_4 data set<<<<"
./stride_detection \
	15sec_0405.csv \
	walk_speed_4_peaksAndTroughs.csv \
	walk_speed_4_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file4.txt \
	4 \

echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_4 data set<<<<"
./stride_detection \
	run_slow_0412.csv \
	run_slow_peaksAndTroughs.csv \
	run_slow_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file5.txt \
	5 \

echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_4 data set<<<<"
./stride_detection \
	run_med_0412.csv \
	run_med_peaksAndTroughs.csv \
	run_med_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file6.txt \
	6 \

echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_4 data set<<<<"
./stride_detection \
	run_fast_0412.csv \
	run_fast_peaksAndTroughs.csv \
	run_fast_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file7.txt \
	7 \

echo ""
