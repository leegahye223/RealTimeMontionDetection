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
echo ">>>>EXECUTING turn_detection on zigzag<<<<"
./turn_detection \
	zigzag.csv \
	zigzag_peaksAndTroughs.csv \
	zigzag_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file_zigzag.txt \
	1 \
    zigzag_turns.csv\
    training_file_turn_zigzag.txt\

read -p "Press any key to run " -n1 -s

echo ""
echo ">>>>EXECUTING turn_detection on zigzag2<<<<"
./turn_detection \
	zigzag2.csv \
	2zigzag_peaksAndTroughs.csv \
	2zigzag_strides.csv \
	0 \
	0 \
	0 \
	0 \
	0 \
	0 \
	training_file_zigzag2.txt \
	1 \
    2zigzag_turns.csv\
    training_file_turn_zigzag2.txt\