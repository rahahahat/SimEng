GCC_TOOLCHAIN_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux
SYSROOT_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux/sysroot
LLVM=/home/rahat/mphil-project/local/installs/riscv-llvm


# GCC_TOOLCHAIN_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux
# SYSROOT_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux/sysroot
# LLVM=/home/rahat/mphil-project/local/installs/riscv-llvm

# CCFLAGS = -Wall --target=riscv64-unknown-linux-gnu --sysroot=$(SYSROOT_DIR) --gcc-toolchain=$(GCC_TOOLCHAIN_DIR) -march=rv64gc -mabi=lp64d -O3 -g -static -O3 

# main: main.c
# 	$(CC) $(CCFLAGS) main.c -o main

# clean:
# 	rm -rf *.o rijndael output*

# CC=$(LLVM)/bin/clang

CC=${LLVM}/bin/clang

# COMPILE_OPTS=--sysroot=$(SYSROOT_DIR) --gcc-toolchain=$(GCC_TOOLCHAIN_DIR) -march=$(ARCH) -mabi=lp64d -O3 -g
COMPILE_OPTS=-Wall --target=riscv64-unknown-linux-gnu --sysroot=$(SYSROOT_DIR) --gcc-toolchain=$(GCC_TOOLCHAIN_DIR) -march=rv64gcv -mabi=lp64d -O3 -g -static -O3
all: s276 s2101 rvv_swap main cleanO

util:
	$(CC) -march=rv64gc -mabi=lp64d -I./ -O3  -g -c ./util.c

s2101: util
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL tsvc2101.c -o prog_tsvc2101_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_A tsvc2101.c -o prog_tsvc2101_a util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE_B tsvc2101.c -o prog_tsvc2101_b util.o
s276: util
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL tsvc276.c -o prog_tsvc276_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE tsvc276.c -o prog_tsvc276_a util.o
rvv_swap:
	$(CC) $(COMPILE_OPTS) -I./ -DORIGINAL rvv_swp.c -o prog_rvvswp_o util.o
	$(CC) $(COMPILE_OPTS) -I./ -DREWRITE rvv_swp.c -o prog_rvvswp_a util.o
main:
	$(CC) $(COMPILE_OPTS) main.c -o main

cleanO:
	rm -rf *.o

clean: cleanO
	rm -rf prog_*