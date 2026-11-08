/* Class-book DISPLAY titles from Angband / FAangband class.txt.
 * Do not rewrite class_book, object_kind, tval, spell IDs or save data.
 */
#ifndef ANGDROID_ZH_TW_CLASS_BOOK_NAMES_H
#define ANGDROID_ZH_TW_CLASS_BOOK_NAMES_H
#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_CLASS_BOOK_COUNT 20

/* A book title is distinct from a spell effect.  Return a complete display
 * noun phrase, matched by both original bracketed title and source tval.
 * Unknown and cross-type names return NULL for the original English path.
 */
static inline const char *angdroid_zh_tw_class_book_name(
    const char *source_title, const char *tval)
{
    static const struct { const char *source, *tval, *display; }
        books[ANGDROID_ZH_TW_CLASS_BOOK_COUNT] = {
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
    size_t i;
    if (!source_title || !tval) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_CLASS_BOOK_COUNT; ++i)
        if (strcmp(source_title, books[i].source) == 0 &&
            strcmp(tval, books[i].tval) == 0)
            return books[i].display;
    return NULL;
}

#endif /* ANGDROID_ZH_TW_CLASS_BOOK_NAMES_H */
