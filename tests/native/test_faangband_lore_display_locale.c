/*
 * FAangband monster lore: exact 11 awareness, 9 speed and 10 race labels.
 * Uses the live production zh-tw-ui.h dictionary, not copied function code.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct translation_case { const char *en; const char *zh; };
static const struct translation_case translations[] = {
    {"prefers to ignore", "傾向忽視"},
    {"pays very little attention to", "幾乎不注意"},
    {"pays little attention to", "不太注意"},
    {"tends to overlook", "容易忽略"},
    {"takes quite a while to see", "很久才會發現"},
    {"takes a while to see", "需要一些時間才發現"},
    {"is fairly observant of", "相當留意"},
    {"is observant of", "會留意"},
    {"is very observant of", "十分留意"},
    {"is vigilant for", "警戒著"},
    {"is ever vigilant for", "始終警戒著"},
    {"incredibly quickly", "極為迅速"},
    {"very quickly", "非常迅速"},
    {"quickly", "迅速"},
    {"fairly quickly", "頗為迅速"},
    {"normal speed", "一般速度"},
    {"slowly", "緩慢"},
    {"very slowly", "非常緩慢"},
    {"incredibly slowly", "極為緩慢"},
    {"erroneously", "異常地"},
    {"orc", "獸人"},
    {"troll", "巨魔"},
    {"giant", "巨人"},
    {"dragon", "龍"},
    {"demon", "惡魔"},
    {"natural", "自然的"},
    {"evil", "邪惡的"},
    {"undead", "不死的"},
    {"nonliving", "非生物的"},
    {"metal", "金屬製的"},
};

static unsigned checks;
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #condition); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    char sentence[256];
    const char *unknown = "unknown FAangband lore adjective";
    for (i = 0; i < sizeof translations / sizeof translations[0]; ++i) {
        CHECK(strcmp(angdroid_game_ui_text(translations[i].en, 1),
            translations[i].zh) == 0);
        CHECK(angdroid_game_ui_text(translations[i].en, 0) ==
            translations[i].en);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);

    (void)snprintf(sentence, sizeof sentence, "這隻%s%s通常出沒於危險等級 %d。",
        angdroid_game_ui_text("evil", 1),
        angdroid_game_ui_text("orc", 1), 12);
    CHECK(strcmp(sentence, "這隻邪惡的獸人通常出沒於危險等級 12。") == 0);

    (void)snprintf(sentence, sizeof sentence, "牠%s入侵者，可能在 %d 英尺外察覺動靜。",
        angdroid_game_ui_text("is vigilant for", 1), 40);
    CHECK(strcmp(sentence, "牠警戒著入侵者，可能在 40 英尺外察覺動靜。") == 0);

    (void)snprintf(sentence, sizeof sentence, "移動得%s",
        angdroid_game_ui_text("very quickly", 1));
    CHECK(strcmp(sentence, "移動得非常迅速") == 0);

    printf("PASS: %u FA monster lore display locale checks\n", checks);
    return 0;
}
