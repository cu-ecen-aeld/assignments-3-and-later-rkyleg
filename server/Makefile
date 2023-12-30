CROSS_COMPILE ?= "native"
TARGET ?= aesdsocket
OBJS=aesdsocket.c linked_list.c
CC ?= gcc
CFLAGS ?= -Wall -Wextra
LDFLAGS ?= -pthread

.SILENT: all default clean

all:
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)
default:
	# echo "$(CROSS_COMPILE)"; \
	if [ "$(CROSS_COMPILE)" = "aarch64-none-linux-gnu-" ]; then \
		aarch64-none-linux-gnu-gcc $(OBJ) -o $(TARGET); \
	elif [ "$(CROSS_COMPILE)" = "native" ]; then \
		$(CC) $(CFLAGS) $(INCLUDES) $(OBJ) -o $(TARGET) $(LDFLAGS); \
	else \
		echo "incorrect arguments"; \
	fi \

clean:
	rm -f $(TARGET)
	rm -f *.o
