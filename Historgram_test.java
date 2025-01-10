// Histogram_test.java
import java.util.Scanner;

class Histogram_test {

	public static void main(String[] args) {
		Scanner scanner = new Scanner(System.in);

		System.out.println("Set image size: n (#rows), m(#columns)");
		int n = scanner.nextInt();
		int m = scanner.nextInt();
		Obraz obraz_1 = new Obraz(n, m);

		obraz_1.calculate_histogram();
		obraz_1.print_histogram();

		System.out.println("Wybierz wariant: 1 - Wątek dla każdego znaku, 2 - Blokowy podział znaków ASCII");
		int wybor = scanner.nextInt();

		switch (wybor) {
			case 1:
				// Wariant 1: Wątek dla każdego znaku
				WatekHistogram[] watkiZnaki = new WatekHistogram[94];
				for (int i = 0; i < 94; i++) {
					char znak = (char) (i + 33);
					watkiZnaki[i] = new WatekHistogram(znak, obraz_1);
					watkiZnaki[i].start();
				}
				for (WatekHistogram watek : watkiZnaki) {
					try {
						watek.join();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				break;

			case 2:
				// Wariant 2: Blokowy podział znaków ASCII
				int liczbaWatek = 4;
				int zakres = 94 / liczbaWatek;
				Thread[] watkiZakresy = new Thread[liczbaWatek];
				for (int i = 0; i < liczbaWatek; i++) {
					int startZnaku = i * zakres;
					int endZnaku = (i == liczbaWatek - 1) ? 93 : (startZnaku + zakres - 1);
					watkiZakresy[i] = new Thread(new WatekHistogramRunnable(startZnaku, endZnaku, obraz_1));
					watkiZakresy[i].start();
				}
				for (Thread watek : watkiZakresy) {
					try {
						watek.join();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				break;

			default:
				System.out.println("Niepoprawny wybór.");
		}

		// Porównanie wyników
		obraz_1.porownajHistogramy();
	}
}
