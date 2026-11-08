/* FAangband differs from Angband on two named spell descriptions. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-descriptions.h"

static unsigned long checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)

static void variant(const char *name, const char *source, const char *translated)
{
    char corrupted[600];
    const char *actual = angdroid_zh_tw_spell_description(name, source, 1);
    CHECK(actual != NULL);
    CHECK(strcmp(actual, translated) == 0);
    CHECK(strstr(actual, "。") != NULL);
    CHECK(angdroid_zh_tw_spell_description(name, source, 0) == source);
    CHECK(strlen(source) < sizeof(corrupted));
    strcpy(corrupted, source);
    corrupted[0] = corrupted[0] == 'X' ? 'Y' : 'X';
    CHECK(angdroid_zh_tw_spell_description(name, corrupted, 1) == corrupted);
    CHECK(angdroid_zh_tw_faangband_variant_description(name, corrupted) == NULL);
    CHECK(angdroid_zh_tw_faangband_variant_description(name, NULL) == NULL);
}

int main(void)
{
    const char *recall =
        "Teleports you from the dungeon or wilderness to the town, or from the town to any of four points you have visited and set.  The recall effect is not immediate; it is delayed by 14+1d20 turns.  During that delay, the prayer can be canceled by invoking the prayer of recall again.  This prayer has no effect when the option to restrict the use of stairs and recall is set, unless Morgoth is dead.";
    const char *berserk =
        "Removes fear and grants you berserk strength (resistance to fear, +72 melee skill, -10 to AC, and prevents healing over time) for a level-dependent duration.";
    CHECK(ANGDROID_ZH_TW_FAANGBAND_VARIANT_DESC_COUNT == 2);
    variant("Word of Recall", recall,
        "從地城或荒野返回城鎮，或從城鎮傳送至曾到訪並設定的四個地點之一。召回效果將在 14+1d20 回合後生效；等待期間再次施放召回祈禱可以取消。若啟用了限制樓梯與召回的選項，除非魔苟斯已死亡，否則此祈禱無效。");
    variant("Berserk Strength", berserk,
        "解除恐懼並獲得狂暴力量：抵抗恐懼、近戰技能提高 72 點、防禦等級（AC）降低 10 點，且期間無法隨時間自然恢復生命值。持續時間取決於角色等級。");
    CHECK(angdroid_zh_tw_faangband_variant_description(
        "Berserk Strength", recall) == NULL);
    CHECK(angdroid_zh_tw_faangband_variant_description(
        "Word of Recall", berserk) == NULL);
    CHECK(angdroid_zh_tw_faangband_variant_description(NULL, recall) == NULL);
    CHECK(angdroid_zh_tw_faangband_variant_description(NULL, NULL) == NULL);
    CHECK(angdroid_zh_tw_spell_description("Word of Recall", NULL, 1) == NULL);
    CHECK(angdroid_zh_tw_spell_description("Word of Recall", recall, 0) == recall);
    /* Original canonical descriptions remain a separate, earlier match.
     * The FA variant must never be substituted for Angband's values. */
    {
        const char *standard_berserk =
            "Removes fear and grants you berserk strength (resistance to fear, +75 melee skill, -10 to AC, and prevents healing over time) for a level-dependent duration.";
        const char *original = angdroid_zh_tw_spell_description(
            "Berserk Strength", standard_berserk, 1);
        CHECK(original != NULL);
        CHECK(strstr(original, "75") != NULL);
        CHECK(strstr(original, "72") == NULL);
        CHECK(angdroid_zh_tw_faangband_variant_description(
            "Berserk Strength", standard_berserk) == NULL);
    }
    /* The two FAangband-only spell translations from the last batch remain. */
    CHECK(strcmp(angdroid_zh_tw_spell_description("Timeless Blink",
        "Teleports you to a square of your choice within your line of sight.  This spell takes no time.", 1),
        "傳送至視線內你所選擇的位置。此法術不消耗時間。") == 0);
    CHECK(strcmp(angdroid_zh_tw_spell_description("Fume of Thangorodrim",
        "Darkens, maps out, and detects all objects on the complete current dungeon level.", 1),
        "使目前整個地城樓層陷入黑暗、繪製地圖，並偵測該樓層的所有物品。") == 0);
    printf("PASS: FAangband two divergent spell descriptions (%lu assertions)\n", checks);
    return 0;
}
