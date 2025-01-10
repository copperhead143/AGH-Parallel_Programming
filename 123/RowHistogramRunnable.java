package org.js;

class RowHistogramRunnable implements Runnable {
    private final Obraz obraz;
    private final int threadId;
    private final int totalThreads;
    private final int[] localHistogram;

    public RowHistogramRunnable(int threadId, int totalThreads, Obraz obraz) {
        this.obraz = obraz;
        this.threadId = threadId;
        this.totalThreads = totalThreads;
        this.localHistogram = new int[94];
    }

    @Override
    public void run() {
        int numRows = obraz.getSizeN();
        int numCols = obraz.getSizeM();
        char[][] tab = obraz.getTab();

        for (int i = threadId; i < numRows; i += totalThreads) {
            for (int j = 0; j < numCols; j++) {
                localHistogram[(int)tab[i][j]-33]++;
            }
        }

        obraz.mergeLocalHistogram(localHistogram);
    }
}
