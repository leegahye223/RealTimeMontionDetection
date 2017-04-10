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
	walk_50sec_0410.csv \
	walk_speed_1_peaksAndTroughs.csv \
	walk_speed_1_strides.csv \
	0.8 \
	0.8 \
	0.8 \
	50.0 \
	30.0 \
	90.0 \
	test_file1.txt \
	1 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_2 data set<<<<"
./stride_detection \
	walk_35sec_0410.csv \
	walk_speed_2_peaksAndTroughs.csv \
	walk_speed_2_strides.csv \
	1.0 \
	1.0 \
	1.8 \
	50.0 \
	40.0 \
	100.0 \
	test_file2.txt \
	2 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_3 data set<<<<"
./stride_detection \
	walk_25sec_0410.csv \
	walk_speed_3_peaksAndTroughs.csv \
	walk_speed_3_strides.csv \
	2.2 \
	4.0 \
	1.2 \
	50.0 \
	30.0 \
	90.0 \
	test_file3.txt \
	3 \


echo ""
echo ">>>>EXECUTING stride_detection on walk_speed_4 data set<<<<"
./stride_detection \
	walk_15sec_0410.csv \
	walk_speed_4_peaksAndTroughs.csv \
	walk_speed_4_strides.csv \
	0.8 \
	6.0 \
	0.8 \
	50.0 \
	40.0 \
	200.0 \
	test_file4.txt \
	4 \

echo ""
