CROSS_COMPILE ?= "native"
.SILENT: all default clean

all:
	# echo "$(CROSS_COMPILE)"; \
	if [ "$(CROSS_COMPILE)" = "aarch64-none-linux-gnu-" ]; then \
		aarch64-none-linux-gnu-gcc writer.c -o writer; \
	elif [ "$(CROSS_COMPILE)" = "native" ]; then \
		gcc writer.c -o writer; \
	else \
		echo "incorrect arguments"; \
	fi \

clean:
	rm -f writer
	rm -f *.o