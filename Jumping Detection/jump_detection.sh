#!/bin/bash

echo ""
echo ">>>>COMPILING<<<<"
make clean
make

echo ""
echo ">>>>DATA COLLECTION<<<<"
./producer &
PRODUCER_ID=$!

echo ""
echo ">>>>JUMP DETECTION<<<<"
./jump_detection &
JUMP_DETECTION_ID=$!

echo ""
read -p "Press any key to kill producer/consumer... " -n1 -s
kill $PRODUCER_ID $JUMP_DETECTION_ID
