# kompilator c
CCOMP = /usr/bin/mpicc

# konsolidator
LOADER = /usr/bin/mpicc

MPIRUN = /usr/bin/mpiexec 

# Dwie wersje optymalizacji dla testów
OPT_HIGH = -O2 -fopenmp    # wersja zoptymalizowana
OPT_NONE = -O0 -fopenmp    # wersja niezoptymalizowana

# biblioteki
LIB = -lm

# Kompilacja obu wersji programu
all: mat_vec_row_MPI_opt mat_vec_row_MPI_unopt

# Wersja zoptymalizowana
mat_vec_row_MPI_opt: mat_vec_row_MPI.c
	$(LOADER) $(OPT_HIGH) mat_vec_row_MPI.c -o mat_vec_row_MPI_opt $(LIB)

# Wersja niezoptymalizowana
mat_vec_row_MPI_unopt: mat_vec_row_MPI.c
	$(LOADER) $(OPT_NONE) mat_vec_row_MPI.c -o mat_vec_row_MPI_unopt $(LIB)

# Test skalowalności słabej (zoptymalizowany)
run_weak: mat_vec_row_MPI_opt
	@echo "=== Weak Scaling Tests (Optimized) ==="
	$(MPIRUN) -np 1 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 2 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 4 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 8 --oversubscribe ./mat_vec_row_MPI_opt

# Test skalowalności silnej (zoptymalizowany)
run_strong_opt: mat_vec_row_MPI_opt
	@echo "=== Strong Scaling Tests (Optimized) ==="
	$(MPIRUN) -np 1 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 2 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 4 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 8 --oversubscribe ./mat_vec_row_MPI_opt

# Test skalowalności silnej (niezoptymalizowany)
run_strong_unopt: mat_vec_row_MPI_unopt
	@echo "=== Strong Scaling Tests (Unoptimized) ==="
	$(MPIRUN) -np 1 --oversubscribe ./mat_vec_row_MPI_unopt
	$(MPIRUN) -np 2 --oversubscribe ./mat_vec_row_MPI_unopt
	$(MPIRUN) -np 4 --oversubscribe ./mat_vec_row_MPI_unopt
	$(MPIRUN) -np 8 --oversubscribe ./mat_vec_row_MPI_unopt

# Zachowanie oryginalnego targetu run
run: mat_vec_row_MPI_opt
	$(MPIRUN) -np 1 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 2 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 4 --oversubscribe ./mat_vec_row_MPI_opt
	$(MPIRUN) -np 8 --oversubscribe ./mat_vec_row_MPI_opt

clean:
	rm -f *.o ./mat_vec_row_MPI_opt ./mat_vec_row_MPI_unopt