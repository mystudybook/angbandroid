#!/usr/bin/env python3
"""FAangband wizard UI strings: bilingual display, unchanged creation commands."""
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
SRC=(ROOT/"app/src/main/cpp/faangband/ui-wizard.c").read_text(encoding="utf-8")
RUN=(ROOT/"scripts/verify-zh-tw.sh").read_text(encoding="utf-8")
for label in (
    '#include "../angband/zh-tw-ui.h"',
    '#include "../angband/menu-text-fit.h"',
    "angdroid_menu_copy_columns(visible, sizeof(visible), label,",
    '"投射效果類型展示"',
    '"將圖格高度設為 1 才能查看效果。"',
    '"全部神器：%s"',
    '"全部%s"',
    '"全部神器"',
    '"全部物品"',
    '"選擇哪個%s神器？"',
    '"要建立哪種%s？"',
    '"要建立哪種神器？"',
    '"要建立哪種物品？"',
    '"確定不儲存直接離開？"',
    '"PROJ_ types display"',
    '"Change tile_height to 1 to see graphics."',
    '"All artifact %s"',
    '"All %s"',
    '"All artifacts"',
    '"All objects"',
    '"Which artifact %s? "',
    '"What kind of %s?"',
    '"What kind of artifact?"',
    '"What kind of object?"',
    '"Really quit without saving? "',
):
    assert label in SRC,label
sub=SRC.split("static bool wiz_create_item_subaction(",1)[1].split(
    "static menu_iter wiz_create_item_submenu",1)[0]
for cmd in ("CMD_WIZ_CREATE_ALL_OBJ_FROM_TVAL",
            "CMD_WIZ_CREATE_ALL_ARTIFACT_FROM_TVAL",
            "CMD_WIZ_CREATE_OBJ", "CMD_WIZ_CREATE_ARTIFACT"):
    assert cmd in sub,cmd
assert 'cmd_set_arg_number(cmdq_peek(), "tval", choices[oid + 1]);' in sub
assert 'cmd_set_arg_number(cmdq_peek(), "index", choices[oid]);' in sub
assert "menu_set_filter(menu, tvals, n);" in SRC
assert "cmdq_push(CMD_WIZ_QUIT_NO_SAVE);" in SRC
assert SRC.count("fa_wiz_put_clipped(curs_attrs[CURS_KNOWN][0 != cursor],")==2
assert "test_faangband_wizard_menu_locale_source.py" in RUN
print("PASS: wizard UI translated; commands and selection rules unchanged")
