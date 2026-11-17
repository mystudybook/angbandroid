/* Regression for the production class-book display dictionary. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-class-book-names.h"

static unsigned long checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)

int main(void)
{
    static const struct { const char *title, *tval, *display; } cases[] = {
        { "[First Spells]", "magic book", "魔法書《初階法術》" },
        { "[Attacks and Knowledge]", "magic book", "魔法書《攻擊與知識》" },
        { "[Magical Defences]", "magic book", "魔法書《魔法防護》" },
        { "[Arcane Control]", "magic book", "魔法書《奧術操控》" },
        { "[Wizard's Tome of Power]", "magic book", "魔法書《法師威能秘典》" },
        { "[Lesser Charms]", "nature book", "自然法術書《初級咒法》" },
        { "[Gifts of Nature]", "nature book", "自然法術書《自然恩賜》" },
        { "[Creature Dominion]", "nature book", "自然法術書《生靈統御》" },
        { "[Nature Craft]", "nature book", "自然法術書《自然技藝》" },
        { "[Wild Forces]", "nature book", "自然法術書《野性之力》" },
        { "[Novice's Handbook]", "prayer book", "祈禱書《新手手冊》" },
        { "[Cleansing Power]", "prayer book", "祈禱書《淨化之力》" },
        { "[Healing and Sanctuary]", "prayer book", "祈禱書《治療與庇護》" },
        { "[Battle Blessings]", "prayer book", "祈禱書《戰鬥祝福》" },
        { "[Wrath of the Valar]", "prayer book", "祈禱書《維拉之怒》" },
        { "[Into the Shadows]", "shadow book", "暗影秘典《步入暗影》" },
        { "[Dark Rituals]", "shadow book", "暗影秘典《黑暗儀式》" },
        { "[Fear and Torment]", "shadow book", "暗影秘典《恐懼與折磨》" },
        { "[Deadly Powers]", "shadow book", "暗影秘典《致命之力》" },
        { "[Corruption of Spirit]", "shadow book", "暗影秘典《靈魂腐化》" },
    };
    size_t i, j;
    const char *types[] = {
        "magic book", "nature book", "prayer book", "shadow book",
        "other book", "ring", "scroll", ""
    };
    CHECK(ANGDROID_ZH_TW_CLASS_BOOK_COUNT == 20);
    CHECK(sizeof(cases) / sizeof(cases[0]) == 20);
    for (i = 0; i < 20; ++i) {
        const char *translated = angdroid_zh_tw_class_book_name(
            cases[i].title, cases[i].tval);
        CHECK(translated != NULL);
        CHECK(strcmp(translated, cases[i].display) == 0);
        CHECK(strstr(translated, "《") != NULL);
        CHECK(strstr(translated, "》") != NULL);
        CHECK(strchr(translated, '[') == NULL);
        CHECK(strchr(translated, ']') == NULL);
        for (j = 0; j < sizeof(types) / sizeof(types[0]); ++j) {
            const char *value = angdroid_zh_tw_class_book_name(
                cases[i].title, types[j]);
            if (strcmp(types[j], cases[i].tval) == 0) {
                CHECK(value != NULL);
                CHECK(strcmp(value, cases[i].display) == 0);
            } else CHECK(value == NULL);
        }
    }
    CHECK(angdroid_zh_tw_class_book_name("[Unknown Future Book]",
        "magic book") == NULL);
    CHECK(angdroid_zh_tw_class_book_name("[First Spells]",
        "prayer book") == NULL);
    CHECK(angdroid_zh_tw_class_book_name(NULL, "magic book") == NULL);
    CHECK(angdroid_zh_tw_class_book_name("[First Spells]", NULL) == NULL);
    CHECK(angdroid_zh_tw_class_book_name("[First Spells]", "") == NULL);
    printf("PASS: 20 source class book titles (%lu assertions)\n", checks);
    return 0;
}
