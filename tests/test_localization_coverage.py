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
for key in ("identified_item_kind_names", "unidentified_flavor_labels",
            "artifact_display_names",
            "ego_item_suffix_names",
            "nonunique_monster_names",
            "unique_monster_display_names"):
    section = a[key]
    assert section["translated"] == section["source_total"], key
    assert section["untranslated"] == [], key
    assert section["invalid_keys"] == [], key
assert a["unidentified_flavor_labels"]["source_total"] == 202
assert a["artifact_display_names"]["source_total"] == 138
assert a["ego_item_suffix_names"]["source_total"] == 79
# Narrative descriptions are a separate corpus from artifact/ego names.
artifact_lore = a["artifact_flavor_descriptions"]
ego_lore = a["ego_flavor_descriptions"]
assert artifact_lore["source_total"] == 138
assert artifact_lore["translated"] == 138
assert artifact_lore["untranslated"] == []
assert artifact_lore["invalid_keys"] == []
assert ego_lore["source_total"] == 5
assert ego_lore["translated"] == 5
assert ego_lore["untranslated"] == []
assert ego_lore["invalid_keys"] == []
kind_lore = a["item_kind_flavor_descriptions"]
assert kind_lore["source_total"] == 141
assert kind_lore["translated"] == 141
assert kind_lore["untranslated"] == []
assert kind_lore["invalid_keys"] == []
assert a["unique_monster_display_names"]["source_total"] == 101
assert a["unique_monster_canonical_names_unchanged"] == 101
assert a["monster_flavor_descriptions"]["translated"] == a["monster_flavor_descriptions"]["source_total"]
assert a["monster_flavor_descriptions"]["untranslated"] == []
assert a["monster_flavor_descriptions"]["invalid_keys"] == []
for key in ("activation_effect_descriptions", "object_property_descriptions",
            "curse_descriptions", "item_origin_labels"):
    section = a[key]
    assert section["translated"] == section["source_total"], key
    assert section["untranslated"] == [], key
    assert section.get("invalid_keys", []) == [], key
assert len(report["variants"]["names"]) >= 11
assert report["status"] == "incomplete"
# The independent source checker must use a regex word boundary for UNIQUE.
# Two backslashes in a Python raw regex would match the literal text "\\b".
checker = (root / "scripts/check_native_game_localization.py").read_text(
    encoding="utf-8"
)
assert 're.search(r"\\bUNIQUE\\b", line)' in checker
print("PASS: coverage inventory and source-complete name catalogs")
