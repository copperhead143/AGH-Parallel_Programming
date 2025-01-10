# kompilator c
CCOMP = gcc
# konsolidator
LOADER = gcc

# opcje optymalizacji:
# wersja do debugowania
#OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
OPT = -O3

all: fork clone kodzik print_info

# pliki naglowkowe
INC = -I../pomiar_czasu

# biblioteki
LIB = -L../pomiar_czasu -lpomiar_czasu -lm

# zaleznosci i komendy
fork: fork.o 
	$(LOADER) $(OPT) fork.o -o fork $(LIB)

# jak uzyskac plik fork.o ?
fork.o: fork.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) fork.c $(INC) 

# zaleznosci i komendy
clone: clone.o 
	$(LOADER) $(OPT) clone.o -o clone $(LIB)

# jak uzyskac plik clone.o ?
clone.o: clone.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) clone.c $(INC) 

# dopisany program
kodzik: kodzik.o 
	$(LOADER) $(OPT) kodzik.o -o kodzik $(LIB)

kodzik.o: kodzik.c ../pomiar_czasu/pomiar_czasu.h
	$(CCOMP) -c $(OPT) kodzik.c $(INC)

# nowy program
print_info: print_info.o
	$(LOADER) $(OPT) print_info.o -o print_info

print_info.o: print_info.c
	$(CCOMP) -c $(OPT) print_info.c

clean:
	rm -f *.o fork clone kodzik print_info