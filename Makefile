CC=gcc
CXX=g++ -std=c++11
CFLAGS=-Wall -Werror -ggdb -O3 -march=native -mtune=native
CXXFLAGS=$(CFLAGS)

BENCHES=bench-zlib bench-slice4 bench-slice8 bench-slice16 bench-slice16-prefetch bench-kernel bench-oldcyrus bench-cyrus bench-crcutil bench-lizardfs

all: $(BENCHES)

bench-lizardfs: crc32-lizardfs.o bench.o
	$(CXX) -o $@ $^
bench-crcutil: crcutil.o bench.o bench-init.o
	$(CXX) -o $@ $^ -lcrcutil
bench-zlib: bench.o bench-init.o
	$(CC) -o $@ $^ -lz
bench-slice4: bench.o bench-init.o crc32-slice4.o
	$(CC) -o $@ $^
bench-slice8: bench.o bench-init.o crc32-slice8.o
	$(CC) -o $@ $^
bench-slice16: bench.o bench-init.o crc32-slice16.o
	$(CC) -o $@ $^
bench-slice16-prefetch: bench.o bench-init.o crc32-slice16-prefetch.o
	$(CC) -o $@ $^
bench-kernel: bench.o crc32-kernel.o
	$(CC) -o $@ $^
bench-oldcyrus: bench.o bench-init.o crc32-oldcyrus.o
	$(CC) -o $@ $^
bench-cyrus: bench.o bench-init.o crc32-cyrus.o
	$(CC) -o $@ $^

mkslicetables: mkslicetables.o
	$(CC) -o $@ $^

clean:
	rm -f $(BENCHES) *.o
