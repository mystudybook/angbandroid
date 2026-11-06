#!/usr/bin/env python3
"""Regression checks for native quantity start/end and Android session-token wiring."""
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
APP = ROOT / "app/src/main/java/org/rephial/xyangband"
NATIVE = (ROOT / "app/src/main/cpp/angband/ui-input.c").read_text(encoding="utf-8")
STATE = (APP / "StateManager.java").read_text(encoding="utf-8")
ACTIVITY = (APP / "GameActivity.java").read_text(encoding="utf-8")
POPUP = (APP / "QuantityPopup.java").read_text(encoding="utf-8")
DIALOG = (APP / "AngbandDialog.java").read_text(encoding="utf-8")
GATE = (APP / "QuantityPromptGate.java").read_text(encoding="utf-8")


class QuantityPromptSessionSourceTest(unittest.TestCase):
    def quantity_body(self):
        return NATIVE.split("static int textui_get_quantity(", 1)[1].split(
            "\n/**\n * Verify something with the user", 1)[0]

    def test_native_end_on_valid_input_or_escape(self):
        body = self.quantity_body()
        self.assertIn("Term_control_quantity(prompt, max, amt);", body)
        self.assertIn("if (!get_string(prompt, buf, sizeof(buf))) break;", body)
        self.assertIn("result = amt;", body)
        self.assertIn('Term_control(TERM_CONTROL_QUANTITY, "end");', body)
        self.assertIn("return result;", body)
        self.assertNotIn("return 0;\n", body.split("Term_control_quantity(prompt, max, amt);", 1)[1])
        self.assertEqual(1, body.count('Term_control(TERM_CONTROL_QUANTITY, "end");'))

    def test_native_thread_stamps_events_before_async_dispatch(self):
        body = STATE.split("public void controlMsg(int type, String body)", 1)[1].split(
            "\n\tpublic void addKey(int k)", 1)[0]
        self.assertIn("GameActivity.TERM_CONTROL_QUANTITY", body)
        self.assertIn('"end".equals(body)', body)
        self.assertIn("quantityPromptGate.close()", body)
        self.assertIn("quantityPromptGate.open()", body)
        self.assertIn("msg.arg2 = quantityToken;", body)
        self.assertLess(body.index("quantityPromptGate.close()"),
                        body.index("handler.sendMessage(msg);"))
        self.assertIn("activity.controlMsg(msg.arg1, (String)msg.obj, msg.arg2);", DIALOG)

    def test_popup_actions_check_current_native_session(self):
        self.assertIn("private int sessionToken = 0;", POPUP)
        self.assertIn("sessionToken, valuePicker.getValue(), valuePicker.getMaxValue()", POPUP)
        self.assertIn("state.cancelQuantity(sessionToken);", POPUP)
        self.assertIn("public void dismissSilently()", POPUP)
        self.assertIn("resolved = true;", POPUP)
        self.assertIn("isQuantityPromptActive(quantityToken)", ACTIVITY)
        self.assertIn('"end".equals(msg)', ACTIVITY)
        self.assertIn("quantPopup.getSessionToken() == quantityToken", ACTIVITY)
        self.assertIn("quantPopup.dismissSilently();", ACTIVITY)
        self.assertIn("quantPopup = new QuantityPopup(this, message, maxValue, initialValue,", ACTIVITY)
        self.assertNotIn("quantPopup.configure(", ACTIVITY)
        self.assertIn("runQuantityPopup(matcher.group(3), maxValue, initialValue,", ACTIVITY)

    def test_quantity_lock_covers_token_and_all_enqueued_keys(self):
        body = STATE.split("public boolean enqueueQuantity(int token, int amount, int max)", 1)[1].split(
            "\n\tpublic void clearKeys()", 1
        )[0]
        self.assertIn("synchronized (quantityPromptGate)", body)
        self.assertIn("quantityPromptGate.isActive(token)", body)
        self.assertIn("this.keyBuffer.addRawSequence(keys);", body)
        self.assertIn("quantityPromptGate.resolve(token);", body)
        self.assertIn("this.keyBuffer.addRawSequence(new int[] { key });", body)
        self.assertNotIn("clearKeys();", body)
        self.assertIn("if (!isActive(token)) return false;", GATE)
        self.assertIn("active = 0;", GATE)
        self.assertIn("latest = serial;", GATE)


if __name__ == "__main__":
    unittest.main()
