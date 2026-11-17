"""Regression guard: top-line messages must not break UTF-8 scalars."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
source = (ROOT / "app/src/main/cpp/angband/ui-input.c").read_text(encoding="utf-8")
helper = (ROOT / "app/src/main/cpp/angband/message-wrap-utf8.h").read_text(encoding="utf-8")
start = source.index("void display_message(")
end = source.index("void bell_message(", start)
body = source[start:end]

for required in (
    '#include "message-wrap-utf8.h"',
    "angdroid_utf8_copy_complete(buf, sizeof(buf), msg)",
    "angdroid_utf8_display_columns(buf)",
    "angdroid_message_next_piece(t, (size_t)(w - 8))",
    "memcpy(line, t, piece.bytes)",
    "Term_putstr(0, 0, -1, color, line)",
    "Term_putstr(message_column, 0, -1, color, t)",
    "message_column += (int)angdroid_utf8_display_columns(t) + 1",
):
    assert required in (source if required.startswith("#include") else body), required

assert "t[--split]" not in body, "Byte-overwriting word wrap was reintroduced"
assert "while (n > w - 1)" not in body, "UTF-8 bytes used as screen width"
assert "angdroid_utf8_next_scalar(text + bytes" in helper
assert "angdroid_terminal_columns(cp)" in helper
print("PASS: top-line UTF-8 message source contract")
