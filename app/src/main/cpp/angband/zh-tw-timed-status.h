/* Display-only translations for the canonical player_timed.txt grade names.
 * Never localize timed_effects[].name, parser keys, thresholds or savefiles.
 * Both Angband and FAangband use these same 66 visible grade captions.
 */
#ifndef ANGDROID_ZH_TW_TIMED_STATUS_H
#define ANGDROID_ZH_TW_TIMED_STATUS_H
#include <stddef.h>
#include <string.h>

struct angdroid_timed_status_pair {
    const char *english;
    const char *zh_tw;
};

static inline const char *angdroid_timed_status_name(
    const char *english, int zh_tw)
{
    static const struct angdroid_timed_status_pair labels[] = {
        { "Haste", "加速" },
        { "Slow", "減速" },
        { "Blind", "失明" },
        { "Paralyzed!", "麻痺" },
        { "Confused", "混亂" },
        { "Afraid", "恐懼" },
        { "Halluc", "幻覺" },
        { "Poisoned", "中毒" },
        { "Graze", "擦傷" },
        { "Light Cut", "輕微割傷" },
        { "Bad Cut", "割傷" },
        { "Nasty Cut", "嚴重割傷" },
        { "Severe Cut", "重度割傷" },
        { "Deep Gash", "深裂傷" },
        { "Mortal Wound", "致命傷" },
        { "Stun", "暈眩" },
        { "Heavy Stun", "重度暈眩" },
        { "Knocked Out", "昏厥" },
        { "Starving", "極度飢餓" },
        { "Faint", "飢餓昏厥" },
        { "Weak", "虛弱" },
        { "Hungry", "飢餓" },
        { "Fed", "已進食" },
        { "Full", "飽足" },
        { "ProtEvil", "防邪" },
        { "Invuln", "無敵" },
        { "Hero", "英勇" },
        { "Berserk", "狂暴" },
        { "Shield", "護盾" },
        { "Blssd", "祝福" },
        { "SInvis", "識破隱形" },
        { "Infra", "紅外視覺" },
        { "RAcid", "抗酸" },
        { "RElec", "抗電" },
        { "RFire", "抗火" },
        { "RCold", "抗寒" },
        { "RPois", "抗毒" },
        { "RConf", "抗混亂" },
        { "Amnesiac", "失憶" },
        { "ESP", "心靈感應" },
        { "Stone", "石膚" },
        { "Terror", "驚恐" },
        { "Sprint", "衝刺" },
        { "Bold", "無懼" },
        { "Scrambled", "能力錯亂" },
        { "TrapSafe", "防陷阱" },
        { "FastCast", "快速施法" },
        { "AttAcid", "酸蝕攻擊" },
        { "AttElec", "電擊攻擊" },
        { "AttFire", "火焰攻擊" },
        { "AttCold", "寒冷攻擊" },
        { "AttPois", "毒素攻擊" },
        { "AttConf", "混亂攻擊" },
        { "AttEvil", "神聖攻擊" },
        { "AttDemon", "惡魔剋星" },
        { "AttVamp", "吸血攻擊" },
        { "Heal", "快速恢復" },
        { "Cmd", "指揮" },
        { "HitRun", "打帶跑" },
        { "NoTrack", "隱匿蹤跡" },
        { "PShot", "強力射擊" },
        { "Taunt", "挑釁" },
        { "Bloodlust", "嗜血" },
        { "BlkBrth", "黑色氣息" },
        { "Stealth", "潛行" },
        { "FrAct", "行動自由" },
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(labels) / sizeof(labels[0]); ++i) {
        if (strcmp(english, labels[i].english) == 0)
            return labels[i].zh_tw;
    }
    /* Unknown/modded labels must not silently change meaning. */
    return english;
}
#endif
