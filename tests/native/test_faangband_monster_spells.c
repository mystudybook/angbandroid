/* Bilingual FAangband-only winged dragon, 7 lore and 14 spell templates. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-symbol-query.h"
#include "../../app/src/main/cpp/angband/zh-tw-mon-lore-terms.h"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-text.h"
static unsigned checks;
#define CHECK(cond) do { assert(cond); ++checks; } while (0)
int main(void)
{
    static const struct {const char *en, *zh;} lore[] = {
        { "ice", "冰霜" },
        { "storm", "暴風" },
        { "dragonfire", "龍焰" },
        { "create hunger", "使人飢餓" },
        { "summon swamp creatures", "召喚沼澤生物" },
        { "summon ants", "召喚螞蟻" },
        { "summon winged dragons", "召喚有翼巨龍" },
    };
    static const struct {const char *en, *zh;} spells[] = {
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
    CHECK(sizeof lore / sizeof lore[0] == 7);
    CHECK(sizeof spells / sizeof spells[0] == 14);
    CHECK(strcmp(angdroid_symbol_monster_base_text("flying dragon", "Winged Dragon", 1),
                 "有翼巨龍") == 0);
    CHECK(angdroid_symbol_monster_base_text("flying dragon", "Winged Dragon", 0)
          != NULL);
    CHECK(strcmp(angdroid_symbol_monster_base_text("dragon", "Dragon", 1), "龍") == 0);
    CHECK(strcmp(angdroid_symbol_monster_base_text("flying dragon", "Modified", 1),
                 "Modified") == 0);
    CHECK(strcmp(angdroid_symbol_monster_base_text("unknown", "Winged Dragon", 1),
                 "Winged Dragon") == 0);
    CHECK(angdroid_symbol_monster_base_text(NULL, "Winged Dragon", 1) != NULL);
    CHECK(angdroid_symbol_monster_base_text("flying dragon", NULL, 1) == NULL);
    for (i = 0; i < sizeof lore / sizeof lore[0]; ++i) {
        CHECK(strcmp(angdroid_zh_tw_monster_spell(lore[i].en, 1), lore[i].zh) == 0);
        CHECK(angdroid_zh_tw_monster_spell(lore[i].en, 0) == lore[i].en);
    }
    for (i = 0; i < sizeof spells / sizeof spells[0]; ++i) {
        CHECK(strcmp(angdroid_mon_spell_text(spells[i].en, 1), spells[i].zh) == 0);
        CHECK(angdroid_mon_spell_text(spells[i].en, 0) == spells[i].en);
    }
    /* Existing Angband translations must still take priority. */
    CHECK(strcmp(angdroid_zh_tw_monster_spell("summon ancient dragons", 1),
                 "召喚遠古巨龍") == 0);
    CHECK(strcmp(angdroid_mon_spell_text("You hear a twang.", 1),
                 "你聽見弓弦聲。") == 0);
    CHECK(strcmp(angdroid_zh_tw_monster_spell("unknown lore", 1),
                 "unknown lore") == 0);
    CHECK(strcmp(angdroid_mon_spell_text("unknown spell text", 1),
                 "unknown spell text") == 0);
    CHECK(angdroid_zh_tw_monster_spell(NULL, 1) == NULL);
    CHECK(angdroid_mon_spell_text(NULL, 1) == NULL);
    puts("PASS: FAangband monster base, 7 lore and 14 spell templates");
    return 0;
}
