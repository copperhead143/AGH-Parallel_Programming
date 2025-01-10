# kompilator c
CCOMP = gcc

# archiwizator bibliotek
AR = ar -rs

# opcje optymalizacji:
# wersja do debugowania
#OPT = -g -DDEBUG
# wersja zoptymalizowana do mierzenia czasu
OPT = -O3 -fPIE

# pliki naglowkowe
INC = -I../pomiar_czasu

# zaleznosci i komendy
pomiar_czasu: pomiar_czasu.o
	$(AR) libpomiar_czasu.a pomiar_czasu.o

pomiar_czasu.o: pomiar_czasu.c pomiar_czasu.h
	$(CCOMP) -c $(OPT) pomiar_czasu.c $(INC)

obliczanie_calki.o: obliczanie_calki.c
	$(CCOMP) -c $(OPT) obliczanie_calki.c $(INC)

dekompozycja_petli.o: dekompozycja_petli.c
	$(CCOMP) -c $(OPT) dekompozycja_petli.c $(INC)

dekompozycja_obszaru.o: dekompozycja_obszaru.c
	$(CCOMP) -c $(OPT) dekompozycja_obszaru.c $(INC)

clean:
	rm -f *.o libpomiar_czasu.a

all: obliczanie_calki

obliczanie_calki: obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o libpomiar_czasu.a
	$(CCOMP) $(OPT) obliczanie_calki.o dekompozycja_petli.o dekompozycja_obszaru.o -o obliczanie_calki -L./lib -L. -L../pomiar_czasu -lpomiar_czasu -lm -lpthread