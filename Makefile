CC = gcc
AR = ar
FLAGS = -g -flto

.PHONY = all clean

TESTS = f477 radius-calculator euclidean-calculator

all: build/libwgs84.a $(addprefix build/,$(TESTS))

build/libwgs84.a: build/wgs84.o | build
	$(AR) rcs build/libwgs84.a build/wgs84.o

build/wgs84.o: src/wgs84.c src/wgs84.h build/cc.inc build/cs.inc build/hc.inc build/hs.inc | build
	$(CC) $(FLAGS) -Ibuild -c src/wgs84.c -o build/wgs84.o

build/%.inc: build/const-writer data/CORCOEF data/EGM96 | build
	./build/const-writer

build/const-writer: build-src/const-writer.c | build
	$(CC) $(FLAGS) build-src/const-writer.c -o build/const-writer

build/%: build/libwgs84.a test/%.c | build
	$(CC) $(FLAGS) -Isrc test/$*.c build/libwgs84.a -lm -o $@

build:
	mkdir build

clean:
	rm -r build
