CC=gcc
LDFLAGS=-lmraa -lfann
CFLAGS=-Wall
SOURCES=test_neural_network.c train_neural_net.c
EXECUTABLES=$(SOURCES:.c=)

all: test_neural_network train_neural_net

test_neural_network: test_neural_network.c
	$(CC) $(CFLAGS) -o test_neural_network test_neural_network.c $(LDFLAGS)

train_neural_net: train_neural_net.c
	$(CC) $(CFLAGS) -o train_neural_net train_neural_net.c $(LDFLAGS)

clean: 
	rm -f test_neural_network train_neural_net
	rm -f *~
	rm -f train_data.net
