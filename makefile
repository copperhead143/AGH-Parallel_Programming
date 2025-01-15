# kompilator c
CCOMP = /usr/bin/mpicc

# konsolidator
LOADER = /usr/bin/mpicc

MPIRUN = /usr/bin/mpiexec 

# opcje optymalizacji
OPT = -O2 -fopenmp

# biblioteki
LIB = -lm

# zaleznosci i komendy
all: oblicz_PI run

oblicz_PI: oblicz_PI.o
	$(LOADER) $(OPT) oblicz_PI.o -o oblicz_PI $(LIB)

oblicz_PI.o: oblicz_PI.c
	$(CCOMP) -c $(OPT) oblicz_PI.c

run:
	$(MPIRUN) -np 2 --oversubscribe ./oblicz_PI

clean:
	rm -f *.o oblicz_PI