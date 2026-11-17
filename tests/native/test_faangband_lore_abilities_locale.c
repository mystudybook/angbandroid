/*
 * FAangband source-exact lore abilities: 35 flag rows and 33 unique words.
 * Uses the real headers for both the translation table and clause grammar.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"
#include "app/src/main/cpp/faangband/mon-lore-display.h"

struct translation { const char *en; const char *zh; };
static const struct translation cases[] = {
    {"invisible", "隱形"},
    {"cold blooded", "冷血"},
    {"not detected by telepathy", "無法被心靈感應偵測"},
    {"rarely detected by telepathy", "很難被心靈感應偵測"},
    {"open doors", "開門"},
    {"bash down doors", "撞開門"},
    {"pass through walls", "穿過牆壁"},
    {"bore through walls", "挖穿牆壁"},
    {"smash walls", "擊碎牆壁"},
    {"push past weaker monsters", "推開較弱的怪物"},
    {"destroy weaker monsters", "消滅較弱的怪物"},
    {"pick up objects", "拾取物品"},
    {"destroy objects", "摧毀物品"},
    {"clear webs", "清除蛛網"},
    {"pass through webs", "穿越蛛網"},
    {"bright light", "強光"},
    {"rock remover", "破岩力量"},
    {"fire", "火焰"},
    {"cold", "寒冷"},
    {"acid", "酸液"},
    {"lightning", "閃電"},
    {"poison", "毒素"},
    {"nether", "冥界力量"},
    {"water", "水"},
    {"plasma", "等離子"},
    {"nexus", "空間扭曲"},
    {"disenchantment", "解除魔法"},
    {"frightened", "恐懼"},
    {"stunned", "暈眩"},
    {"confused", "混亂"},
    {"slept", "睡眠"},
    {"held", "束縛"},
    {"slowed", "緩速"},
};
static int checks;
#define CHECK(value) do { if (!(value)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #value); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    char sentence[200];
    const char *unknown = "unrecognized monster resistance phrase";
    for (i = 0; i < sizeof cases / sizeof cases[0]; ++i) {
        CHECK(strcmp(angdroid_game_ui_text(cases[i].en, 1),
            cases[i].zh) == 0);
        CHECK(angdroid_game_ui_text(cases[i].en, 0) == cases[i].en);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);

    CHECK(strcmp(angdroid_fa_lore_separator(0, 3, false), "") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(1, 3, false), "、") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(2, 3, false), "與") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(1, 2, true), "或") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(2, 3, true), "或") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(1, 1, true), "") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(-1, 3, false), "") == 0);
    CHECK(strcmp(angdroid_fa_lore_sentence_end(".  ", true), "。  ") == 0);
    CHECK(strcmp(angdroid_fa_lore_sentence_end(".  ", false), ".  ") == 0);
    CHECK(strcmp(angdroid_fa_lore_sentence_end("", true), "") == 0);

    (void)snprintf(sentence, sizeof sentence, "牠能%s%s%s。",
        angdroid_game_ui_text("open doors", 1),
        angdroid_fa_lore_separator(1, 2, false),
        angdroid_game_ui_text("pass through walls", 1));
    CHECK(strcmp(sentence, "牠能開門與穿過牆壁。") == 0);

    (void)snprintf(sentence, sizeof sentence, "牠容易受到%s%s%s影響。",
        angdroid_game_ui_text("fire", 1),
        angdroid_fa_lore_separator(1, 2, true),
        angdroid_game_ui_text("cold", 1));
    CHECK(strcmp(sentence, "牠容易受到火焰或寒冷影響。") == 0);

    (void)snprintf(sentence, sizeof sentence, "牠不會陷入%s%s%s。",
        angdroid_game_ui_text("frightened", 1),
        angdroid_fa_lore_separator(1, 2, true),
        angdroid_game_ui_text("stunned", 1));
    CHECK(strcmp(sentence, "牠不會陷入恐懼或暈眩。") == 0);

    printf("PASS: %d source-exact FA monster ability locale checks\n",
        checks);
    return 0;
}
