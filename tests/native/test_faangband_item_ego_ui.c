/* C99: source-complete identified FAangband ego and kind-name lookup. */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-item-zh-tw.h"
#include "../../app/src/main/cpp/angband/zh-tw-item-grammar.h"
#include "../../app/src/main/cpp/angband/obj-desc-suffix-utf8.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

int main(void)
{
    static const char *ego_names[] = {
        "of Shadows",
        "of Brightness",
        "(Everburning)",
        "of True Sight",
        "(Holy Avenger)",
        "of Doriath",
        "(Blessed)",
        "of Gondolin",
        "of the Noldor",
        "of Fury",
        "of Nogrod",
        "of the Balrog",
        "of Melting",
        "of Shocking",
        "of Burning",
        "of Freezing",
        "of Poisoning",
        "of the Hunter",
        "of Slay Evil",
        "of Dread",
        "of Slay Demon",
        "of the Drûgs",
        "of Sneaking",
        "of Slay Dragon",
        "of *Slay* Animal",
        "of *Slay* Evil",
        "of *Slay* Undead",
        "of *Slay* Demon",
        "of *Slay* Orc",
        "of *Slay* Troll",
        "of *Slay* Giant",
        "of *Slay* Dragon",
        "of Angband",
        "of Accuracy",
        "of Velocity",
        "of Ossiriand",
        "of Extra Might",
        "of Extra Shots",
        "of Hurt Animal",
        "of Hurt Evil",
        "of Hurt Undead",
        "of Hurt Demon",
        "of Hurt Dragon",
        "of Holy Might",
        "of *Hurt* Evil",
        "of Acid",
        "of Electricity",
        "of Flame",
        "of Frost",
        "of Poison",
        "of Wounding",
        "of Backbiting",
        "of Digging",
        "of Earthquakes",
        "of Slow Descent",
        "of Stealth",
        "of Free Action",
        "of Speed",
        "of Stability",
        "of Elvenkind",
        "of Kicking",
        "of Intelligence",
        "of Wisdom",
        "of the Magi",
        "of Might",
        "of Lordliness",
        "of Seeing",
        "of Infravision",
        "of Light",
        "of Telepathy",
        "of Regeneration",
        "of Serenity",
        "of Night and Day",
        "of Trollkind",
        "of the Shadow Realm",
        "of Anchoring",
        "(Wolfhide)",
        "of Ered Engrin",
        "of Desirability",
        "of Resistance",
        "of Permanence",
        "(Dwarven)",
        "of Protection",
        "of Shard Protection",
        "of Aman",
        "of Enveloping",
        "of Slaying",
        "of Agility",
        "of Power",
        "of Magic Mastery",
        "of Thievery",
        "of Combat",
        "(Reinforced)",
        "(Coated)",
        "of the Earth",
        "of Preservation",
        "of Craftsmanship",
        "of Mental Strength",
        "of Doom",
        "of Basic Resistance",
        "of Magical Item Mastery",
        "of Clarity",
        "of the Shadows",
        "of Metamorphosis",
        "of Sustenance",
        "of Trickery",
        "of Weaponmastery",
        "of Vitality",
        "of Insight",
        "of the Elements",
        "of Physical Prowess",
        "of Mobility",
        "of Arcane Resistance",
        "of Utility",
        "of Hindrance",
        "of the Dawn",
        "of Woe",
        "of Fickleness",
    };
    static const struct { const char *name, *tval, *expected; } fa_kinds[] = {
        { "& Main Gauche~", "sword", "副手短劍" },
        { "& Valinorean Sword~", "sword", "維林諾長劍" },
        { "& Magestaff~", "hafted", "法師長杖" },
        { "& Storm Hammer~", "hafted", "風暴戰鎚" },
        { "& Dart~", "polearm", "飛鏢" },
        { "& Thrusting Spear~", "polearm", "突刺長矛" },
        { "& Silver Crown~", "crown", "銀冠" },
        { "& Shadow Cloak~", "cloak", "暗影斗篷" },
        { "& Body Shield~", "shield", "全身盾" },
        { "& Shield of Deflection~", "shield", "偏斜之盾" },
        { "Steel", "ring", "鋼製戒指" },
        { "Pewter", "ring", "錫鑞戒指" },
        { "Galvorn", "ring", "加爾沃恩戒指" },
        { "Brass", "ring", "黃銅戒指" },
        { "Bronze", "ring", "青銅戒指" },
        { "Copper", "ring", "銅戒指" },
        { "Silver", "ring", "銀戒指" },
        { "Gold", "ring", "金戒指" },
        { "Adamantite", "ring", "精金戒指" },
        { "Mithril", "ring", "秘銀戒指" },
        { "Chalcedony", "amulet", "玉髓護身符" },
        { "Malachite", "amulet", "孔雀石護身符" },
        { "Opal", "amulet", "蛋白石護身符" },
        { "Garnet", "amulet", "石榴石護身符" },
        { "Pearl", "amulet", "珍珠護身符" },
        { "Amethyst", "amulet", "紫水晶護身符" },
        { "Sapphire", "amulet", "藍寶石護身符" },
        { "Ruby", "amulet", "紅寶石護身符" },
        { "Emerald", "amulet", "祖母綠護身符" },
        { "Diamond", "amulet", "鑽石護身符" },
        { "Striking", "wand", "打擊魔杖" },
        { "Storms", "wand", "風暴魔杖" },
        { "Shrapnel", "wand", "破片魔杖" },
        { "Lightning Strike", "rod", "雷擊魔棒" },
        { "Northwinds", "rod", "北風魔棒" },
        { "Dragonfire", "rod", "龍焰魔棒" },
        { "Glaurung's Blood", "rod", "格勞龍之血魔棒" },
        { "& Small jeweled chest~", "chest", "小型鑲寶石寶箱" },
        { "& Large jeweled chest~", "chest", "大型鑲寶石寶箱" },
    };
    char composed[128];
    char suffix[160];
    char out[128];
    const char *translated;
    size_t end;
    size_t i;

    CHECK(sizeof(ego_names) / sizeof(ego_names[0]) == 118);
    CHECK(ANGDROID_ZH_TW_FAANGBAND_EGO_NAMES == 66);
    CHECK(sizeof(fa_kinds) / sizeof(fa_kinds[0]) == 39);

    for (i = 0; i < sizeof(ego_names) / sizeof(ego_names[0]); ++i) {
        translated = angdroid_faangband_known_ego_ui_name(
            ego_names[i], true);
        CHECK(translated != NULL);
        CHECK(*translated != '\0');
        CHECK(strcmp(ego_names[i], translated) != 0);
        CHECK(angdroid_faangband_known_ego_ui_name(
            ego_names[i], false) == NULL);
        CHECK(angdroid_zh_tw_ego_name(ego_names[i]) ||
              angdroid_zh_tw_faangband_ego_name(ego_names[i]));
    }
    for (i = 0; i < sizeof(fa_kinds) / sizeof(fa_kinds[0]); ++i) {
        const char *actual = angdroid_faangband_known_kind_ui_name(
            fa_kinds[i].name, fa_kinds[i].tval, true);
        CHECK(actual != NULL);
        CHECK(strcmp(actual, fa_kinds[i].expected) == 0);
        CHECK(angdroid_faangband_known_kind_ui_name(
            fa_kinds[i].name, fa_kinds[i].tval, false) == NULL);
    }

    CHECK(strcmp(angdroid_faangband_known_kind_ui_name(
        "& Long Sword~", "sword", true), "長劍") == 0);
    CHECK(strcmp(angdroid_faangband_known_kind_ui_name(
        "& Main Gauche~", "sword", true), "副手短劍") == 0);
    CHECK(strcmp(angdroid_faangband_known_ego_ui_name(
        "of the Noldor", true), "諾多精靈") == 0);
    CHECK(strcmp(angdroid_faangband_known_ego_ui_name(
        "of Slay Dragon", true), "屠龍") == 0);
    CHECK(angdroid_faangband_known_kind_ui_name(NULL, "sword", true) == NULL);
    CHECK(angdroid_faangband_known_kind_ui_name("anything", NULL, true) == NULL);
    CHECK(angdroid_faangband_known_kind_ui_name("unlisted", "sword", true) == NULL);
    CHECK(angdroid_faangband_known_kind_ui_name(
        "unlisted", "sword", false) == NULL);
    CHECK(angdroid_faangband_known_ego_ui_name(NULL, true) == NULL);
    CHECK(angdroid_faangband_known_ego_ui_name("unlisted", true) == NULL);

    /* An already identified unflavored kind has a complete Chinese noun.
     * Classification applies only to the display string, not its source. */
    CHECK(angdroid_zh_tw_known_item_name(
        composed, sizeof(composed), "sword", "長劍", true, 2));
    CHECK(strcmp(composed, "2 把長劍") == 0);
    CHECK(angdroid_zh_tw_known_item_name(
        composed, sizeof(composed), "sword", "長劍", true, 1));
    CHECK(strcmp(composed, "長劍") == 0);
    CHECK(angdroid_zh_tw_known_item_name(
        composed, sizeof(composed), "sword", "長劍", true, 0));
    CHECK(strcmp(composed, "沒有剩餘的長劍") == 0);

    /* Identified effect adds the noun once, but never reveals an
     * unidentified effect: caller decides whether the effect is known. */
    CHECK(angdroid_zh_tw_identified_flavor_name(
        composed, sizeof(composed), "wand", "風暴"));
    CHECK(strcmp(composed, "風暴魔杖") == 0);
    CHECK(angdroid_zh_tw_identified_flavor_name(
        composed, sizeof(composed), "wand", "風暴魔杖"));
    CHECK(strcmp(composed, "風暴魔杖") == 0);

    CHECK(angdroid_zh_tw_item_suffix(
        suffix, sizeof(suffix), "諾多精靈"));
    CHECK(strcmp(suffix, "（諾多精靈）") == 0);
    CHECK(angdroid_zh_tw_item_suffix(
        suffix, sizeof(suffix), "（矮人製）"));
    CHECK(strcmp(suffix, "（矮人製）") == 0);

    strcpy(out, "長劍");
    end = strlen(out);
    CHECK(angdroid_obj_desc_append_suffix_atomic(
        out, sizeof(out), &end, "（諾多精靈）", false));
    CHECK(strcmp(out, "長劍（諾多精靈）") == 0);
    printf("PASS: 118 FA ego labels, 39 exclusive kinds, safe UI naming (%lu checks)\n", checks);
    return 0;
}
