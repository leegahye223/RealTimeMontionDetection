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
echo ">>>>EXECUTING stride_detection on wstr<<<<"
./stride_detection \
	wstr.csv \
	wstr_peaksAndTroughs.csv \
	wstr_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file_wstr.txt \
	1 \
    wstr_turns.csv\
