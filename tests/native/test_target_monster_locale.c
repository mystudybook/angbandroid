/* Display-only targeting names: invisible monsters may never leak identity.
 * Compile this standalone against production headers with C99 strict warnings. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-target-monster.h"
static unsigned checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)
static void name_case(const char *english, const char *race, int unique,
                      const char *chinese, int expect_match)
{
    char result[192];
    int translated = angdroid_target_monster_display(result, sizeof(result),
        english, race, unique, 1);
    CHECK(translated == expect_match);
    CHECK(strcmp(result, chinese) == 0);
    CHECK(angdroid_target_monster_display(result, sizeof(result), english,
        race, unique, 0) == 0);
    CHECK(strcmp(result, english) == 0);
}
int main(void)
{
    static const struct {const char *en, *zh;} statuses[] = {
        { "unhurt", "未受傷" },
        { "undamaged", "沒有損傷" },
        { "somewhat wounded", "輕微受傷" },
        { "somewhat damaged", "輕微受損" },
        { "wounded", "受傷" },
        { "damaged", "受損" },
        { "badly wounded", "身受重傷" },
        { "badly damaged", "嚴重受損" },
        { "almost dead", "瀕死" },
        { "almost destroyed", "幾近毀滅" },
        { "asleep", "睡眠中" },
        { "held", "遭束縛" },
        { "disenchanted", "遭消魔" },
        { "confused", "混亂" },
        { "afraid", "恐懼" },
        { "stunned", "昏迷" },
        { "slowed", "緩速" },
        { "hasted", "加速" },
    };
    char result[192];
    char small[3];
    size_t i;
    CHECK(sizeof(statuses) / sizeof(statuses[0]) == 18);
    /* FAangband-only name, original Angband name and unique names. */
    name_case("a blue dragon", "blue dragon", 0, "藍龍", 1);
    name_case("an apparition of hunger", "apparition of hunger", 0,
              "飢餓幽影", 1);
    name_case("The blue dragon", "blue dragon", 0, "藍龍", 1);
    name_case("the blue dragon (offscreen)", "blue dragon", 0,
              "藍龍（畫面外）", 1);
    name_case("The blue dragon, (offscreen)", "blue dragon", 0,
              "藍龍，（畫面外）", 1);
    name_case("Mughâsh the Kobold Lord", "Mughâsh the Kobold Lord", 1,
              "狗頭人領主穆加什", 1);
    name_case("Sauron, the Sorcerer", "Sauron, the Sorcerer", 1,
              "索倫，黑暗巫師", 1);
    name_case("a scrawny cat", "scrawny cat", 0, "瘦弱的貓", 1);
    /* Do not reveal telepathically sensed/hidden monsters or convert pronouns. */
    name_case("it", "blue dragon", 0, "it", 0);
    name_case("someone", "blue dragon", 0, "someone", 0);
    name_case("something", "blue dragon", 0, "something", 0);
    name_case("a blue dragon warrior", "blue dragon", 0,
              "a blue dragon warrior", 0);
    name_case("a unknown beast", "unknown beast", 0, "a unknown beast", 0);
    CHECK(angdroid_target_monster_display(result, sizeof(result), NULL,
        "blue dragon", 0, 1) == 0 && result[0] == '\0');
    CHECK(angdroid_target_monster_display(NULL, 0, "a blue dragon",
        "blue dragon", 0, 1) == 0);
    CHECK(angdroid_target_monster_display(small, sizeof(small),
        "a blue dragon", "blue dragon", 0, 1) == 1);
    CHECK(small[0] == '\0'); /* Too small for one complete Han character. */
    for (i = 0; i < sizeof(statuses) / sizeof(statuses[0]); ++i) {
        angdroid_target_monster_status(result, sizeof(result), statuses[i].en, 1);
        CHECK(strcmp(result, statuses[i].zh) == 0);
        angdroid_target_monster_status(result, sizeof(result), statuses[i].en, 0);
        CHECK(strcmp(result, statuses[i].en) == 0);
    }
    angdroid_target_monster_status(result, sizeof(result),
        "unhurt, asleep, confused", 1);
    CHECK(strcmp(result, "未受傷、睡眠中、混亂") == 0);
    angdroid_target_monster_status(result, sizeof(result),
        "wounded, unknown state", 1);
    CHECK(strcmp(result, "受傷、unknown state") == 0);
    angdroid_target_monster_status(result, sizeof(result), NULL, 1);
    CHECK(result[0] == '\0');
    CHECK(strcmp(angdroid_target_monster_intro("You see ", 1), "你看到") == 0);
    CHECK(strcmp(angdroid_target_monster_intro("You sense ", 1), "你感應到") == 0);
    CHECK(strcmp(angdroid_target_monster_intro("You recall ", 1), "你回想起") == 0);
    CHECK(strcmp(angdroid_target_monster_intro("You see ", 0), "You see ") == 0);
    CHECK(strcmp(angdroid_target_monster_intro("unknown", 1), "unknown") == 0);
    CHECK(angdroid_target_monster_intro(NULL, 1) == NULL);
    printf("PASS: safe monster target lookup, %u assertions\n", checks);
    return 0;
}
