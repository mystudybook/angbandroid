/* FAangband-only trap names and knowledge descriptions. Display only.
 * Keep the original standard Angband trap arrays and canonical trap data intact.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_TRAPS_H
#define ANGDROID_ZH_TW_FAANGBAND_TRAPS_H
#include <stddef.h>
#include <string.h>
struct angdroid_fa_trap_pair { const char *english; const char *traditional; };
static inline const char *angdroid_faangband_trap_lookup(const char *en, int zh_tw,
    const struct angdroid_fa_trap_pair *pairs, size_t count)
{
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < count; ++i)
        if (strcmp(en, pairs[i].english) == 0) return pairs[i].traditional;
    return en;
}
static inline const char *angdroid_faangband_trap_name(const char *en, int zh_tw)
{
    static const struct angdroid_fa_trap_pair map[] = {
        { "falling branch", "落枝陷阱" },
        { "basic trap", "基礎怪物陷阱" },
        { "sturdy trap", "堅固怪物陷阱" },
        { "netted trap", "捕網陷阱" },
        { "confusion trap", "混亂陷阱" },
        { "poison trap", "毒霧怪物陷阱" },
        { "immaterial trap", "無實體怪物陷阱" },
        { "electric trap", "閃電怪物陷阱" },
        { "explosive trap", "爆炸怪物陷阱" },
        { "portal trap", "傳送門陷阱" },
        { "stasis trap", "停滯陷阱" },
        { "drain life trap", "生命吸取陷阱" },
        { "unmagic trap", "破魔陷阱" },
        { "dispelling trap", "驅散陷阱" },
        { "banishment trap", "放逐陷阱" },
    };
    return angdroid_faangband_trap_lookup(en, zh_tw, map,
        sizeof map / sizeof map[0]);
}
static inline const char *angdroid_faangband_trap_lore(const char *en, int zh_tw)
{
    static const struct angdroid_fa_trap_pair map[] = {
        { "A falling branch could be random, or an act of an enemy, animal or vegetable.", "可能是意外掉落，也可能是敵人、動物或植物造成的落枝。" },
        { "A basic monster trap.", "最基本的怪物陷阱。" },
        { "A monster trap that doesn't break easily.", "不容易損壞的怪物陷阱。" },
        { "A net trap to hit flying monsters.", "用網子捕捉飛行怪物的陷阱。" },
        { "A trap to confuse monsters.", "使怪物陷入混亂的陷阱。" },
        { "A monster trap that releases a cloud of poison.", "會釋放毒霧的怪物陷阱。" },
        { "A trap to hurt immaterial monsters.", "能傷害無實體怪物的陷阱。" },
        { "An electrified monster trap.", "帶有電流的怪物陷阱。" },
        { "A monster trap that creates an explosion.", "會引發爆炸的怪物陷阱。" },
        { "A trap that teleports monsters.", "會傳送怪物的陷阱。" },
        { "A trap that holds a monster in stasis.", "能讓怪物陷入停滯狀態的陷阱。" },
        { "A trap that hurts nearby living monsters.", "能傷害附近活物的陷阱。" },
        { "A trap that disenchants monsters.", "能解除怪物魔法效果的陷阱。" },
        { "A trap that hurts all monsters in sight.", "能傷害視野內所有怪物的陷阱。" },
        { "A trap that removes nearby like monsters.", "能將附近同類怪物移除的陷阱。" },
    };
    return angdroid_faangband_trap_lookup(en, zh_tw, map,
        sizeof map / sizeof map[0]);
}
#endif
