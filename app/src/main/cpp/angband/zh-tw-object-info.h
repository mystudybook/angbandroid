/* Display-only terms from original Angband projection/object/slay data. */
#ifndef ANGDROID_ZH_TW_OBJECT_INFO_H
#define ANGDROID_ZH_TW_OBJECT_INFO_H
#include <stddef.h>
#include <string.h>
struct angdroid_oi_term { const char *en; const char *zh; };
static inline const char *angdroid_zh_tw_object_term(const char *term, int zh_tw)
{
    static const struct angdroid_oi_term map[] = {
        {"acid", "強酸"},
        {"lightning", "閃電"},
        {"fire", "火焰"},
        {"cold", "寒冷"},
        {"poison", "毒素"},
        {"light", "光明"},
        {"dark", "黑暗"},
        {"sound", "聲波"},
        {"shards", "碎片"},
        {"nexus", "空間連結"},
        {"nether", "冥界能量"},
        {"chaos", "混沌"},
        {"disenchantment", "消魔"},
        {"water", "水"},
        {"ice", "冰"},
        {"gravity", "重力"},
        {"inertia", "惰性"},
        {"force", "力場"},
        {"time", "時間"},
        {"plasma", "電漿"},
        {"fear", "恐懼"},
        {"blindness", "致盲"},
        {"confusion", "混亂"},
        {"stunning", "震懾"},
        {"strength", "力量"},
        {"intelligence", "智力"},
        {"wisdom", "智慧"},
        {"dexterity", "敏捷"},
        {"constitution", "體質"},
        {"stealth", "潛行"},
        {"searching skill", "搜索能力"},
        {"infravision", "紅外線視覺"},
        {"tunneling", "挖掘"},
        {"speed", "速度"},
        {"damage reduction", "傷害減免"},
        {"extra blows", "額外攻擊次數"},
        {"shooting speed", "射擊速度"},
        {"shooting power", "射擊威力"},
        {"extra moves", "額外行動"},
        {"evil creatures", "邪惡生物"},
        {"animals", "動物"},
        {"orcs", "獸人"},
        {"trolls", "食人妖"},
        {"giants", "巨人"},
        {"demons", "惡魔"},
        {"dragons", "巨龍"},
        {"undead", "不死生物"},
        {"rubble", "碎石"},
        {"magma veins", "岩漿礦脈"},
        {"quartz veins", "石英礦脈"},
        {"granite", "花崗岩"},
    };
    size_t i;
    if (!term || !zh_tw) return term;
    for (i = 0; i < sizeof(map)/sizeof(map[0]); ++i)
        if (strcmp(term, map[i].en) == 0) return map[i].zh;
    return term;
}
#endif
