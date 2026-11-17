/* Source-matched FAangband-only monster spell display templates.
 * Keep {name} and other canonical dynamic tags for existing expansion.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_MON_SPELL_MESSAGES_H
#define ANGDROID_ZH_TW_FAANGBAND_MON_SPELL_MESSAGES_H
#include <stddef.h>
#include <string.h>
static inline const char *angdroid_faangband_mon_spell_text(const char *en,
    int zh_tw)
{
    static const struct {const char *en, *zh;} map[] = {
        { "{name} breathes ice.", "{name}噴吐冰霜氣息。" },
        { "You hear a tinkling roar.", "你聽見清脆作響的咆哮聲。" },
        { "{name} breathes storm.", "{name}噴吐風暴氣息。" },
        { "You hear a tempestuous roar.", "你聽見暴風般的咆哮聲。" },
        { "{name} breathes dragonfire.", "{name}噴吐龍焰。" },
        { "You hear a violent roar.", "你聽見猛烈的咆哮聲。" },
        { "{name} gestures at you, and you suddenly feel hungry.", "{name}向你做了個手勢，你突然感到飢餓。" },
        { "You suddenly feel hungry.", "你突然感到飢餓。" },
        { "You resist the effects!", "你抵抗了這些效果！" },
        { "{name} summons swamp creaturess.", "{name}召喚了沼澤生物。" },
        { "You hear many squishy things appear nearby!", "你聽見附近出現許多濕黏生物的聲響！" },
        { "{name} summons ants.", "{name}召喚了螞蟻。" },
        { "You hear crawling things appear nearby!", "你聽見附近有爬行生物出現！" },
        { "{name} summons winged dragons.", "{name}召喚了有翼巨龍。" },
    };
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(en, map[i].en) == 0) return map[i].zh;
    return en;
}
#endif
