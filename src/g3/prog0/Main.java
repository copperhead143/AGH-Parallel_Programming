package g3.prog0;

public class Main {
    public static void main(String[] args) {
        Calka_callable calka_callable = new Calka_callable(0, Math.PI, 0.01);

        System.out.println("\nWersja sekwencyjna");
        calka_callable.compute_integral();
    }
}
