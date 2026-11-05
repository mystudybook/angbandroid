#!/usr/bin/env python3
"""Guard the native localization audit against accidental source truncation.

This stays independent of the checker itself: even if the checker cannot
import or run, this test reports a readable failure.
"""
import ast
import json
from pathlib import Path
import re
import sys

CHECKER = Path(__file__).resolve().parents[1] / "scripts/check_native_game_localization.py"


def check_source(source):
    tree = ast.parse(source, filename=str(CHECKER))
    mains = [n for n in tree.body if isinstance(n, ast.FunctionDef)
             and n.name == "main"]
    if len(mains) != 1:
        raise AssertionError("Expected exactly one native checker main()")

    footers = [n for n in tree.body if isinstance(n, ast.If)
               and isinstance(n.test, ast.Compare)
               and isinstance(n.test.left, ast.Name)
               and n.test.left.id == "__name__"
               and any(isinstance(c, ast.Constant) and c.value == "__main__"
                       for c in n.test.comparators)]
    if len(footers) != 1 or tree.body[-1] is not footers[0]:
        raise AssertionError("Expected one final __main__ entry point")

    # A helper named table() is called early in main().  Any binding with
    # that name anywhere in main(), even in a later for-loop, makes Python
    # treat it as an uninitialized local and raises UnboundLocalError.
    if any(isinstance(n, ast.Name) and n.id == "table"
           and isinstance(n.ctx, (ast.Store, ast.Del))
           for n in ast.walk(mains[0])):
        raise AssertionError("main() shadows the table() parsing helper")

    parser = [n for n in ast.walk(mains[0]) if isinstance(n, ast.FunctionDef)
              and n.name == "terrain_table_pairs"]
    if len(parser) != 1:
        raise AssertionError("Missing unique terrain_table_pairs() parser")
    calls = [n for n in ast.walk(parser[0]) if isinstance(n, ast.Call)
             and isinstance(n.func, ast.Attribute)
             and isinstance(n.func.value, ast.Name)
             and n.func.value.id == "re" and n.func.attr == "findall"]
    if len(calls) != 1 or not calls[0].args or not isinstance(calls[0].args[0], ast.Constant):
        raise AssertionError("Missing canonical regex in terrain/trap parser")

    expression = calls[0].args[0].value
    example = '    { "fire trap", "火焰陷阱" },\n'
    pairs = re.findall(expression, example, re.MULTILINE)
    if len(pairs) != 1 or tuple(map(json.loads, pairs[0])) != ("fire trap", "火焰陷阱"):
        raise AssertionError("Terrain/trap regex cannot parse its C source pairs")

    if len([n for n in ast.walk(parser[0]) if isinstance(n, ast.ExceptHandler)
            and isinstance(n.type, ast.Name) and n.type.id == "ValueError"]) != 1:
        raise AssertionError("Terrain/trap parser has no decoding error fallback")

    for name in ("terrain_names", "terrain_lore", "trap_names", "trap_lore"):
        if not any(isinstance(n, ast.Constant) and n.value == name
                   for n in ast.walk(mains[0])):
            raise AssertionError("Missing canonical terrain/trap table: " + name)


def main():
    code = CHECKER.read_text(encoding="utf-8")
    check_source(code)
    checks = 1

    # A repeated top-level entry point must be rejected, even when parseable.
    try:
        check_source(code + '\nif __name__ == "__main__":\n    main()\n')
    except AssertionError:
        checks += 1
    else:
        raise AssertionError("Duplicate main-entry regression not detected")

    # Regression: a future loop variable named table must be rejected
    # before the native checker is invoked.
    try:
        check_source(code.replace("for table_name, expected in (",
                                  "for table, expected in (", 1))
    except AssertionError:
        checks += 1
    else:
        raise AssertionError("table() shadowing regression not detected")

    # The original corruption was an unfinished regular-expression string.
    try:
        check_source(code + "\nINVALID = 'unterminated\n")
    except SyntaxError:
        checks += 1
    else:
        raise AssertionError("Truncated expression regression not detected")

    print(f"PASS: native checker syntax and canonical table parser ({checks} checks)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
