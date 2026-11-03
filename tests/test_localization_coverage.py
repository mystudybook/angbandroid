#!/usr/bin/env python3
"""Regression test of the repository-wide zh-TW localization inventory."""
import json
import subprocess
import sys
from pathlib import Path

root = Path(__file__).resolve().parents[1]
out = subprocess.check_output(
    [sys.executable, str(root / "scripts/audit-zh-tw-coverage.py"), "--json"],
    text=True,
)
report = json.loads(out)
a = report["angband"]
for key in ("identified_item_kind_names", "nonunique_monster_names"):
    section = a[key]
    assert section["translated"] == section["source_total"], key
    assert section["untranslated"] == [], key
    assert section["invalid_keys"] == [], key
assert a["monster_flavor_descriptions"]["translated"] > 0
assert len(report["variants"]["names"]) >= 11
assert report["status"] == "incomplete"
print("PASS: coverage inventory and source-complete name catalogs")
