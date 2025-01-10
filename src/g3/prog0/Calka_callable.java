package g3.prog0;



 import java.util.concurrent.Callable;

//public class Calka_callable implements Callable<Double>{
public class Calka_callable {
	private double dx; //wysokosc trapeza
	private double xp; //
	private double xk;
	private int N;

	public Calka_callable(double xp, double xk, double dx) {
		this.xp = xp;
		this.xk = xk;
		this.N = (int) Math.ceil((xk - xp) / dx);
		this.dx = (xk - xp) / N;
		System.out.println("tworzenie instancji Calka_callable");
		System.out.println("xp = " + xp + ", xk = " + xk + ", N = " + N);
		System.out.println("dx wymagane = " + dx + ", dx final = " + this.dx);
	}

	private double Func(double x) {
		return Math.sin(x);
	}

	public double compute_integral() {
		double calka = 0;
		int i;
		for (i = 0; i < N; i++) {
			double x1 = xp + i * dx;
			double x2 = x1 + dx;
			calka += ((Func(x1) + Func(x2)) / 2.) * dx;
		}
		System.out.println("calka czastkowa: " + calka);
		return calka;
	}

}
