#!/usr/bin/env python3
"""Verify all 38 FAangband divergent artifact stories against original data."""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
ANG = (ROOT / "variant_lib/angband/gamedata/artifact.txt").read_text(encoding="utf-8")
FA = (ROOT / "variant_lib/faangband/gamedata/artifact.txt").read_text(encoding="utf-8")
HEADER = (CPP / "zh-tw-faangband-artifact-variants.h").read_text(encoding="utf-8")
STANDARD = (CPP / "zh-tw-artifact-lore.h").read_text(encoding="utf-8")
INFO = (CPP / "obj-info.c").read_text(encoding="utf-8")
INIT = (CPP / "obj-init.c").read_text(encoding="utf-8")
SAVE = (CPP / "save-charoutput.c").read_text(encoding="utf-8")
NATIVE = (ROOT / "scripts/test-native-utf8.sh").read_text(encoding="utf-8")
VERIFY = (ROOT / "scripts/verify-zh-tw.sh").read_text(encoding="utf-8")

# obj-init.c appends parser_getstr(p, "text") without adding separators.
# Keep every original leading, internal, and trailing space unchanged.
def artifacts(source):
    result = {}
    for section in re.split(r"^name:", source, flags=re.M)[1:]:
        lines = section.splitlines()
        name = lines[0].strip()
        assert name not in result
        result[name] = "".join(line[5:] for line in lines
                               if line.startswith("desc:"))
    return result

ang = artifacts(ANG)
fa = artifacts(FA)
divergent = {name for name in ang.keys() & fa.keys()
             if ang[name] != fa[name]}
assert len(ang) == 138 and len(fa) == 126
assert len(divergent) == 38
assert len(ang.keys() & fa.keys()) == 85
assert "ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_VARIANTS 38" in HEADER

# Parse complete C string constants (JSON-escaped literals are C99-compatible).
literal = r'"(?:\\.|[^"\\])*"'
pattern = re.compile(
    r"\{\s*(" + literal + r"),\s*(" + literal +
    r"),\s*(" + literal + r"),\s*(" + literal + r")\s*\}"
)
rows = [tuple(json.loads(text) for text in match.groups())
        for match in pattern.finditer(HEADER)]
assert len(rows) == 38, len(rows)
assert {row[0] for row in rows} == divergent
assert len({row[0] for row in rows}) == 38
assert all(fa[name] == fa_text and ang[name] == ang_text
           for name, fa_text, ang_text, _ in rows)
assert all(zh and zh != fa_text and any(ord(c) > 127 for c in zh)
           for _, fa_text, _, zh in rows)
assert len(re.findall(r'^\s*\{\s*"[^"]+",\s*"[^"]+"\s*\},$', STANDARD,
                      re.M)) == 138

assert 'a->text = string_append(a->text, parser_getstr(p, "text"));' in INIT
assert 'strcmp(english_description, variants[i].fa_source) == 0' in HEADER
assert 'strcmp(english_description, variants[i].ang_source) == 0' in HEADER
assert 'if (allow_standard) *allow_standard = false;' in HEADER
assert 'if (allow_standard) *allow_standard = true;' in HEADER
assert 'if (!name || !english_description) return NULL;' in HEADER

assert '#include "zh-tw-faangband-artifact-variants.h"' in INFO
assert INFO.count("angdroid_zh_tw_faangband_variant_artifact_lore(") == 1
start = INFO.index("if (!OPT(player, birth_randarts) && obj->artifact &&")
end = INFO.index('textblock_append(tb, "%s\\n\\n", flavor);', start)
section = INFO[start:end]
assert "obj->known->artifact && obj->artifact->text" in section
assert "const char *flavor = obj->artifact->text;" in section
assert "if (zh_tw) {" in section
assert "bool allow_standard = true;" in section
assert "obj->artifact->name, flavor, &allow_standard" in section
assert section.index("angdroid_zh_tw_faangband_variant_artifact_lore(") < section.index(
    "angdroid_zh_tw_artifact_flavor(")
assert section.count("if (!translated && allow_standard)") == 2
assert "if (translated) flavor = translated;" in section
assert "angdroid_zh_tw_enabled() && !(mode & OINFO_SPOIL)" in INFO
assert "zh-tw-faangband-artifact-variants.h" not in SAVE
assert "tests/native/test_faangband_shared_artifact_lore.c" in NATIVE
assert "tests/test_faangband_shared_artifact_lore_source.py" in VERIFY
print("PASS: 38 FA/Ang story pairs, exact source and safe English fallback")
