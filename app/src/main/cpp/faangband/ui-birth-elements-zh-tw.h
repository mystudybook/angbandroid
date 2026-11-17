/* FAangband birth-sheet element labels. Display only: keep ELEM_ IDs,
 * stored percentage-damage resistances, player ability records and
 * all save data unchanged. A NULL name means retain the source text.
 */
#ifndef ANGDROID_FA_BIRTH_ELEMENTS_ZH_TW_H
#define ANGDROID_FA_BIRTH_ELEMENTS_ZH_TW_H
#include <stddef.h>
#include "player.h"

/* The element ID is taken from the canonical list-elements.h enum.
 * Do not translate ability->name to determine the index. */
static inline const char *angdroid_fa_birth_element_name(int element)
{
    switch (element) {
    case ELEM_ACID: return "強酸";
    case ELEM_ELEC: return "閃電";
    case ELEM_FIRE: return "火焰";
    case ELEM_COLD: return "寒冷";
    case ELEM_POIS: return "毒素";
    case ELEM_LIGHT: return "光明";
    case ELEM_DARK: return "黑暗";
    case ELEM_SOUND: return "聲波";
    case ELEM_SHARD: return "碎片";
    case ELEM_NEXUS: return "空間連結";
    case ELEM_NETHER: return "冥界能量";
    case ELEM_CHAOS: return "混沌";
    case ELEM_DISEN: return "消魔";
    case ELEM_WATER: return "水";
    case ELEM_ICE: return "冰";
    case ELEM_GRAVITY: return "重力";
    case ELEM_INERTIA: return "惰性";
    case ELEM_FORCE: return "力場";
    case ELEM_TIME: return "時間";
    case ELEM_PLASMA: return "電漿";
    case ELEM_STORM: return "風暴";
    case ELEM_DRAGONFIRE: return "龍焰";
    case ELEM_HELLFIRE: return "獄火";
    case ELEM_METEOR: return "隕石";
    case ELEM_MISSILE: return "飛彈";
    case ELEM_MANA: return "魔力";
    case ELEM_HOLY_ORB: return "神聖能量";
    case ELEM_ARROW: return "箭矢";
    default: return NULL;
    }
}

/* In FAangband resistances are the percentage of incoming damage:
 * 100 is normal, 0 is immunity, 1–99 are resistances, and >100 is
 * vulnerability. Unknown/baseline values retain the source label.
 * Return 2=immune, 1=resist, -1=vulnerable, 0=unchanged. */
static inline int angdroid_fa_birth_element_resistance(int percentage)
{
    if (percentage == RES_LEVEL_MAX) return 2;
    if (percentage > RES_LEVEL_MAX && percentage < RES_LEVEL_BASE)
        return 1;
    if (percentage > RES_LEVEL_BASE) return -1;
    return 0;
}
#endif /* ANGDROID_FA_BIRTH_ELEMENTS_ZH_TW_H */
