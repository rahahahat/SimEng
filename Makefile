
GCC_TOOLCHAIN_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux
SYSROOT_DIR=/home/rahat/mphil-project/local/installs/rv-gnu-tc-linux/sysroot
LLVM=/home/rahat/mphil-project/local/installs/riscv-llvm

CC = ${LLVM}/bin/clang

CCFLAGS = -Wall --target=riscv64-unknown-linux-gnu --sysroot=$(SYSROOT_DIR) --gcc-toolchain=$(GCC_TOOLCHAIN_DIR) -march=rv64gc -mabi=lp64d -O3 -g -static -O3 

main: main.c
	$(CC) $(CCFLAGS) main.c -o main

clean:
	rm -rf *.o rijndael output*