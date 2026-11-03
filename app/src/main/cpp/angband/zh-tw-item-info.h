/* Display-only item information strings from original Angband.
 * Only texts already permitted by the game's identification gates reach
 * this helper. Do not translate object state or game-data descriptors here.
 */
#ifndef ANGDROID_ZH_TW_ITEM_INFO_H
#define ANGDROID_ZH_TW_ITEM_INFO_H
#include <string.h>
#include <stddef.h>
struct angdroid_item_info_label { const char *en; const char *zh; };
static inline const char *angdroid_item_info_text(const char *en, int zh_tw)
{
    static const struct angdroid_item_info_label labels[] = {
        {"  No fuel required.", "  不需要燃料。"},
        {"  Cannot be refueled.", "  無法補充燃料。"},
        {"\nYou can read this book.\n", "\n你可以閱讀這本書。\n"},
        {"It can be eaten.\n", "這件物品可以食用。\n"},
        {"It can be drunk.\n", "這件物品可以飲用。\n"},
        {"It can be read.\n", "這件物品可以閱讀。\n"},
        {"It can be aimed.\n", "這件物品可以瞄準使用。\n"},
        {"It can be activated.\n", "這件物品可以啟動。\n"},
        {"It can be thrown at creatures with damaging effect.\n", "這件物品可以投擲攻擊怪物，造成傷害。\n"},
        {"Combat info:\n", "戰鬥資訊：\n"},
        {"You are too weak to use this weapon.\n", "你的力量不足以有效使用這把武器。\n"},
        {"It provides one random higher resistance.  ", "提供一項隨機高階抗性。  "},
        {"It provides one random sustain.  ", "提供一項隨機能力維持效果。  "},
        {"It provides one random ability.  ", "提供一項隨機能力。  "},
        {"It provides one random ability or base resistance.  ", "提供一項隨機能力或基本抗性。  "},
        {"\n\nYou do not know what this is.\n", "\n\n你還不知道這件物品是什麼。\n"},
        {"You do not know the full extent of this item's powers.\n", "你還未完全了解這件物品的力量。\n"},
        {"\n\nThis item does not seem to possess any special abilities.", "\n\n這件物品似乎沒有任何特殊能力。"},
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(labels)/sizeof(labels[0]); ++i) {
        if (strcmp(en, labels[i].en) == 0) return labels[i].zh;
    }
    return en;
}
#endif
