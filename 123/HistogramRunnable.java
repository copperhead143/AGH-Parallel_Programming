package org.js;

class HistogramRunnable implements Runnable {
    private final int threadId;
    private final int totalThreads;
    private final Obraz obraz;

    public HistogramRunnable(int threadId, int totalThreads, Obraz obraz) {
        this.threadId = threadId;
        this.totalThreads = totalThreads;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        int symbolsPerThread = 94 / totalThreads;
        int startSymbol = threadId * symbolsPerThread;
        int endSymbol = (threadId == totalThreads - 1) ? 93 : startSymbol + symbolsPerThread - 1;

        obraz.calculateHistogramVariant2(startSymbol, endSymbol);
        obraz.printSymbolHistogramVariant2(startSymbol, endSymbol, threadId + 1);
    }
}
