package org.js;

import java.util.Random;
import java.util.concurrent.atomic.AtomicIntegerArray;

class Obraz {

    private int size_n;
    private int size_m;
    private char[][] tab;
    private char[] tab_symb;
    private int[] histogram;
    private AtomicIntegerArray hist_parallel;

    public Obraz(int n, int m) {

        this.size_n = n;
        this.size_m = m;
        tab = new char[n][m];
        tab_symb = new char[94];

        final Random random = new Random();

        // for general case where symbols could be not just integers
        for(int k=0;k<94;k++) {
            tab_symb[k] = (char)(k+33); // substitute symbols
        }

        for(int i=0;i<n;i++) {
            for(int j=0;j<m;j++) {
                tab[i][j] = tab_symb[random.nextInt(94)];  // ascii 33-127
                //tab[i][j] = (char)(random.nextInt(94)+33);  // ascii 33-127
                System.out.print(tab[i][j]+" ");
            }
            System.out.print("\n");
        }
        System.out.print("\n\n");

        histogram = new int[94];
        hist_parallel = new AtomicIntegerArray(94);
        clear_histogram();
    }

    public int getSizeN() {
        return size_n;
    }

    public int getSizeM() {
        return size_m;
    }

    public char[][] getTab() {
        return tab;
    }

    public void clear_histogram(){

        for(int i=0;i<94;i++) histogram[i]=0;

    }

    public void calculate_histogram(){

        for(int i=0;i<size_n;i++) {
            for(int j=0;j<size_m;j++) {

                // optymalna wersja obliczania histogramu, wykorzystujÄca fakt, Ĺźe symbole w tablicy
                // moĹźna przeksztaĹciÄ w indeksy tablicy histogramu
                // histogram[(int)tab[i][j]-33]++;

                // wersja bardziej ogĂłlna dla tablicy symboli nie utoĹźsamianych z indeksami
                // tylko dla tej wersji sensowne jest zrĂłwnoleglenie w dziedzinie zbioru znakĂłw ASCII
                for(int k=0;k<94;k++) {
                    if(tab[i][j] == tab_symb[k]) histogram[k]++;
                    //if(tab[i][j] == (char)(k+33)) histogram[k]++;
                }

            }
        }
    }

    public void calculateHistogramVariant1(char symbol) {
        int symbolIndex = symbol - 33;
        int count = 0;

        for (int i = 0; i < size_n; i++) {
            for (int j = 0; j < size_m; j++) {
                if (tab[i][j] == symbol) {
                    count++;
                }
            }
        }
        hist_parallel.set(symbolIndex, count);  // Atomic update
    }

    public synchronized void printSymbolHistogramVariant1(char symbol, int threadId) {
        int symbolIndex = symbol - 33;
        int count = hist_parallel.get(symbolIndex);
        String bar = "=".repeat(count);
        System.out.println("Thread " + threadId + ": " + symbol + " " + bar);
    }

    public void calculateHistogramVariant2(int startSymbol, int endSymbol) {
        for (int i = 0; i < size_n; i++) {
            for (int j = 0; j < size_m; j++) {
                char symbol = tab[i][j];
                if (symbol >= tab_symb[startSymbol] && symbol <= tab_symb[endSymbol]) {
                    hist_parallel.incrementAndGet(symbol - 33);
                }
            }
        }
    }

    public synchronized void printSymbolHistogramVariant2(int startSymbol, int endSymbol, int threadId) {
        for (int i = startSymbol; i <= endSymbol; i++) {
            int count = hist_parallel.get(i);
            String bar = "=".repeat(count);
            System.out.println("Thread " + threadId + ": " + tab_symb[i] + " " + bar);
        }
    }

    public void mergeLocalHistogram(int[] localHistogram) {
        for (int i = 0; i < 94; i++) {
            hist_parallel.addAndGet(i, localHistogram[i]);
        }
    }

    public void compare_histograms() {
        System.out.println("\nComparing histograms...");
        boolean match = true;
        for (int i = 0; i < 94; i++) {
            if (histogram[i] != hist_parallel.get(i)) {
                System.out.println("Mismatch at index " + i + ": Sequential = " + histogram[i] + ", Parallel = " + hist_parallel.get(i));
                match = false;
            }
        }
        if (match) {
            System.out.println("Histograms match!");
        } else {
            System.out.println("Histograms do not match.");
        }
    }

    public void print_histogram(){

        for(int i=0;i<94;i++) {
            System.out.print(tab_symb[i]+" "+histogram[i]+"\n");
            //System.out.print((char)(i+33)+" "+histogram[i]+"\n");
        }

    }
}
