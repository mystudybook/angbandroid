package org.rephial.xyangband;

import java.util.ArrayDeque;
import java.util.Iterator;
import java.util.Queue;

/**
 * FIFO for normal game keys and quantity responses.
 *
 * The owner (KeyBuffer) synchronizes on this instance for every operation,
 * including waiting and notifying. Ordinary keys use token 0 and must never
 * be discarded on a quantity session change.
 */
final class QuantityKeyQueue {
    private static final class Entry {
        final int key;
        final int token;

        Entry(int key, int token) {
            this.key = key;
            this.token = token;
        }
    }

    private final Queue<Entry> entries = new ArrayDeque<>();
    private int quantityToken = 0;

    void offer(int key) {
        entries.offer(new Entry(key, 0));
    }

    /**
     * Reject synthetic keys unless their native prompt is still current.
     * The caller adds the full sequence while holding the KeyBuffer lock.
     */
    boolean offerSequence(int[] keys, int token) {
        if (keys == null || keys.length == 0 ||
                token == 0 || token != quantityToken) return false;
        for (int key : keys) {
            entries.offer(new Entry(key, token));
        }
        return true;
    }

    /**
     * A native "end", a superseding prompt, or a game restart expires only
     * quantity-tagged entries. Preserve ordinary keys, macro framing and
     * save/quit signals already present in the same FIFO.
     */
    void setQuantityToken(int token) {
        quantityToken = token;
        Iterator<Entry> it = entries.iterator();
        while (it.hasNext()) {
            Entry item = it.next();
            if (item.token != 0 && item.token != token) {
                it.remove();
            }
        }
    }

    Integer poll() {
        while (true) {
            Entry item = entries.poll();
            if (item == null) return null;
            if (item.token == 0 || item.token == quantityToken) return item.key;
        }
    }

    void clear() {
        entries.clear();
    }
}
