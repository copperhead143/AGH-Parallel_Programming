package g4;

import java.util.concurrent.ForkJoinPool;

public class Main {
    public static void main(String[] args) {
        int[] array = { 42, 69, 21, 37, 82, 149, 98 };

        ForkJoinPool pool = new ForkJoinPool();
        DivideTask task = new DivideTask(array);

        int[] result = pool.invoke(task);

        System.out.println("posorotwane:");
        for (int num : result) {
            System.out.print(num + " ");
        }
    }
}
