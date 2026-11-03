#!/usr/bin/env python3
"""Offline contracts for duplicate-symbol fixes in Android native variants.

Checks source declarations, NOT C compilation or the Android linker.
The real verification remains :app:assembleDebug with NDK r28c.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CPP = ROOT / "app" / "src" / "main" / "cpp"
checks = 0


def require(path, expected):
    global checks
    content = path.read_text(encoding="utf-8")
    if expected not in content:
        raise AssertionError(f"{path.relative_to(ROOT)}: missing {expected!r}")
    checks += 1


def forbid(path, unexpected):
    global checks
    content = path.read_text(encoding="utf-8")
    if unexpected in content:
        raise AssertionError(f"{path.relative_to(ROOT)}: unexpected {unexpected!r}")
    checks += 1


for variant in ("npp054", "npp710"):
    source = CPP / variant
    variable = source / "variable.c"
    utility = source / "z-util.c"
    fileio = source / "z-file.c"
    hooks = (
        "bool (*ang_sort_comp)(const void *u, const void *v, int a, int b);",
        "void (*ang_sort_swap)(void *u, void *v, int a, int b);",
    )
    for declaration in hooks:
        require(variable, declaration)
        require(utility, "extern " + declaration)
        forbid(utility, "\n" + declaration)
    for declaration in ("int player_uid;", "int player_egid;"):
        require(variable, declaration)
        require(fileio, "extern " + declaration)
        forbid(fileio, "\n" + declaration)

    for filename, length in (("cmd0.c", 22), ("obj-ui.c", 30)):
        menu = source / filename
        require(menu, f"static char comm[{length}];")
        require(menu, f"static cptr comm_descr[{length}];")
        require(menu, "static int poss;")
    require(source / "obj-ui.c", "static cmd_code comm_command[30];")

require(CPP / "unangband066" / "externs.h",
        "extern const sound_name_type angband_sound_name[MSG_MAX];")
require(CPP / "unangband066" / "variable.c",
        "const sound_name_type angband_sound_name[MSG_MAX] =")
require(CPP / "npp710" / "generate.c",
        "int large_room = ((dun->cent_n < 1) ? 11 : 10);")

print(f"PASS: {checks} native shared-library symbol ownership source checks")
