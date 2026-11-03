/* Exact presentation-only translations of original Angband pain.txt.
 * The same race-specific pain type and severity selects the English source
 * first, then this function optionally supplies a Chinese display sentence.
 */
#ifndef ANGDROID_ZH_TW_MON_PAIN_H
#define ANGDROID_ZH_TW_MON_PAIN_H
#include <string.h>
#include <stddef.h>
#define ANGDROID_ZH_TW_PAIN_PHRASES 65
struct angdroid_zh_pain_pair { const char *en; const char *zh; };
static inline const char *angdroid_zh_tw_pain_message(const char *source)
{
    static const struct angdroid_zh_pain_pair entries[ANGDROID_ZH_TW_PAIN_PHRASES] = {
        { "shrug[s] off the attack.", "擋下了這次攻擊。" },
        { "grunt[s] with pain.", "痛得悶哼一聲。" },
        { "cr[ies|y] out in pain.", "痛得大叫起來。" },
        { "scream[s] in pain.", "發出痛苦的尖叫。" },
        { "scream[s] in agony.", "發出淒厲的慘叫。" },
        { "writhe[s] in agony.", "痛苦地扭動著。" },
        { "cr[ies|y] out feebly.", "虛弱地哀叫。" },
        { "barely notice[s].", "幾乎沒有察覺這次攻擊。" },
        { "flinch[es].", "畏縮了一下。" },
        { "squelch[es].", "發出濕黏的聲響。" },
        { "quiver[s] in pain.", "痛得顫抖。" },
        { "writhe[s] about.", "不停地扭動。" },
        { "jerk[s] limply.", "無力地抽搐。" },
        { "snarl[s] with pain.", "痛得低聲咆哮。" },
        { "yelp[s] in pain.", "發出痛苦的短嚎。" },
        { "howl[s] in pain.", "痛得嚎叫。" },
        { "howl[s] in agony.", "發出淒厲的長嚎。" },
        { "yelp[s] feebly.", "虛弱地嗚咽。" },
        { "ignore[s] the attack.", "對這次攻擊毫不在意。" },
        { "squeal[s] in pain.", "痛得尖聲叫喊。" },
        { "shriek[s] in pain.", "發出痛苦的尖嘯。" },
        { "shriek[s] in agony.", "發出極其淒厲的尖嘯。" },
        { "hiss[es].", "發出嘶嘶聲。" },
        { "rear[s] up in anger.", "憤怒地挺起身子。" },
        { "hiss[es] furiously.", "憤怒地嘶嘶作響。" },
        { "snarl[s].", "發出低沉的咆哮。" },
        { "growl[s] angrily.", "憤怒地吼叫。" },
        { "hiss[es] in pain.", "痛得發出嘶嘶聲。" },
        { "mewl[s] in pain.", "痛苦地嗚咽。" },
        { "hiss[es] in agony.", "痛苦地不停嘶嘶作響。" },
        { "mewl[s] pitifully.", "可憐地低聲哀鳴。" },
        { "drone[s] angrily.", "憤怒地嗡嗡作響。" },
        { "scuttle[s] about.", "驚慌地四處亂竄。" },
        { "twitch[es] in pain.", "痛得抽動。" },
        { "jerk[s] in pain.", "痛得猛烈抽搐。" },
        { "jerk[s] in agony.", "劇痛中不停抽搐。" },
        { "jerk[s] feebly.", "虛弱地抽搐。" },
        { "flap[s] angrily.", "憤怒地拍打翅膀。" },
        { "jeer[s] in pain.", "痛苦地發出嘲叫。" },
        { "squawk[s] with pain.", "痛得嘎嘎亂叫。" },
        { "twitter[s] in agony.", "痛苦地急促鳴叫。" },
        { "flutter[s] about.", "不停地振翅亂飛。" },
        { "chirp[s] feebly.", "虛弱地啁啾。" },
        { "jerk[s].", "猛地抽動。" },
        { "rattle[s].", "發出喀啦聲。" },
        { "clatter[s].", "發出鏗鏘聲。" },
        { "shake[s].", "不斷搖晃。" },
        { "stagger[s].", "踉蹌了一下。" },
        { "crumple[s].", "癱倒成一團。" },
        { "grunt[s].", "發出悶哼。" },
        { "moan[s].", "低聲呻吟。" },
        { "groan[s].", "發出痛苦的呻吟。" },
        { "hesitate[s].", "動作遲疑了。" },
        { "spin[s] fiercely.", "猛烈旋轉著。" },
        { "swirl[s] about.", "飛快地盤旋。" },
        { "twist[s] around.", "不停地扭轉。" },
        { "spin[s] slowly.", "緩慢旋轉。" },
        { "swirl[s] weakly.", "虛弱地盤旋。" },
        { "twist[s] limply.", "無力地扭動。" },
        { "laugh[s] off the attack.", "笑著無視這次攻擊。" },
        { "sneer[s].", "露出輕蔑的冷笑。" },
        { "scowl[s].", "怒目而視。" },
        { "bellow[s] in rage.", "憤怒地咆哮。" },
        { "scream[s] in fury.", "狂怒地尖叫。" },
        { "wince[s].", "痛得皺起了臉。" },
    };
    size_t i;
    if (!source) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_PAIN_PHRASES; ++i)
        if (strcmp(source, entries[i].en) == 0)
            return entries[i].zh;
    return NULL;
}
#endif
