#!/usr/bin/env python3
"""Guard atomic quantity input wiring without requiring the Android SDK."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
BUFFER = (JAVA / "KeyBuffer.java").read_text(encoding="utf-8")
STATE = (JAVA / "StateManager.java").read_text(encoding="utf-8")
POPUP = (JAVA / "QuantityPopup.java").read_text(encoding="utf-8")
SEQUENCE = (JAVA / "QuantityInputSequence.java").read_text(encoding="utf-8")
STORE = (ROOT / "app/src/main/cpp/angband/ui-store.c").read_text(encoding="utf-8")


class QuantityQueueSourceTest(unittest.TestCase):
    def test_raw_sequence_enqueued_under_one_lock(self):
        body = BUFFER.split("public void addRawSequence(int[] keys) {", 1)[1].split(
            "\n\tpublic void performCenterTap(", 1
        )[0]
        self.assertIn("synchronized (keybuffer)", body)
        self.assertIn("for (int key : keys)", body)
        self.assertIn("keybuffer.offer(key);", body)
        self.assertIn("wakeUp();", body)
        self.assertNotIn("keybuffer.clear()", body)
        self.assertNotIn("add(key)", body)

    def test_popup_must_not_issue_individual_confirm_keys(self):
        self.assertIn("if (sendValue())", POPUP)
        self.assertIn("state.enqueueQuantity(", POPUP)
        self.assertNotIn("getKeyRightForQuantity()", POPUP)
        self.assertNotIn("getKeyBackspace()", POPUP)
        self.assertNotIn("getKeyEnter()", POPUP)
        self.assertIn("if (locked || resolved) return;", POPUP)
        self.assertIn("id == R.id.cancelButton", POPUP)
        self.assertIn("state.addKey(state.getKeyEsc());", POPUP)

    def test_state_routes_validated_sequence_atomically(self):
        body = STATE.split("public boolean enqueueQuantity(int amount, int max) {", 1)[1].split(
            "\n\tpublic void clearKeys(", 1
        )[0]
        self.assertIn("QuantityInputSequence.forQuantity(", body)
        self.assertIn("getKeyRightForQuantity()", body)
        self.assertIn("getKeyBackspace()", body)
        self.assertIn("getKeyEnter()", body)
        self.assertIn("if (keys == null || this.keyBuffer == null) return false;", body)
        self.assertIn("this.keyBuffer.addRawSequence(keys);", body)
        self.assertNotIn("clearKeys()", body)

    def test_native_quantity_and_store_guards_unchanged(self):
        self.assertIn("static final int PREFIX_KEYS = 15;", SEQUENCE)
        self.assertIn("amount > max", SEQUENCE)
        self.assertIn("Integer.toString(amount)", SEQUENCE)
        self.assertIn("keys[at] = enterKey;", SEQUENCE)
        self.assertIn("if (amt <= 0) return false;", STORE)
        self.assertIn("angdroid_store_explicit_yes(true, response.key.code)", STORE)


if __name__ == "__main__":
    unittest.main()
