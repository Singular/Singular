OS := $(shell uname)

CC = /usr/bin/gcc
ALL = interval.so
ifeq ($(OS),Darwin)
	All += interval.so interval.o
endif

.PHONY = all

all: $(ALL)

clean:
	rm -v $(ALL)

ifeq ($(OS),Linux)
interval.so: interval.cc interval.h
	$(CC) -shared -DDYNAMIC_VERSION -fPIC `libsingular-config --cflags` $< -o $@
else
interval.so: interval.o
	libtool -dynamic -twolevel_namespace -weak_reference_mismatches weak -undefined dynamic_lookup -o $@ $^

interval.o: interval.cc interval.h
	$(CC) -shared -DDYNAMIC_VERSION -fPIC `libsingular-config --cflags` -O -c $< -o $@
endif
