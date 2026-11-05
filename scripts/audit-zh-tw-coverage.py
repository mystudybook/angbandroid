#!/usr/bin/env python3
"""Inventory actual native zh-TW display catalog coverage, not translation quality.

Counts are grounded in the repository's canonical game data. Full translation
requires many independent C/C++ game variants, descriptions and UI templates.
Use --json for a machine-readable report without claiming full completeness.
"""
from __future__ import annotations

import argparse
import json
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app/src/main/cpp/angband"
GAMEDATA = ROOT / "variant_lib/angband/gamedata"
PAIR = re.compile(r'\{\s*("(?:\\.|[^"\\])*"),\s*("(?:\\.|[^"\\])*")\s*\}')


def entries(path: Path) -> dict[str, str]:
    content = path.read_text(encoding="utf-8")
    pairs = [(json.loads(a), json.loads(b)) for a, b in PAIR.findall(content)]
    if len({a for a, _ in pairs}) != len(pairs):
        raise ValueError("Duplicate source label in " + str(path))
    return dict(pairs)


def coverage(source: set[str], translations: dict[str, str]) -> dict:
    return {
        "translated": len(translations),
        "source_total": len(source),
        "untranslated": sorted(source - translations.keys()),
        "invalid_keys": sorted(translations.keys() - source),
    }


def names(filename: str) -> set[str]:
    return {
        line.partition(":")[2]
        for line in (GAMEDATA / filename).read_text(encoding="utf-8").splitlines()
        if line.startswith("name:")
    }


def monsters() -> tuple[set[str], set[str]]:
    records = []
    name = None
    unique = False
    for line in (GAMEDATA / "monster.txt").read_text(encoding="utf-8").splitlines():
        if line.startswith("name:"):
            if name is not None:
                records.append((name, unique))
            name = line.partition(":")[2]
            unique = False
        elif name is not None and line.startswith("flags:"):
            if re.search(r"\bUNIQUE\b", line):
                unique = True
    if name is not None:
        records.append((name, unique))
    regular = {name for name, unique in records if not unique and not name.startswith("<")}
    uniques = {name for name, unique in records if unique}
    return regular, uniques


def inspect() -> dict:
    item_names = {name for name in names("object.txt") if not name.startswith("<")}
    monster_names, unique_names = monsters()
    item_translations = entries(CPP / "zh-tw-items.h")
    ego_translations = entries(CPP / "zh-tw-ego-names.h")
    artifact_translations = entries(CPP / "zh-tw-artifact-names.h")
    flavor_translations = entries(CPP / "zh-tw-flavor-names.h")
    monster_translations = entries(CPP / "zh-tw-monsters.h")
    unique_monster_translations = entries(CPP / "zh-tw-unique-monsters.h")
    lore_translations = entries(CPP / "zh-tw-lore.h")
    ui_translations = entries(CPP / "zh-tw-ui.h")
    combat_content = (CPP / "zh-tw-combat.h").read_text(encoding="utf-8")
    combat_labels = len(PAIR.findall(combat_content))
    magic_terms = entries(CPP / "zh-tw-magic.h")
    activations = entries(CPP / "zh-tw-activations.h")
    abilities = entries(CPP / "zh-tw-item-abilities.h")
    curse_header = (CPP / "zh-tw-curse.h").read_text(encoding="utf-8")
    curse_section = curse_header.split(
        "static inline const char *angdroid_curse_description", 1
    )[1].split("static inline void angdroid_curse_row", 1)[0]
    curse_descriptions = dict(
        (json.loads(en), json.loads(zh))
        for en, zh in PAIR.findall(curse_section)
    )
    activation_names: set[str] = set()
    current_name = ""
    for line in (GAMEDATA / "activation.txt").read_text(encoding="utf-8").splitlines():
        if line.startswith("name:"):
            current_name = line[5:]
        elif line.startswith("desc:") and current_name:
            activation_names.add(current_name)
    ability_descriptions = {
        line[5:] for line in
        (GAMEDATA / "object_property.txt").read_text(encoding="utf-8").splitlines()
        if line.startswith("desc:")
    }
    curse_source = {
        line[5:] for line in
        (GAMEDATA / "curse.txt").read_text(encoding="utf-8").splitlines()
        if line.startswith("desc:")
    }
    origins = {
        name for name, count in re.findall(
            r"ORIGIN\(\s*(\w+),\s*(-?\d+),",
            (CPP / "list-origins.h").read_text(encoding="utf-8")
        ) if int(count) >= 0
    }
    origin_ui = (CPP / "obj-info.c").read_text(encoding="utf-8")
    origin_cases = {name: "case ORIGIN_" + name + ":" in origin_ui
                    for name in origins}
    variant_root = ROOT / "variant_lib"
    variants = sorted(p.name for p in variant_root.iterdir() if p.is_dir())
    native = ROOT / "app/src/main/cpp"

    return {
        "status": "incomplete",
        "scope": "native display-only zh-TW (not full gameplay translation)",
        "angband": {
            "identified_item_kind_names": {
                "translated": len(item_translations),
                "source_total": len(item_names),
                "untranslated": sorted(item_names - item_translations.keys()),
                "invalid_keys": sorted(item_translations.keys() - item_names),
            },
            "nonunique_monster_names": {
                "translated": len(monster_translations),
                "source_total": len(monster_names),
                "untranslated": sorted(monster_names - monster_translations.keys()),
                "invalid_keys": sorted(monster_translations.keys() - monster_names),
            },
            "unique_monster_display_names": coverage(
                unique_names, unique_monster_translations
            ),
            "ego_item_suffix_names": coverage(
                names("ego_item.txt"), ego_translations
            ),
            "artifact_display_names": coverage(
                names("artifact.txt"), artifact_translations
            ),
            "unidentified_flavor_labels": coverage(
                {
                    p[-1] for line in (GAMEDATA / "flavor.txt").read_text(
                        encoding="utf-8"
                    ).splitlines()
                    if line.startswith(("flavor:", "fixed:"))
                    for p in [line.split(":")]
                    if p[-1]
                },
                flavor_translations,
            ),
            "monster_flavor_descriptions": {
                "translated": len(lore_translations),
                "source_total": len(monster_names | unique_names),
                "untranslated": sorted((monster_names | unique_names) - lore_translations.keys()),
                "invalid_keys": sorted(lore_translations.keys() - (monster_names | unique_names)),
            },
            "activation_effect_descriptions": coverage(
                activation_names, activations
            ),
            "object_property_descriptions": coverage(
                ability_descriptions, abilities
            ),
            "curse_descriptions": coverage(
                curse_source, curse_descriptions
            ),
            "item_origin_labels": {
                "translated": sum(origin_cases.values()),
                "source_total": len(origins),
                "untranslated": sorted(
                    name for name, present in origin_cases.items() if not present
                ),
            },
            "ui_literal_mappings": len(ui_translations),
            "combat_verb_and_quality_mappings": combat_labels,
            "spell_realm_display_terms": len(magic_terms),
            "unique_monster_canonical_names_unchanged": len(unique_names),
        },
        "variants": {
            "count": len(variants),
            "names": variants,
            "native_source_files": {
                variant: sum(
                    1 for p in (native / variant).rglob("*")
                    if p.is_file() and p.suffix.lower() in {".c", ".cc", ".cpp"}
                ) for variant in variants
            },
            "independent_native_localization_audit_pending": [
                v for v in variants if v != "angband"
            ],
        },
        "limitations": [
            "Item names are display-only for identified kinds, not object_desc grammar.",
            "Monster names are display-only; all unique source identities remain canonical.",
            "Monster flavor descriptions are mapped; other lore sections and dynamic event text may still be English.",
            "Activation effects are translated only when the item's effect is known.",
            "Item names in full inventory descriptions and export text are not uniformly localized.",
            "The Android device/emulator was not tested by this report.",
            "Other game variants need independent source and resource translation.",
        ],
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--json", action="store_true", help="Print full JSON including untranslated names")
    parser.add_argument("--sample", type=int, default=12, help="Untranslated lore examples")
    args = parser.parse_args()
    report = inspect()
    if args.json:
        print(json.dumps(report, ensure_ascii=False, indent=2))
    else:
        angband = report["angband"]
        for label, title in (
            ("identified_item_kind_names", "Identified item kind names"),
            ("nonunique_monster_names", "Non-unique monster names"),
            ("unique_monster_display_names", "Unique monster display names"),
            ("ego_item_suffix_names", "Ego item suffix names"),
            ("artifact_display_names", "Artifact display names"),
            ("unidentified_flavor_labels", "Unidentified flavor labels"),
            ("monster_flavor_descriptions", "Monster flavor descriptions"),
            ("activation_effect_descriptions", "Activation effect descriptions"),
            ("object_property_descriptions", "Object property descriptions"),
            ("curse_descriptions", "Curse descriptions"),
            ("item_origin_labels", "Item origin display cases"),
        ):
            section = angband[label]
            print(f"{title}: {section['translated']}/{section['source_total']}")
            if section.get("invalid_keys"):
                print("  INVALID keys:", ", ".join(section["invalid_keys"][:args.sample]))
            if section["untranslated"]:
                print("  Untranslated examples:", ", ".join(section["untranslated"][:max(0, args.sample)]))
        print(f"Core UI labels: {angband['ui_literal_mappings']}; combat: "
              f"{angband['combat_verb_and_quality_mappings']}; magic terms: "
              f"{angband['spell_realm_display_terms']}")
        other = report["variants"]["independent_native_localization_audit_pending"]
        print(f"Variants: {report['variants']['count']} "
              f"(independent native localization pending: {len(other)})")
        print("Status: INCOMPLETE — full lore, gameplay messages and other variants remain.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
