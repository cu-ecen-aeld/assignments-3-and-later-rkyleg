CROSS_COMPILE ?= "native"
TARGET=aesdsocket
OBJS=aesdsocket.c
CC=gcc
CFLAGS=-Wall -Wextra

.SILENT: all default clean

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)
default:
	# echo "$(CROSS_COMPILE)"; \
	if [ "$(CROSS_COMPILE)" = "aarch64-none-linux-gnu-" ]; then \
		aarch64-none-linux-gnu-gcc $(OBJ) -o $(TARGET); \
	elif [ "$(CROSS_COMPILE)" = "native" ]; then \
		gcc $(OBJ) -o $(TARGET); \
	else \
		echo "incorrect arguments"; \
	fi \

clean:
	rm -f $(TARGET)
	rm -f *.o
