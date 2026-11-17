/* Angband 4.2.4 combat output translations for known, immutable verbs.
 * Unknown verbs and unknown result descriptors return NULL, so the caller
 * preserves the complete original English sentence. No game data is edited.
 */
#ifndef ANGDROID_ZH_TW_COMBAT_H
#define ANGDROID_ZH_TW_COMBAT_H

#include <stddef.h>
#include <string.h>

struct angdroid_combat_label {
    const char *english;
    const char *chinese;
};

static inline const char *angdroid_combat_lookup(
        const char *english, const struct angdroid_combat_label *pairs,
        size_t count)
{
    size_t i;
    if (!english) return NULL;
    for (i = 0; i < count; ++i) {
        if (strcmp(english, pairs[i].english) == 0)
            return pairs[i].chinese;
    }
    return NULL;
}

static inline const char *angdroid_melee_verb_zh_tw(const char *english)
{
    static const struct angdroid_combat_label pairs[] = {
        {"punch", "揮拳擊中了"},
        /* Shape-change attack verbs in both Angband and FAangband. */
        {"bite", "咬住了"},
        {"claw", "用利爪抓傷了"},
        {"clout", "猛擊了"},
        {"crush", "重壓了"},
        {"cuff", "掌擊了"},
        {"peck", "啄擊了"},
        {"scratch", "抓傷了"},
        {"slash", "劈砍了"},
        {"strike", "擊打了"},
        {"swipe", "猛抓了"},
        {"tear", "撕裂了"},
        {"thump", "重捶了"},
        {"hit", "擊中了"},
        {"fail to harm", "未能傷害"},
        {"dissolve", "以強酸腐蝕了"},
        {"shock", "以閃電電擊了"},
        {"burn", "用火焰灼燒了"},
        {"freeze", "冰凍了"},
        {"poison", "以毒素傷害了"},
        {"corrode", "腐蝕了"},
        {"zap", "電擊了"},
        {"singe", "燒灼了"},
        {"chill", "以寒氣侵襲了"},
        {"sicken", "以疫病侵襲了"},
        {"smite", "重擊了"},
        {"fiercely smite", "狠狠重擊了"},
    };
    return angdroid_combat_lookup(english, pairs,
            sizeof(pairs) / sizeof(pairs[0]));
}

static inline const char *angdroid_ranged_verb_zh_tw(const char *english)
{
    static const struct angdroid_combat_label pairs[] = {
        {"hits", "擊中了"},
        {"fails to harm", "未能傷害"},
        {"dissolves", "以強酸腐蝕了"},
        {"shocks", "以閃電電擊了"},
        {"burns", "用火焰灼燒了"},
        {"freezes", "冰凍了"},
        {"poisons", "以毒素傷害了"},
        {"corrodes", "腐蝕了"},
        {"zaps", "電擊了"},
        {"singes", "燒灼了"},
        {"chills", "以寒氣侵襲了"},
        {"sickens", "以疫病侵襲了"},
        {"pierces", "貫穿了"},
        {"deeply pierces", "深深貫穿了"},
    };
    return angdroid_combat_lookup(english, pairs,
            sizeof(pairs) / sizeof(pairs[0]));
}

static inline const char *angdroid_hit_quality_zh_tw(const char *english)
{
    static const struct angdroid_combat_label pairs[] = {
        {"It was a good hit!", "這是一次漂亮的攻擊！"},
        {"It was a great hit!", "這次攻擊非常有效！"},
        {"It was a superb hit!", "這是一次絕佳的攻擊！"},
        {"It was a *GREAT* hit!", "這是一次強力重擊！"},
        {"It was a *SUPERB* hit!", "這是一次極其猛烈的重擊！"},
    };
    return angdroid_combat_lookup(english, pairs,
            sizeof(pairs) / sizeof(pairs[0]));
}
#endif
