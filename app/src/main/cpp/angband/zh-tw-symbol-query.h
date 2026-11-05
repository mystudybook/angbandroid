/* Display-only labels for the original Angband symbol-query command.
 * Canonical tval strings from list-tvals.h and gameplay IDs stay English.
 * Unknown keys and English mode preserve the source label verbatim.
 */
#ifndef ANGDROID_ZH_TW_SYMBOL_QUERY_H
#define ANGDROID_ZH_TW_SYMBOL_QUERY_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT 36

static inline const char *angdroid_symbol_tval_name(const char *canonical,
                                                    int zh_tw)
{
    static const struct { const char *en; const char *zh; }
        names[ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT] = {
        { "none", "無" },
        { "chest", "箱子" },
        { "shot", "投石彈" },
        { "arrow", "箭矢" },
        { "bolt", "弩箭" },
        { "bow", "弓" },
        { "digger", "挖掘工具" },
        { "hafted", "鈍擊武器" },
        { "polearm", "長柄武器" },
        { "sword", "劍" },
        { "boots", "靴子" },
        { "gloves", "手套" },
        { "helm", "頭盔" },
        { "crown", "冠冕" },
        { "shield", "盾牌" },
        { "cloak", "披風" },
        { "soft armor", "軟甲" },
        { "hard armor", "硬甲" },
        { "dragon armor", "龍鱗甲" },
        { "light", "光源" },
        { "amulet", "護身符" },
        { "ring", "戒指" },
        { "staff", "法杖" },
        { "wand", "魔杖" },
        { "rod", "魔棒" },
        { "scroll", "卷軸" },
        { "potion", "藥水" },
        { "flask", "油瓶" },
        { "food", "食物" },
        { "mushroom", "蘑菇" },
        { "magic book", "魔法書" },
        { "prayer book", "祭司法術書" },
        { "nature book", "自然法術書" },
        { "shadow book", "暗影法術書" },
        { "other book", "神祕法術書" },
        { "gold", "金錢" },
    };
    size_t i;
    if (!canonical || !zh_tw) return canonical;
    for (i = 0; i < ANGDROID_ZH_TW_SYMBOL_TVAL_COUNT; ++i) {
        if (strcmp(canonical, names[i].en) == 0) return names[i].zh;
    }
    return canonical;
}

/* Only translated labels receive Chinese punctuation. Unknown source labels
 * retain the exact English fallback, including its final full stop.
 * The symbol itself is never translated or substituted.
 */
static inline void angdroid_symbol_query_format(char *out, size_t capacity,
                                                char sym, const char *canonical,
                                                const char *localized, int zh_tw)
{
    const char *label;
    const char *period;
    if (!out || !capacity) return;
    if (!canonical) canonical = "";
    label = (zh_tw && localized) ? localized : canonical;
    period = (zh_tw && strcmp(label, canonical) != 0) ? "。" : ".";
    (void)snprintf(out, capacity, "%c - %s%s", sym, label, period);
}

#endif /* ANGDROID_ZH_TW_SYMBOL_QUERY_H */
