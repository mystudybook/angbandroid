/* Exact original Angband display descriptions. Never modify game data.
 * Unknown values and non-zh-TW callers retain their original English text.
 */
#ifndef ANGDROID_ZH_TW_PROJECTION_TERMS_H
#define ANGDROID_ZH_TW_PROJECTION_TERMS_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_projection_description(const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "acid", "酸液" },
        { "lightning", "閃電" },
        { "fire", "火焰" },
        { "cold", "寒氣" },
        { "poison", "毒素" },
        { "light", "光明" },
        { "dark", "黑暗" },
        { "sound", "聲波" },
        { "shards", "碎片" },
        { "nexus", "次元之力" },
        { "nether", "冥界之力" },
        { "chaos", "混沌" },
        { "disenchantment", "解除魔法" },
        { "water", "水流" },
        { "ice", "冰霜" },
        { "gravity", "重力" },
        { "inertia", "慣性" },
        { "force", "衝擊力" },
        { "time", "時間之力" },
        { "plasma", "電漿" },
        { "a meteor", "流星" },
        { "a missile", "魔法飛彈" },
        { "mana", "魔力" },
        { "a holy orb", "神聖光球" },
        { "an arrow", "箭矢" },
        { "darkness", "黑暗之力" },
        { "rock remover", "移除岩石" },
        { "destroys doors", "摧毀門扉" },
        { "disables traps", "解除陷阱" },
        { "creates doors", "創造門扉" },
        { "creates traps", "製造陷阱" },
        { "teleports undead away", "將不死生物傳送離開" },
        { "teleports evil monsters away", "將邪惡怪物傳送離開" },
        { "teleports monsters with a spirit away", "將有靈魂的怪物傳送離開" },
        { "teleports monsters away", "將怪物傳送離開" },
        { "turns undead", "驅退不死生物" },
        { "frightens evil monsters", "使邪惡怪物恐懼" },
        { "scares living monsters", "使活物恐懼" },
        { "causes monsters to flee", "使怪物逃跑" },
        { "damages undead", "傷害不死生物" },
        { "damages evil monsters", "傷害邪惡怪物" },
        { "damages all monsters", "傷害所有怪物" },
        { "attempts to put undead to sleep", "試圖使不死生物沉睡" },
        { "attempts to put evil monsters to sleep", "試圖使邪惡怪物沉睡" },
        { "attempts to put all monsters to sleep", "試圖使所有怪物沉睡" },
        { "hastes, heals and magically duplicates monsters", "加速、治療並複製怪物" },
        { "polymorphs monsters into other kinds of creatures", "將怪物變成其他生物" },
        { "heals monsters", "治療怪物" },
        { "hastes monsters", "加速怪物" },
        { "attempts to slow monsters", "試圖減緩怪物速度" },
        { "attempts to confuse monsters", "試圖使怪物混亂" },
        { "attempts to hold monsters still", "試圖使怪物無法行動" },
        { "attempts to stun monsters", "試圖使怪物暈眩" },
        { "damages living monsters", "傷害活物" },
        { "kills monsters below a hitpoint threshold", "消滅生命值低於門檻的怪物" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
