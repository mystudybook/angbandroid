package org.rephial.xyangband;

/** JDK-only guard tests; no Android SDK or game runtime. */
public final class QuantityPromptGateTest {
    private static int checks = 0;

    private static void expect(boolean value, String label) {
        checks++;
        if (!value) throw new AssertionError(label);
    }

    public static void main(String[] args) {
        QuantityPromptGate gate = new QuantityPromptGate();
        expect(!gate.isActive(0), "zero is never a session");
        expect(!gate.resolve(0), "zero cannot resolve");

        int first = gate.open();
        expect(first != 0, "valid first session");
        expect(gate.isActive(first), "first active");
        expect(!gate.isActive(first + 1), "future ID invalid");
        expect(gate.resolve(first), "first confirmation");
        expect(!gate.isActive(first), "confirmed session inactive");
        expect(!gate.resolve(first), "duplicate confirmation rejected");
        expect(gate.close() == first, "close reports first ID");

        int second = gate.open();
        expect(second != first, "new prompt gets new session ID");
        expect(!gate.isActive(first), "old popup cannot respond");
        expect(gate.isActive(second), "second active");
        expect(gate.close() == second, "native exit identifies second");
        expect(!gate.isActive(second), "native exit invalidates");
        expect(!gate.resolve(second), "old callback cannot confirm");

        int third = gate.open();
        int fourth = gate.open();
        expect(third != fourth, "new prompt supersedes old");
        expect(!gate.resolve(third), "superseded prompt cannot cancel");
        expect(gate.resolve(fourth), "current prompt can cancel");

        for (int i = 0; i < 10000; i++) {
            int previous = gate.close();
            int next = gate.open();
            expect(next != 0 && next != previous, "monotonic IDs " + i);
            expect(gate.isActive(next), "active " + i);
        }
        System.out.println("PASS: quantity prompt session gate (" + checks + " assertions)");
    }
}
