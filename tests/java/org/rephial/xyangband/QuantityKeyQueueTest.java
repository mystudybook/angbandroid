package org.rephial.xyangband;

/** Pure JDK regressions for quantity key expiry without deleting ordinary keys. */
public final class QuantityKeyQueueTest {
    private static int checks;

    private static void expect(boolean condition, String label) {
        checks++;
        if (!condition) throw new AssertionError(label);
    }

    private static void next(QuantityKeyQueue queue, int expected, String label) {
        Integer found = queue.poll();
        expect(found != null && found == expected, label);
    }

    private static void empty(QuantityKeyQueue queue, String label) {
        expect(queue.poll() == null, label);
    }

    public static void main(String[] args) {
        QuantityKeyQueue queue = new QuantityKeyQueue();
        empty(queue, "initially empty");
        expect(!queue.offerSequence(new int[] {'1'}, 7), "must open native session");
        queue.offer('m');
        queue.offer(-200); // macro framing
        queue.offer(-1);   // save signal
        queue.setQuantityToken(7);
        expect(queue.offerSequence(new int[] {'1', '2', 156}, 7), "accept current batch");
        next(queue, 'm', "normal input preserved");
        next(queue, -200, "macro marker preserved");
        next(queue, -1, "save signal preserved");
        next(queue, '1', "first digit preserved after UI resolve");
        next(queue, '2', "second digit preserved after UI resolve");
        next(queue, 156, "Enter preserved after UI resolve");
        empty(queue, "all consumed");

        expect(queue.offerSequence(new int[] {159, 156}, 7), "batch before end");
        queue.offer('x');
        queue.offer(-200);
        queue.setQuantityToken(0);
        next(queue, 'x', "normal key survives native end");
        next(queue, -200, "macro marker survives native end");
        empty(queue, "end discards only unconsumed quantity");
        expect(!queue.offerSequence(new int[] {156}, 7), "old token rejected after end");
        expect(!queue.offerSequence(new int[] {156}, 0), "untagged synthetic batch rejected");

        queue.setQuantityToken(8);
        expect(!queue.offerSequence(null, 8), "null batch rejected");
        expect(!queue.offerSequence(new int[0], 8), "empty batch rejected");
        expect(!queue.offerSequence(new int[] {156}, 7), "superseded token rejected");
        expect(queue.offerSequence(new int[] {159, '2', 156}, 8), "new batch accepted");
        queue.offer(-1);
        queue.setQuantityToken(9);
        next(queue, -1, "save marker survives superseding prompt");
        empty(queue, "superseded batch removed");
        expect(queue.offerSequence(new int[] {'4', 156}, 9), "new session works");
        next(queue, '4', "new session digit");
        next(queue, 156, "new session Enter");
        empty(queue, "new session fully consumed");

        // A native prompt may exit with keys remaining. Confirm that old
        // quantity bytes never spill into the next prompt or game screen.
        for (int i = 1; i <= 10000; i++) {
            queue.clear();
            queue.setQuantityToken(i);
            queue.offer(i);
            expect(queue.offerSequence(new int[] {159, '5', 156}, i),
                    "queue tagged response " + i);
            queue.offer(-200);
            queue.offer(-1);
            if ((i & 1) == 0) {
                // Partial consumption before end still leaves a tagged tail.
                next(queue, i, "normal prefix " + i);
                next(queue, 159, "first tagged key " + i);
                queue.setQuantityToken(0);
            } else {
                // Supersession before any tagged key is consumed.
                queue.setQuantityToken(i + 1);
                next(queue, i, "normal prefix " + i);
            }
            next(queue, -200, "macro sentinel survives " + i);
            next(queue, -1, "save signal survives " + i);
            empty(queue, "stale quantity tail removed " + i);
        }

        queue.clear();
        queue.setQuantityToken(22);
        expect(queue.offerSequence(new int[] {159, 156}, 22), "pre-save response");
        queue.clear();
        queue.offer(-1); // existing signalSave() contract
        next(queue, -1, "signalSave retains override behavior");
        empty(queue, "no post-save junk");

        System.out.println("PASS: tagged quantity key queue (" + checks + " assertions)");
    }
}
