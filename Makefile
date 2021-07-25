CC = gcc
LIKWID_PATH = /home/soft/likwid
SRC = matmult.c matriz.c
OBJS = matmult.o matriz.o
RM = rm -f
CFLAGS = -O3 -mavx2 -march=native
OUTPUT = matmult

all: $(OBJS)
	$(CC) -L$(LIKWID_PATH)/lib $(OBJS) -o $(OUTPUT) -llikwid
	
matmult.o: matmult.c
	$(CC) $(CFLAGS) -DLIKWID_PERFMON -I${LIKWID_PATH}/include -c matmult.c  

matriz.o: matriz.c matriz.h
	$(CC) $(CFLGS) -DLIKWID_PERFMON -I${LIKWID_PATH}/include -c matriz.c

clean:
	$(RM) *.tmp
	$(RM) $(OBJS)

purge: clean
	$(RM) $(OUTPUT)

run: 
	likwid-perfctr -C 3 -g FLOPS_DP -m "./$(OUTPUT) -n 1024"
