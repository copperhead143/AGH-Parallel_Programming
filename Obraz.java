// Obraz.java
import java.util.Random;

class Obraz {

	private int size_n;
	private int size_m;
	private char[][] tab;
	private char[] tab_symb;
	private int[] histogram;
	private int[] hist_parallel;  // Nowa tablica dla wyników równoległych

	public Obraz(int n, int m) {

		this.size_n = n;
		this.size_m = m;
		tab = new char[n][m];
		tab_symb = new char[94];
		histogram = new int[94];
		hist_parallel = new int[94];  // Inicjalizacja tablicy równoległej

		final Random random = new Random();

		// Inicjalizacja symboli ASCII
		for(int k=0; k<94; k++) {
			tab_symb[k] = (char)(k+33); 
		}

		// Wypełnianie tablicy znaków
		for(int i=0; i<n; i++) {
			for(int j=0; j<m; j++) {
				tab[i][j] = tab_symb[random.nextInt(94)];  
				System.out.print(tab[i][j]+" ");
			}
			System.out.print("\n");
		}
		System.out.print("\n\n");

		clear_histogram();
	}

	public void clear_histogram(){
		for(int i=0; i<94; i++) {
			histogram[i] = 0;
			hist_parallel[i] = 0; // Zerowanie równoległego histogramu
		}
	}

	public void calculate_histogram(){
		for(int i=0; i<size_n; i++) {
			for(int j=0; j<size_m; j++) {
				for(int k=0; k<94; k++) {
					if(tab[i][j] == tab_symb[k]) histogram[k]++;
				}
			}
		}
	}

	// Metoda równoległego liczenia histogramu dla danego znaku
	public synchronized void liczHistogramDlaZnaku(char znak) {
		for (int i = 0; i < size_n; i++) {
			for (int j = 0; j < size_m; j++) {
				if (tab[i][j] == znak) {
					hist_parallel[znak - 33]++;
				}
			}
		}
	}

	// Metoda do graficznego wyświetlania fragmentu histogramu dla znaku
	public synchronized void drukujFragmentHistogramu(char znak) {
		int liczbaWystapien = hist_parallel[znak - 33];
		StringBuilder grafika = new StringBuilder();
		for (int i = 0; i < liczbaWystapien; i++) {
			grafika.append("=");
		}
		System.out.println("Wątek dla znaku '" + znak + "': " + grafika);
	}

	public void print_histogram(){
		for(int i=0; i<94; i++) {
			System.out.print(tab_symb[i] + " " + histogram[i] + "\n");
		}
	}

	// Metoda porównująca histogramy
	public void porownajHistogramy() {
		for (int i = 0; i < 94; i++) {
			if (histogram[i] != hist_parallel[i]) {
				System.out.println("Histogramy różnią się dla znaku: " + tab_symb[i]);
				return;
			}
		}
		System.out.println("Histogramy są identyczne.");
	}
}
