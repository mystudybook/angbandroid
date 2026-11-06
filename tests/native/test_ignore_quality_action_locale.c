/* Standalone C99 quality-ignore captions and screen-export zh-TW strings.
 * Never changes game ignore indices, H/F key bindings or dump file formats.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ignore-quality-action.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

static int checked = 0;
static void check_quality(const char *quality, const char *type,
                          const char *expected_zh)
{
    char label[160], english[160];
    assert(angdroid_ignore_quality_action_text(
        label, sizeof(label), quality, type, true));
    assert(strcmp(label, expected_zh) == 0);
    assert(angdroid_utf8_display_columns(label) > 0);
    assert(angdroid_ignore_quality_action_text(
        label, sizeof(label), quality, type, false));
    assert(snprintf(english, sizeof(english), "All %s %s",
                    quality, type) > 0);
    assert(strcmp(label, english) == 0);
    checked += 4;
}

static void check_screen(const char *en, const char *zh)
{
    assert(strcmp(angdroid_game_ui_text(en, true), zh) == 0);
    assert(strcmp(angdroid_game_ui_text(en, false), en) == 0);
    checked += 2;
}

int main(void)
{
    char out[160];
    char tiny[4] = "old";
    const char *unknown = "All custom flavor";

    check_quality("bad", "Rings", "忽略所有劣質戒指");
    check_quality("average", "Sharp Melee Weapons",
                  "忽略所有普通銳刃近戰武器");
    check_quality("good", "Bows", "忽略所有優良弓");
    check_quality("non-artifact", "Power Dragon Scale Mail",
                  "忽略所有非神器力量龍鱗甲");
    check_quality("good", "Lights", "忽略所有優良光源");

    /* Unknown mods must fall back to a complete English label. */
    assert(angdroid_ignore_quality_action_text(out, sizeof(out),
                                               "unknown quality", "Rings", true));
    assert(strcmp(out, "All unknown quality Rings") == 0);
    assert(angdroid_ignore_quality_action_text(out, sizeof(out),
                                               "good", "unknown type", true));
    assert(strcmp(out, "All good unknown type") == 0);
    assert(!angdroid_ignore_quality_action_text(tiny, sizeof(tiny),
                                                "bad", "Rings", true));
    assert(tiny[0] == '\0');
    assert(!angdroid_ignore_quality_action_text(tiny, sizeof(tiny),
                                                NULL, "Rings", true));
    assert(tiny[0] == '\0');
    assert(!angdroid_ignore_quality_action_text(NULL, sizeof(out),
                                                "bad", "Rings", true));

    check_screen("Dump as (H)TML or (F)orum text? ",
                 "匯出格式：H HTML／F 論壇文字？ ");
    check_screen("Include monster list? ", "要包含怪物清單嗎？ ");
    check_screen("Screen dump failed.", "畫面匯出失敗。");
    assert(angdroid_game_ui_text(unknown, true) == unknown);

    printf("PASS: %d bilingual ignore-quality and screen-export checks\n",
           checked);
    return 0;
}
