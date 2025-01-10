package org.js;

import java.util.Scanner;


class Histogram_test {

    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#kolumns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        Obraz obraz_1 = new Obraz(n, m);

        obraz_1.calculate_histogram();
        obraz_1.print_histogram();

        // Wariant 1
        /*Thread[] threads = new Thread[94];
        for (int i = 0; i < 94; i++) {
            char symbol = (char) (i + 33);  // ASCII characters 33 to 126
            threads[i] = new HistogramThread(obraz_1, symbol, i + 1);
            threads[i].start();
        }

        for (int i = 0; i < 94; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }*/

        // Wariant 2
        /*System.out.println("Set number of threads");
        int num_threads = scanner.nextInt();

        Thread[] threads = new Thread[num_threads];

        for (int i = 0; i < num_threads; i++) {
            threads[i] = new Thread(new HistogramRunnable(i, num_threads, obraz_1));
            threads[i].start();
        }

        for (int i = 0; i < num_threads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }*/
        // 4.0
        System.out.println("Set number of threads");
        int num_threads = scanner.nextInt();

        Thread[] threads = new Thread[num_threads];

        for (int i = 0; i < num_threads; i++) {
            threads[i] = new Thread(new RowHistogramRunnable(i, num_threads, obraz_1));
            threads[i].start();
        }

        for (int i = 0; i < num_threads; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        obraz_1.compare_histograms();
    }
}
