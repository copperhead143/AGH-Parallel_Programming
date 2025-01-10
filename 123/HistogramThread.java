package org.js;

class HistogramThread extends Thread {
    private final Obraz obraz;
    private final char symbol;
    private final int threadId;

    public HistogramThread(Obraz obraz, char symbol, int threadId) {
        this.obraz = obraz;
        this.symbol = symbol;
        this.threadId = threadId;
    }

    @Override
    public void run() {
        obraz.calculateHistogramVariant1(symbol);
        obraz.printSymbolHistogramVariant1(symbol, threadId);
    }
}
