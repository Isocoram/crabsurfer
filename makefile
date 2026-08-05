CC = gcc
CFLAGS = -O0

crabsurfer: crabsurfer.c 
	$(CC) $(CFLAGS) crabsurfer.c -o crabsurfer

.PHONY: clean
clean:
	rm -f crabsurfer
