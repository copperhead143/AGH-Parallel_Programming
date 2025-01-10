#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int zmienna_wspolna = 0;

#define WYMIAR 1000
#define ROZMIAR WYMIAR *WYMIAR
double a[ROZMIAR], b[ROZMIAR], c[ROZMIAR];

double czasozajmowacz() {
  int i, j, k;
  int n = WYMIAR;
  for (i = 0; i < ROZMIAR; i++)
    a[i] = 1.0 * i;
  for (i = 0; i < ROZMIAR; i++)
    b[i] = 1.0 * (ROZMIAR - i);
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      c[i + n * j] = 0.0;
      for (k = 0; k < n; k++) {
        c[i + n * j] += a[i + n * k] * b[k + n * j];
      }
    }
  }
  return (c[ROZMIAR - 1]);
}

void *zadanie_watku(void *arg_wsk) {
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  printf("\twatek potomny: uniemozliwione zabicie\n");

  czasozajmowacz();

  printf("\twatek potomny: umozliwienie zabicia\n");
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

  pthread_testcancel();

  zmienna_wspolna++;
  printf("\twatek potomny: zmiana wartosci zmiennej wspolnej\n");

  return (NULL);
}

int main() {
  pthread_t tid;
  pthread_attr_t attr;
  void *wynik;
  int i;

  // Wątek przyłączalny

  printf("main thread: tworzenie child thread 1\n");

  /* Tu wstaw kod tworzenia wątku z domyślnymi własnościami */
  pthread_create(&tid, NULL, zadanie_watku, NULL);

  sleep(2); // czas na uruchomienie watku

  printf("\tmain thread: wyslanie sygnalu zabicia watku\n");
  pthread_cancel(tid);

  // Co nalezy zrobić przed sprawdzeniem czy wątki się skonczyły?
  pthread_join(tid, &wynik);

  if (wynik == PTHREAD_CANCELED)
    printf("\tmain thread: watek potomny zostal zabity\n");
  else
    printf("\tmain thread: watek potomny NIE zostal zabity - blad\n");

  // Odłączanie wątku

  zmienna_wspolna = 0;

  printf("main thread: tworzenie watku potomnego nr 2\n");

  /*Tu wstaw kod tworzenia wątku z domyślnymi własnościami*/
  pthread_create(&tid, NULL, zadanie_watku, NULL);

  sleep(2); // czas na uruchomienie watku

  printf("\tmain thread: odlaczenie watku potomnego\n");
  // Instrukcja odłączenia?
  pthread_detach(tid);

  printf("\tmain thread: wyslanie sygnalu zabicia watku odlaczonego\n");
  pthread_cancel(tid);

  // Czy watek został zabity? Jak to sprawdzić?
  printf("\tmain thread: sprawdzenie zabicia watku potomnego\n");

  for (int i = 0; i < 5; ++i) {
    sleep(1);
    printf("zmienna_wspolna = %d\n", zmienna_wspolna);
    if (zmienna_wspolna > 0)
      break;
  }

  if (zmienna_wspolna == 0)
    printf("\tmain thread: watek potomny prawdopodobnie zostal zabity - OK\n");
  else
    printf("\tmain thread: watek potomny prawdopodobnie NIE zostal zabity - "
           "BLAD\n");

  // Wątek odłączony

  // Inicjacja atrybutów?
  pthread_attr_init(&attr);

  // Ustawianie typu watku na odłaczony
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  printf("main thread: tworzenie odlaczonego watku potomnego nr 3\n");
  pthread_create(&tid, &attr, zadanie_watku, NULL);

  // Niszczenie atrybutów
  pthread_attr_destroy(&attr);

  printf("\tmain thread: koniec pracy, watek odlaczony pracuje dalej\n");
  pthread_exit(NULL); // co stanie sie gdy uzyjemy exit(0)?

  //exit(0) zabija wszystkie wątki potomne, pthread_exit(NULL) pozwala watkom potomnym na zakonczenie pracy
}
