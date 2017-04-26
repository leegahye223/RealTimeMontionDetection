#!/bin/bash


echo ""
echo ">>>>collecting data<<<<"
./data_collection_realtime &

echo ""
echo ">>>>classifying motion<<<<"
./stride_detection_1 data.csv pd.csv sd.csv 0 0 0 0 0 0 trd.txt 0 &

