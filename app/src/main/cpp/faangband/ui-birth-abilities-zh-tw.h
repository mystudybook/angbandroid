/* FAangband race/class trait labels. Presentation only: never modify
 * player_abilities, ability indexes, flags, the parser or savefile data.
 * A type-specific flag is used to distinguish equal numeric indexes in
 * the player and object flag namespaces. Unknown labels stay English.
 */
#ifndef ANGDROID_FA_BIRTH_ABILITIES_ZH_TW_H
#define ANGDROID_FA_BIRTH_ABILITIES_ZH_TW_H

#include <string.h>
#include "player.h"
#include "../angband/zh-tw-ui.h"

static inline const char *angdroid_fa_birth_ability_name(
    const char *english, const char *type, int index, int zh_tw)
{
    if (!english || !zh_tw || !type) return english;
    if (strcmp(type, "player") == 0) {
        switch (index) {
        /* Race and ancestry traits */
        case PF_ROCK: return "岩石身軀";
        case PF_KNOW_MUSHROOM: return "蘑菇知識";
        case PF_SEE_ORE: return "感知礦脈";
        case PF_SWORD_SKILL: return "劍術專精";
        case PF_POLEARM_SKILL: return "長柄武器專精";
        case PF_HAFTED_SKILL: return "鈍器專精";
        case PF_SLING_SKILL: return "投石索專精";
        case PF_BOW_SKILL: return "弓術專精";
        case PF_XBOW_SKILL: return "弩術專精";
        case PF_SWORD_UNSKILL: return "不擅長劍術";
        case PF_POLEARM_UNSKILL: return "不擅長長柄武器";
        case PF_HAFTED_UNSKILL: return "不擅長鈍器";
        case PF_SLING_UNSKILL: return "不擅長投石索";
        case PF_BOW_UNSKILL: return "不擅長弓術";
        case PF_XBOW_UNSKILL: return "不擅長弩術";
        case PF_HARDY: return "堅韌體質";
        case PF_FLYING: return "飛行能力";
        case PF_SHADOW: return "暗影形態";
        case PF_WOODEN: return "木質身軀";
        case PF_BEARSKIN: return "熊皮護體";
        case PF_PLAINSMAN: return "平原居民";
        case PF_ELVEN: return "精靈血統";
        case PF_DWARVEN: return "矮人血統";
        case PF_EDAIN: return "伊甸人血統";
        /* Class traits and specialties */
        case PF_BRAVERY_30: return "30 級勇氣";
        case PF_BLESS_WEAPON: return "武器祝福";
        case PF_BEAM: return "光束施法";
        case PF_CHOOSE_SPELLS: return "自選法術";
        case PF_NO_MANA: return "不使用魔力";
        case PF_CHARM: return "魅惑";
        case PF_UNLIGHT: return "暗影之力";
        case PF_STEAL: return "偷竊技巧";
        case PF_SHIELD_BASH: return "盾擊";
        case PF_EVIL: return "邪惡力量";
        case PF_COMBAT_REGEN: return "戰鬥再生";
        case PF_BOW_SPEED_GOOD: return "加快弓箭射擊";
        case PF_BOW_SPEED_GREAT: return "大幅加快弓箭射擊";
        case PF_SLING_SPEED_GOOD: return "加快投石索射擊";
        case PF_SLING_SPEED_GREAT: return "大幅加快投石索射擊";
        case PF_XBOW_SPEED_GOOD: return "加快弩箭射擊";
        case PF_XBOW_SPEED_GREAT: return "大幅加快弩箭射擊";
        case PF_BACKSTAB: return "背刺";
        case PF_UNARMED_COMBAT: return "徒手格鬥";
        case PF_DEVICE_EXPERT: return "魔法裝置專家";
        case PF_STRONG_MAGIC: return "強化魔法";
        case PF_HOLY: return "神聖之力";
        case PF_TRAP: return "陷阱技巧";
        case PF_WOODSMAN: return "森林求生";
        case PF_XTRA_SPECIALTY: return "額外專精";
        case PF_ARMOR_MAST: return "護甲精通";
        case PF_SHIELD_MAST: return "盾牌精通";
        case PF_ARMOR_PROFICIENCY: return "護甲熟練";
        case PF_EVASION: return "閃避";
        case PF_MAGIC_RESIST: return "魔法抗性";
        case PF_PHASEWALK: return "相位移動";
        case PF_ARMSMAN: return "武器專家";
        case PF_FAST_ATTACK: return "快速攻擊";
        case PF_MARKSMAN: return "神射手";
        case PF_PIERCE_SHOT: return "穿透射擊";
        case PF_MIGHTY_THROW: return "強力投擲";
        case PF_POWER_STRIKE: return "強力打擊";
        case PF_MARTIAL_ARTS: return "武術";
        case PF_MANA_BURN: return "魔力灼燒";
        case PF_RAPID_FIRE: return "連射";
        case PF_BEGUILE: return "迷惑術";
        case PF_ENHANCE_MAGIC: return "魔法增幅";
        case PF_POWER_SIPHON: return "力量汲取";
        case PF_HEIGHTEN_MAGIC: return "魔法提升";
        case PF_SOUL_SIPHON: return "靈魂汲取";
        case PF_HARMONY: return "調和";
        case PF_CHANNELING: return "能量引導";
        case PF_ATHLETICS: return "體能訓練";
        case PF_CLARITY: return "心靈清明";
        case PF_FURY: return "狂怒";
        case PF_MEDITATION: return "冥想";
        case PF_REGENERATION: return "再生";
        case PF_EXTRA_TRAP: return "額外陷阱";
        case PF_HOLY_LIGHT: return "神聖之光";
        default: break;
        }
    } else if (strcmp(type, "object") == 0) {
        switch (index) {
        case OF_SUST_STR: return "維持力量";
        case OF_SUST_INT: return "維持智力";
        case OF_SUST_WIS: return "維持智慧";
        case OF_SUST_DEX: return "維持敏捷";
        case OF_SUST_CON: return "維持體質";
        case OF_PROT_FEAR: return "防護恐懼";
        case OF_PROT_BLIND: return "防護失明";
        case OF_PROT_CONF: return "防護混亂";
        case OF_PROT_STUN: return "防護昏迷";
        case OF_SLOW_DIGEST: return "減慢飢餓";
        case OF_FEATHER: return "輕盈落地";
        case OF_REGEN: return "生命再生";
        case OF_TELEPATHY: return "心靈感應";
        case OF_SEE_INVIS: return "看破隱形";
        case OF_FREE_ACT: return "行動自由";
        case OF_HOLD_LIFE: return "生命維持";
        case OF_IMPACT: return "強力衝擊";
        case OF_BLESSED: return "受祝福";
        case OF_ACCURATE: return "命中精準";
        case OF_VELOCITY: return "高速飛行";
        case OF_TRAP_IMMUNE: return "免疫陷阱";
        case OF_THROWING: return "投擲武器";
        case OF_PERFECT_BALANCE: return "完美平衡";
        default: break;
        }
    }
    return angdroid_game_ui_text(english, zh_tw);
}

#endif /* ANGDROID_FA_BIRTH_ABILITIES_ZH_TW_H */
