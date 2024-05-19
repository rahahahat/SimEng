CC=/home/rahat/local/rv-gnu-tc-install/bin/riscv64-unknown-elf-gcc
ARCH=rv64gc


ifdef rvv
	$(ARCH)=rv64gcv
endif

COMPILE_OPTS=-march=$(ARCH) -mabi=lp64d -O3 -g

all: s276 s2101 rvv_swap cleanO

util:
	$(CC) -march=rv64gc -mabi=lp64d -I./ -O3  -g -c ./util.c

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

cleanO:
	rm -rf *.o

clean: cleanO
	rm -rf prog_*