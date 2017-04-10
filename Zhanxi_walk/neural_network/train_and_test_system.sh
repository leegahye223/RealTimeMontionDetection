#!/bin/bash

echo ""
echo ">>>>COMPILING<<<<"
make clean
make

echo ""
echo ">>>>TRAINING neural network<<<<"
./train_neural_net

echo ""
echo ">>>>TESTING neural network<<<<"
./test_neural_network
