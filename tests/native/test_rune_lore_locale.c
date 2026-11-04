/* Display-only rune localization with unknown-source English fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-rune-lore.h"

static void title(const char *en, const char *zh)
{
    char out[256];
    assert(strcmp(angdroid_zh_tw_rune_title(en, 1, out, sizeof(out)), zh) == 0);
    assert(strcmp(angdroid_zh_tw_rune_title(en, 0, out, sizeof(out)), en) == 0);
}
static void detail(const char *en, const char *zh)
{
    char out[256];
    assert(strcmp(angdroid_zh_tw_rune_description(en, 1, out, sizeof(out)), zh) == 0);
    assert(strcmp(angdroid_zh_tw_rune_description(en, 0, out, sizeof(out)), en) == 0);
}
int main(void)
{
    char out[128];
    title("enchantment to armor", "護甲強化");
    title("enchantment to hit", "命中強化");
    title("enchantment to damage", "傷害強化");
    title("resist fire", "火焰抗性");
    title("resist Cold", "寒冷抗性");
    title("fire brand", "火焰附魔");
    title("slay orcs", "剋制獸人");
    title("strength", "力量");
    title("speed", "速度");

    detail("Object magically increases the player's armor class",
           "物品以魔法提高玩家的護甲值。");
    detail("Object magically increases the player's chance to hit",
           "物品以魔法提高玩家的命中率。");
    detail("Object magically increases the player's damage",
           "物品以魔法提高玩家的傷害。");
    detail("Object gives the player a magical bonus to strength.",
           "物品讓玩家的力量獲得魔法加成。");
    detail("Object affects the player's resistance to fire.",
           "物品會影響玩家對火焰的抗性。");
    detail("Object brands the player's attacks with cold.",
           "物品可為玩家的攻擊附加寒冷屬性。");
    detail("Object makes the player's attacks against orcs more powerful.",
           "物品會提高玩家對獸人的攻擊威力。");
    detail("Object gives the player the property of speed.",
           "物品賦予玩家速度特性。");

    /* Unknown names and curse descriptions never get guessed at. */
    title("mysterious curse", "mysterious curse");
    title("resist undiscovered element", "resist undiscovered element");
    detail("Object causes permanent hunger.", "Object causes permanent hunger.");
    detail("Object affects the player's resistance to unknown.",
           "Object affects the player's resistance to unknown.");
    assert(angdroid_zh_tw_rune_title(NULL, 1, out, sizeof(out)) == NULL);
    assert(angdroid_zh_tw_rune_description(NULL, 1, out, sizeof(out)) == NULL);
    assert(strcmp(angdroid_zh_tw_rune_title("resist fire", 1, out, 4),
                  "resist fire") == 0);
    assert(strcmp(angdroid_zh_tw_rune_description(
                  "Object affects the player's resistance to fire.",
                  1, out, 4),
                  "Object affects the player's resistance to fire.") == 0);
    puts("PASS: rune titles/lore, locale and unknown fallback, safe buffers");
    return 0;
}
