CC=gcc
LDFLAGS=-lmraa -lfann
SOURCES=jump_detection.c producer.c LSM9DS0.c
EXECUTABLES=$(SOURCES:.c=)

all: producer jump_detection

producer: producer.c
	$(CC) $(LDFLAGS) -o producer producer.c LSM9DS0.c

jump_detection: jump_detection.c
	$(CC) $(LDFLAGS) -o jump_detection jump_detection.c

clean:
	rm -rf jump_detection producer *~
	rm -rf jump_peaksAndTroughs.csv jumps.csv jump_peaks_troughs_filtered.csv jump_training_file.txt
	rm -rf data_*
	