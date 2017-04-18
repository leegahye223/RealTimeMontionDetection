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
echo ">>>>EXECUTING stride_detection on stair_up_fast data set<<<<"
./stair_stride_detection \
	stair_up_fast.csv \
	stair_up_fast_peaksAndTroughs.csv \
    stair_up_fast_strides.csv \
	0.12 \
	0.6 \
	0.2 \
	0.36 \
	0.4\
	0.4\
	stair_up_fast_detection.csv



echo ""
echo ">>>>EXECUTING stride_detection on stair_down_fast data set<<<<"
./stair_stride_detection \
	stair_down_fast.csv \
	stair_down_fast_peaksAndTroughs.csv \
    stair_down_fast_strides.csv \
	0.4\
	0.3\
	0.25 \
	0.3 \
	0.25\
	0.45 \
	stair_down_fast_detection.csv




echo ""
echo ">>>>EXECUTING stride_detection on stair_up_slow data set<<<<"
./stair_stride_detection \
	stair_up_slow.csv \
	stair_up_slow_peaksAndTroughs.csv \
    stair_up_slow_strides.csv\
	0.01\
	0.2 \
	0.1 \
	0.12 \
	0.12\
	0.2\
	stair_up_slow_detection.csv


echo ""
echo ">>>>EXECUTING stride_detection on stair_down_slow data set<<<<"
./stair_stride_detection \
	stair_down_slow.csv \
	stair_down_slow__peaksAndTroughs.csv \
    stair_down_slow_strides.csv \
	0.15 \
	0.18 \
	0.2 \
	0.15 \
	0.1 \
	0.25\
	stair_down_slow_detection.csv
