#!/usr/bin/env python3
"""Source-exact 20 class-book translations, gated by screen and book tval."""
from pathlib import Path
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
HEADER = (CPP / "zh-tw-class-book-names.h").read_text(encoding="utf-8")
OBJ = (CPP / "obj-desc.c").read_text(encoding="utf-8")
KNOW = (CPP / "ui-knowledge.c").read_text(encoding="utf-8")
EXPORT = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
RUN = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

pairs = re.findall(
    r'^\s*\{ "(\[[^"]+\])", "(magic book|nature book|prayer book|shadow book)", "([^"]+)" \},$',
    HEADER, re.M)
assert len(pairs) == 20
assert len({(title, kind) for title, kind, _ in pairs}) == 20
for variant in ("angband", "faangband"):
    source = (ROOT / f"variant_lib/{variant}/gamedata/class.txt").read_text(
        encoding="utf-8")
    books = {
        (line.split(":")[3], line.split(":")[1])
        for line in source.splitlines() if line.startswith("book:")
    }
    assert len(books) == 20, variant
    assert books == {(title, kind) for title, kind, _ in pairs}, variant
assert all(title.startswith("[") and title.endswith("]") for title, _, _ in pairs)
assert all("《" in display and display.endswith("》") for _, _, display in pairs)
assert "strcmp(source_title, books[i].source) == 0" in HEADER
assert "strcmp(tval, books[i].tval) == 0" in HEADER
assert '#include "zh-tw-class-book-names.h"' in OBJ
assert '#include "zh-tw-class-book-names.h"' in KNOW
assert OBJ.count("angdroid_zh_tw_class_book_name(") == 2
assert KNOW.count("angdroid_zh_tw_class_book_name(") == 1
assert "if (zh_tw && !translated)" in OBJ
assert "if (!localized_kind)" in OBJ
assert "if ((mode & ODESC_ZH_TW) && angdroid_zh_tw_enabled() && aware" in OBJ
assert "if (!easy_know && !kind->aware && kind->flavor)" in OBJ
assert 'book_title ? book_title : kind->name' in KNOW
assert '"The %s spell, %s, from %s triggers the shapechange."' in KNOW
assert "zh-tw-class-book-names.h" not in EXPORT
assert "tests/native/test_class_book_names_locale.c" in RUN
assert "tests/test_class_book_names_source.py" in VERIFY
print("PASS: 20 class-book source names, known display, shape lore and English fallback")
