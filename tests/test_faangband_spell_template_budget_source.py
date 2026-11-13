#!/usr/bin/env python3
"""FAangband batch 253: no dropped suffix or split UTF-8 on repeated spell tags.

Derive *every* Chinese template from the production locale data. The C99
runtime guard conservatively reserves up to 320 bytes for each interpolation
(the largest spell-local subject/target display buffer).
"""
from pathlib import Path
import json
import re

ROOT = Path(__file__).resolve().parents[1]

def read(name):
    return (ROOT / name).read_text(encoding="utf-8")

VISIBLE = read("app/src/main/cpp/faangband/mon-spell-messages-visible-zh-tw.h")
OTHER = read("app/src/main/cpp/faangband/mon-spell-messages-other-zh-tw.h")
ALT = read("app/src/main/cpp/angband/zh-tw-mon-spell-alt.inc")
PROJECTION = read("variant_lib/faangband/gamedata/projection.txt")
LASH = read("app/src/main/cpp/angband/zh-tw-mon-spell-lash.h")
LOCALE = read("app/src/main/cpp/faangband/mon-spell-message-locale.h")
SPELL = read("app/src/main/cpp/faangband/mon-spell.c")
NATIVE = read("tests/native/test_faangband_spell_template_budget.c")
RUNNER = read("scripts/test-fa-spell-template-budget.sh")
VERIFY = read("scripts/verify-zh-tw.sh")

pair_re = re.compile(
    r'^\s*\{\s*("(?:\\.|[^"\\])*")\s*,\s*("(?:\\.|[^"\\])*")\s*\},',
    re.MULTILINE)

def pairs(contents):
    return [(json.loads(en), json.loads(zh))
            for en, zh in pair_re.findall(contents)]

vis = pairs(VISIBLE)
other = pairs(OTHER)
alternate = pairs(ALT)
assert (len(vis), len(other), len(alternate)) == (116, 105, 17)
all_cases = vis + other + alternate
assert len(all_cases) == 238

tag_re = re.compile(r'\{([a-z]+)\}')
all_tag_names = {"name", "pronoun", "target", "type", "oftype"}

def upper_bound(message):
    remaining_text = tag_re.sub("", message)
    return len(remaining_text.encode("utf-8")) + 320 * len(tag_re.findall(message))

worst_cases = [(upper_bound(zh), en) for en, zh in all_cases]
assert all(en and zh for en, zh in all_cases)
for en, zh in all_cases:
    assert sorted(tag_re.findall(en)) == sorted(tag_re.findall(zh))
    assert set(tag_re.findall(zh)).issubset(all_tag_names)
    assert upper_bound(zh) <= 1023, (en, upper_bound(zh))
    assert len(tag_re.findall(zh)) <= 2, en
assert max(bound for bound, _ in worst_cases) == 679

# {type}/{oftype} come from projection lash descriptors. Ensure they never
# exceed the same 320-byte substitution reservation used for monster names.
source_lash = {
    line[len("lash-desc:"):]
    for line in PROJECTION.splitlines() if line.startswith("lash-desc:")
}
translated_lash = {en: zh for en, zh in pairs(LASH)}
assert len(source_lash) == len(translated_lash) == 25
assert source_lash == set(translated_lash)
assert max(len(value.encode("utf-8")) for value in (
    list(source_lash) + list(translated_lash.values()))) < 320


# Both the worst-case expansion length and the maximum scratch-buffer
# sizes must remain within the 1024-byte original message allocation.
body = SPELL.split("static void spell_message(", 1)[1].split(
    "const struct monster_spell *monster_spell_by_index(", 1)[0]
assert 'char buf[1024] = "\\0";' in body
assert 'char m_name[256];' in body
assert 'char m_poss[256];' in body
assert 'char cn_name[320];' in body
assert 'char cn_offscreen[320];' in body
assert 'angdroid_fa_spell_template_fits(' in body
assert "if (localized && !angdroid_fa_spell_template_fits(" in body
assert body.count("char m_name[256];") == 2
assert body.count("char cn_offscreen[320];") == 2
assert 'in_cursor, sizeof(buf), 320)' in body
assert 'msgt(spell->msgt, "%s", "施法訊息過長，已省略詳細內容。");' in body
assert body.index("localized = (in_cursor != selected);") < body.index(
    "angdroid_fa_spell_template_fits(")
assert body.index("angdroid_fa_spell_template_fits(") < body.index(
    "next = strchr(in_cursor, '{');")
assert body.index("next = strchr(in_cursor, '{');") < body.index(
    "switch (spell_tag_lookup(tag))")
assert body.index("if (!seen) {") < body.index(
    "angdroid_fa_spell_template_fits(")
assert body.index("} else if (!hits) {") < body.index(
    "angdroid_fa_spell_template_fits(")
assert "msgt(spell->msgt, \"%s\", safe_buf);" in body
assert "msgt(spell->msgt, \"%s\", buf);" in body
assert "if (localized) {" in body

# Guard must operate on raw UTF-8 bytes and avoid additions that overflow
# the remaining capacity. Never multiply an untrusted tag count.
for token in [
    "static inline bool angdroid_fa_spell_template_fits(",
    "if (!message || capacity == 0 || max_expansion_bytes == 0)",
    "remaining = capacity - 1;",
    "if (*cursor == '{')",
    "strchr(cursor + 1, '}')",
    "if (max_expansion_bytes > remaining) return false;",
    "remaining -= max_expansion_bytes;",
    "if (remaining == 0) return false;",
    "--remaining;",
    "++cursor;",
    "return true;"
]:
    assert token in LOCALE, token

assert "-std=c99 -Wall -Wextra -Werror -pedantic" in RUNNER
assert "test_faangband_spell_template_budget.c" in RUNNER
assert "angdroid_fa_spell_template_fits(" in NATIVE
assert "正常輸出。" in NATIVE
assert "施法訊息過長，已省略詳細內容。" in SPELL
assert "test-fa-spell-template-budget.sh" in VERIFY
assert "test_faangband_spell_template_budget_source.py" in VERIFY
print("PASS: 238 templates, <=2 dynamic tags, max 679 of 1023 usable bytes")
