#!/usr/bin/env python3
"""Check quantity-tagged FIFO isolation and FAangband native completion events."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT / "app/src/main/java/org/rephial/xyangband"
BUFFER = (JAVA / "KeyBuffer.java").read_text(encoding="utf-8")
QUEUE = (JAVA / "QuantityKeyQueue.java").read_text(encoding="utf-8")
STATE = (JAVA / "StateManager.java").read_text(encoding="utf-8")
GATE = (JAVA / "QuantityPromptGate.java").read_text(encoding="utf-8")
FANATIVE = (ROOT / "app/src/main/cpp/faangband/ui-input.c").read_text(encoding="utf-8")
ANGNATIVE = (ROOT / "app/src/main/cpp/angband/ui-input.c").read_text(encoding="utf-8")


class QuantityTaggedQueueSourceTest(unittest.TestCase):
    def test_tagged_and_ordinary_key_contract(self):
        self.assertIn("private final Queue<Entry> entries = new ArrayDeque<>();", QUEUE)
        self.assertIn("entries.offer(new Entry(key, 0));", QUEUE)
        self.assertIn("entries.offer(new Entry(key, token));", QUEUE)
        self.assertIn("token == 0 || token != quantityToken", QUEUE)
        self.assertIn("item.token != 0 && item.token != token", QUEUE)
        self.assertIn("it.remove();", QUEUE)
        self.assertIn("item.token == 0 || item.token == quantityToken", QUEUE)
        self.assertIn("return item.key;", QUEUE)

    def test_keybuffer_shared_lock_and_interruptible_wait(self):
        self.assertIn("private final QuantityKeyQueue keybuffer = new QuantityKeyQueue();", BUFFER)
        self.assertIn("public boolean addRawSequence(int[] keys, int token)", BUFFER)
        body = BUFFER.split("public boolean addRawSequence(int[] keys, int token)", 1)[1].split(
            "\n\tpublic void performCenterTap()", 1
        )[0]
        self.assertIn("synchronized (keybuffer)", body)
        self.assertIn("keybuffer.offerSequence(keys, token)", body)
        self.assertIn("keybuffer.setQuantityToken(token);", body)
        self.assertNotIn("keybuffer.clear();", body)
        get = BUFFER.split("public int get(int v)", 1)[1].split(
            "\n\tpublic void signalSave()", 1
        )[0]
        self.assertIn("Integer next = keybuffer.poll();", get)
        self.assertIn("for (;;)", get)
        self.assertIn("keybuffer.wait();", get)
        self.assertIn("finally", get)
        self.assertIn("getSpecialKey()", get)
        self.assertNotIn("keybuffer.peek()", get)
        self.assertIn("keybuffer.clear();", BUFFER)  # save() still retains override

    def test_ui_resolve_keeps_submitted_keys_until_native_end(self):
        submit = STATE.split("public boolean enqueueQuantity(int token, int amount, int max)", 1)[1].split(
            "\n\tpublic void clearKeys()", 1
        )[0]
        self.assertIn("this.keyBuffer.addRawSequence(keys, token)", submit)
        self.assertIn("quantityPromptGate.resolve(token);", submit)
        self.assertIn("this.keyBuffer.addRawSequence(new int[] { key }, token)", submit)
        self.assertNotIn("this.keyBuffer.clear();", submit)
        control = STATE.split("public void controlMsg(int type, String body)", 1)[1].split(
            "\n\tpublic void addKey(int k)", 1
        )[0]
        self.assertIn("quantityPromptGate.open()", control)
        self.assertIn("quantityPromptGate.close()", control)
        self.assertIn("this.keyBuffer.setQuantityToken(ended ? 0 : quantityToken);", control)
        self.assertLess(control.index("quantityPromptGate.open()"),
                        control.index("this.keyBuffer.setQuantityToken("))
        self.assertIn("if (!isActive(token)) return false;", GATE)

    def test_game_exit_and_reset_expires_tagged_responses(self):
        self.assertGreaterEqual(STATE.count("this.keyBuffer.setQuantityToken(0);"), 2)
        self.assertIn("this.keyBuffer.signalGameExit();", STATE)
        self.assertIn("this.keyBuffer = new KeyBuffer(this);", STATE)

    def test_faangband_native_end_on_success_and_cancel(self):
        body = FANATIVE.split("static int textui_get_quantity(", 1)[1].split(
            "\n\n/**\n * Verify something with the user", 1
        )[0]
        self.assertIn("Term_control_quantity(prompt, max, amt);", body)
        self.assertIn("if (!get_string(prompt, buf, 7)) {", body)
        self.assertEqual(2, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))
        self.assertLess(body.index("if (!get_string"), body.index("amt = atoi(buf);"))
        self.assertIn("if ((buf[0] == '*') || isalpha((unsigned char)buf[0])) amt = max;", body)
        self.assertIn("return (amt);", body)
        ang = ANGNATIVE.split("static int textui_get_quantity(", 1)[1].split(
            "\n/**\n * Verify something with the user", 1
        )[0]
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', ang)


if __name__ == "__main__":
    unittest.main()
