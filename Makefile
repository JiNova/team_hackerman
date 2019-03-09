CFLAGS = -Wall
CC = gcc

source=$(wildcard *.c)
binaries=$(wildcard *.exec)

obj = $(source:.c=)

all: $(obj)

$(obj): %: %.c
	$(CC) $(CFLAGS) -o $@.exec $<
	
clean:  
	rm -f $(binaries)