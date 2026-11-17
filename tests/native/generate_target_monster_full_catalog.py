#!/usr/bin/env python3
"""Generate host C99 regression tests from the complete production name tables.

Usage (from the repository root):
  python3 tests/native/generate_target_monster_full_catalog.py build/monster-full.c
  cc -std=c99 -Wall -Wextra -Werror -pedantic -I . build/monster-full.c -o build/monster-full
  build/monster-full

Extracts all canonical names directly from the production header tables so
we don't maintain a second, diverging list of 715 monster translations.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ANGBAND = ROOT / "app/src/main/cpp/angband"
PAIR = re.compile(r'\{\s*"([^"\\]*)"\s*,\s*"([^"\\]*)"\s*\},?')


def get_pairs(filename: str, pattern: str) -> list[tuple[str, str]]:
    text = (ANGBAND / filename).read_text(encoding="utf-8")
    m = re.search(pattern, text)
    if not m:
        raise ValueError(f"Cannot find name table in {filename}")
    until = text.index("};", m.end())
    pairs = PAIR.findall(text[m.end():until])
    return pairs


standard = get_pairs("zh-tw-monsters.h", r"names\[ANGDROID_ZH_TW_MONSTER_NAMES\]\s*=\s*\{")
unique = get_pairs("zh-tw-unique-monsters.h", r"names\[ANGDROID_ZH_TW_UNIQUE_MONSTER_NAMES\]\s*=\s*\{")
fa = get_pairs("zh-tw-faangband-monster-flavor.h", r"names\[\]\s*=\s*\{")

assert len(standard) == 522, f"Expected 522 standard names, got {len(standard)}"
assert len(unique) == 101, f"Expected 101 unique names, got {len(unique)}"
assert len(fa) == 92, f"Expected 92 FAangband names, got {len(fa)}"
assert len({a for a, _ in standard}) == 522
assert len({a for a, _ in unique}) == 101
assert len({a for a, _ in fa}) == 92
assert len(set(a for a, _ in standard) | set(a for a, _ in unique) |
           set(a for a, _ in fa)) == 715


def c_string(v: str) -> str:
    return json.dumps(v, ensure_ascii=False)


def array(name: str, pairs: list[tuple[str, str]], unique_flag: int) -> str:
    lines = [f"static const struct monster_case {name}[] = {{"]
    for en, zh in pairs:
        lines.append(f"    {{ {c_string(en)}, {c_string(zh)}, {unique_flag} }},")
    lines.append("};")
    return "\n".join(lines)


src = r'''/* GENERATED from production name dictionaries. DO NOT edit directly. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-target-monster.h"
struct monster_case { const char *en, *zh; int unique; };
'''
src += '\n' + array("regular", standard, 0)
src += '\n' + array("unique", unique, 1)
src += '\n' + array("fa", fa, 0)
src += r'''
static unsigned checks;
static unsigned errors;
#define CHECK(e, label) do { \
    if (!(e)) { if (errors < 30) fprintf(stderr,"FAIL %s (%s) at line %d\n", label, c->en, __LINE__); ++errors; } \
    ++checks; \
} while(0)
static void check_one(const struct monster_case *c)
{
    char out[1024], src[512];
    int n;
    n = angdroid_target_monster_display(out,sizeof out,c->en,c->en,c->unique,1);
    CHECK(n == 1, "full name match");
    CHECK(strcmp(out,c->zh)==0, "translation");
    snprintf(src,sizeof src,"%s (offscreen)",c->en);
    n = angdroid_target_monster_display(out,sizeof out,src,c->en,c->unique,1);
    CHECK(n==1, "offscreen match");
    snprintf(src,sizeof src,"%s（畫面外）",c->zh);
    CHECK(strcmp(out,src)==0,"offscreen translation");
    n = angdroid_target_monster_display(out,sizeof out,c->en,c->en,c->unique,0);
    CHECK(n==0,"English mode");
    CHECK(strcmp(out,c->en)==0,"English output");
    n = angdroid_target_monster_display(out,sizeof out,"it",c->en,c->unique,1);
    CHECK(n==0,"invisible privacy");
    CHECK(strcmp(out,"it")==0,"invisible pronoun output");
}
int main(void)
{
    size_t i;
    for(i=0;i<sizeof regular/sizeof regular[0];++i) check_one(&regular[i]);
    for(i=0;i<sizeof unique/sizeof unique[0];++i) check_one(&unique[i]);
    for(i=0;i<sizeof fa/sizeof fa[0];++i) {
        struct monster_case c = fa[i];
        check_one(&c);
        c.unique = 1;
        check_one(&c);
    }
    printf("%s: %u assertions, %u errors, %zu total names tested\n",
        errors ? "FAIL" : "PASS",checks,errors,
        sizeof regular/sizeof regular[0]+sizeof unique/sizeof unique[0]+sizeof fa/sizeof fa[0]);
    return errors ? 1 : 0;
}
'''

if __name__ == "__main__":
    output = Path(sys.argv[1]) if len(sys.argv) > 1 else ROOT / "build/test_target_monster_full_catalog.generated.c"
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(src, encoding="utf-8")
    print(f"Generated {output} with 522 + 101 + 92 canonical names.")
