GCC_TOOLCHAIN_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux
SYSROOT_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux/sysroot
LLVM=/home/rahat/mphil-project/local/installs/riscv-llvm

# CC=$(LLVM)/bin/clang
CC=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux/bin/riscv64-unknown-linux-gnu-gcc
ARCH=rv64g

# COMPILE_OPTS=--sysroot=$(SYSROOT_DIR) --gcc-toolchain=$(GCC_TOOLCHAIN_DIR) -march=$(ARCH) -mabi=lp64d -O3 -g
COMPILE_OPTS=-march=$(ARCH) -O3 -g
all: s276 s2101 rvv_swap main cleanO

util:
	$(CC) -march=rv64g -mabi=lp64d -I./ -O3  -g -c ./util.c

s276: util
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL tsvc276.c -o prog_tsvc276_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_A tsvc276.c -o prog_tsvc276_a util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_B tsvc276.c -o prog_tsvc276_b util.o
s2101: util
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL tsvc2101.c -o prog_tsvc2101_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_A tsvc2101.c -o prog_tsvc2101_a util.o
rvv_swap:
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL rvv_swp.c -o prog_rvvswp_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_A rvv_swp.c -o prog_rvvswp_a util.o
main:
	$(CC) $(COMPILE_OPTS) -I./ main.c -o main

cleanO:
	rm -rf *.o

clean: cleanO
	rm -rf prog_*