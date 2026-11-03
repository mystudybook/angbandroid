package org.rephial.xyangband;

import java.nio.charset.StandardCharsets;
import java.util.Random;

/** Deterministic pure-Java buffer fuzzing; no Android dependencies at runtime. */
public final class TermWindowStressTest {
    private static void validate(TermWindow w, int step, String action) {
        for (int row = 0; row < w.rows; row++) {
            for (int col = 0; col < w.cols; col++) {
                TermWindow.TermPoint p = w.buffer[row][col];
                if (p == null) throw new AssertionError("Null cell at " + step);
                if (p.textColumns == 2 &&
                        (col + 1 >= w.cols || w.buffer[row][col + 1].textColumns != 0)) {
                    throw new AssertionError("Orphan wide lead, step=" + step + " action=" + action);
                }
                if (p.textColumns == 0 &&
                        (col == 0 || w.buffer[row][col - 1].textColumns != 2)) {
                    throw new AssertionError("Orphan continuation, step=" + step + " action=" + action);
                }
            }
        }
    }

    public static void main(String[] args) {
        final Random random = new Random(20261103L);
        final String[] examples = {
                "ABC", "中文", "劍龍", "A中B", "。！？", "𠀀", "X", "中", "\tA"
        };
        TermWindow window = new TermWindow(5, 16, 0, 0);
        for (int step = 0; step < 15000; step++) {
            int operation = random.nextInt(11);
            int row = random.nextInt(window.rows);
            int col = random.nextInt(window.cols);
            window.move(row, col);
            String action;
            switch (operation) {
                case 0: case 1: case 2:
                    action = "write";
                    byte[] bytes = examples[random.nextInt(examples.length)]
                            .getBytes(StandardCharsets.UTF_8);
                    window.addnstr(bytes.length, bytes, "UTF-8");
                    break;
                case 3:
                    action = "addch"; window.addch('x'); break;
                case 4:
                    action = "clearpoint"; window.clearPoint(row, col); break;
                case 5:
                    action = "toeol"; window.clrtoeol(); break;
                case 6:
                    action = "tobot"; window.clrtobot(); break;
                case 7:
                    action = "tile"; window.addTile(col, row, 0x80, 65, 0, 0); break;
                case 8:
                    action = "pad"; window.addTilePad(col, row, 2, 2); break;
                case 9:
                    action = "scroll"; window.scroll(); break;
                default:
                    action = "resize";
                    window.resize(1 + random.nextInt(16), 1 + random.nextInt(8));
                    break;
            }
            validate(window, step, action);
        }
        System.out.println("PASS: 15,000 deterministic terminal buffer operations");
    }
}
