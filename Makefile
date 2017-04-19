all: stride_detection

stride_detection: stride_detection.c
	gcc -lmraa -lm -o stride_detection_turn stride_detection_turn.c

clean:
	rm -rf stride_detection_turn *~