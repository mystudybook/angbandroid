package org.rephial.xyangband;

import java.util.concurrent.CountDownLatch;

/** Portable host-JDK checks for the cross-thread input lifecycle epoch. */
public final class InputLifecycleEpochTest {
    private static int checks = 0;

    private static void check(boolean condition, String description) {
        checks++;
        if (!condition) throw new AssertionError(description);
    }

    public static void main(String[] args) throws Exception {
        InputLifecycleEpoch epoch = new InputLifecycleEpoch();
        check(epoch.current() == 1, "initial epoch");
        check(epoch.invalidate() == 2, "focus loss advances");
        check(epoch.current() == 2, "native thread can read current");
        check(epoch.invalidate() == 3, "pause advances");
        check(epoch.invalidate() == 4, "resume advances");
        check(epoch.current() == 4, "latest epoch is visible");

        /* Multiple producers cannot lose an invalidation under concurrent
         * UI lifecycle callbacks and JNI readers. */
        final int n = 4;
        final int perThread = 1000;
        final CountDownLatch ready = new CountDownLatch(n);
        final CountDownLatch begin = new CountDownLatch(1);
        Thread[] threads = new Thread[n];
        for (int i = 0; i < n; i++) {
            threads[i] = new Thread(() -> {
                ready.countDown();
                try {
                    begin.await();
                    for (int j = 0; j < perThread; j++) epoch.invalidate();
                } catch (InterruptedException e) {
                    Thread.currentThread().interrupt();
                    throw new AssertionError(e);
                }
            });
            threads[i].start();
        }
        ready.await();
        begin.countDown();
        for (Thread thread : threads) thread.join();
        check(epoch.current() == 4 + n * perThread,
            "concurrent generations are not lost");
        check(epoch.invalidate() == 5 + n * perThread,
            "next transition remains monotonic");

        System.out.println("PASS: input lifecycle epoch (" + checks +
            " checks)");
    }
}
