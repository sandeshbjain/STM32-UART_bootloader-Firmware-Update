CC = gcc
CFLAGS = -std=gnu99

all: send_binary.c RS232_library/rs232.c
	$(CC) $(CFLAGS) -o sendfile send_binary.c RS232_library/rs232.c

clean:
	rm -f sendfile
