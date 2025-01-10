package g3.prog1;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class Main {
    private static final int THREADS = 8;
    private static final int TASKS = 32;

    public static void main(String[] args) {
        ExecutorService executor = Executors.newFixedThreadPool(THREADS);

        double start = 0;
        double end = Math.PI;
        double dx = 0.01;

        double division = end / TASKS;

        List<Future<Double>> futures = new ArrayList<Future<Double>>();

        for (int i = 0; i < TASKS; ++i) {
            Calka_callable calka_callable = new Calka_callable(start + i * division,
                    start + (i + 1) * division, dx);
            Future<Double> future = executor.submit(calka_callable);
            futures.add(future);
        }

        executor.shutdown();

        double result = 0;
        for (Future<Double> future : futures) {
            try {
                result += future.get();
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        System.out.println("\n========\nWynik: " + result);
        System.out.println("wszystkie watki zakonczone");
    }
}