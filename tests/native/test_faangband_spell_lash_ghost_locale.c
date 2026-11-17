/*
 * FAangband batch 251: actual projection.txt lash-desc values and special
 * monster/player-ghost subject grammar. Compile the production headers.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/mon-spell-message-locale.h"
#include "app/src/main/cpp/angband/zh-tw-mon-spell-lash.h"

struct translation { const char *en; const char *zh; };
static const struct translation source_lash[] = {
    {"acid", "酸液"},
    {"lightning", "閃電"},
    {"fire", "火焰"},
    {"frost", "冰霜"},
    {"venom", "毒液"},
    {"brightness", "耀眼光芒"},
    {"blackness", "黑暗"},
    {"sound", "聲波"},
    {"razors", "利刃"},
    {"dislocation", "空間錯位"},
    {"withering", "枯萎之力"},
    {"confusion", "混亂"},
    {"unmagic", "破除魔法"},
    {"water", "水流"},
    {"icicles", "冰錐"},
    {"gravity", "重力"},
    {"lassitude", "遲滯之力"},
    {"impact", "強烈衝擊"},
    {"ruination", "毀滅之力"},
    {"living fire", "活生生的火焰"},
    {"dragonfire", "龍焰"},
    {"hellfire", "地獄火"},
    {"meteoric iron", "隕鐵"},
    {"raw magic", "純粹魔力"},
    {"holy power", "神聖力量"},
};

static unsigned checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    char out[200];
    char small[32];
    const char *unknown = "unspecified lash magic";
    const char *ghost = "艾莉絲";
    const char *race = "幽靈戰士";
    const char *named_ghost = "艾莉絲, the Brave";

    CHECK(sizeof source_lash / sizeof source_lash[0] == 25);
    for (i = 0; i < sizeof source_lash / sizeof source_lash[0]; ++i) {
        CHECK(strcmp(angdroid_mon_spell_lash(source_lash[i].en, 1),
            source_lash[i].zh) == 0);
        CHECK(angdroid_mon_spell_lash(source_lash[i].en, 0) ==
            source_lash[i].en);
        CHECK(strlen(source_lash[i].zh) > 0);
    }
    CHECK(angdroid_mon_spell_lash(unknown, 1) == unknown);
    CHECK(angdroid_mon_spell_lash(NULL, 1) == NULL);

    CHECK(strcmp(angdroid_mon_spell_lash("venom", 1), "毒液") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("brightness", 1), "耀眼光芒") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("dislocation", 1), "空間錯位") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("withering", 1), "枯萎之力") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("hellfire", 1), "地獄火") == 0);
    CHECK(strcmp(angdroid_mon_spell_lash("holy power", 1), "神聖力量") == 0);

    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, true,
        out, sizeof out), "艾莉絲（幽靈戰士）") == 0);
    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士 (offscreen)", ghost, race, true,
        out, sizeof out), "艾莉絲（幽靈戰士）（畫面外）") == 0);
    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the Brave, the 幽靈戰士", named_ghost, race,
        true, out, sizeof out), "艾莉絲, the Brave（幽靈戰士）") == 0);

    CHECK(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, false,
        out, sizeof out) != out);
    CHECK(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 巨魔", ghost, race, true,
        out, sizeof out) != out);
    CHECK(angdroid_fa_spell_ghost_subject(
        "艾莉絲, another 幽靈戰士", ghost, race, true,
        out, sizeof out) != out);
    CHECK(angdroid_fa_spell_ghost_subject(
        NULL, ghost, race, true, out, sizeof out) == NULL);
    CHECK(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, true,
        out, 0) != out);

    /* The ghost name and suffix are UTF-8-safe at every small capacity. */
    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, true,
        small, 4), "艾") == 0);
    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, true,
        small, 10), "艾莉絲") == 0);
    CHECK(strcmp(angdroid_fa_spell_ghost_subject(
        "艾莉絲, the 幽靈戰士", ghost, race, true,
        small, 13), "艾莉絲") == 0);

    /* monster_desc() marks off-panel names in English; translated
     * spell notifications must preserve the positional fact in Chinese. */
    CHECK(strcmp(angdroid_fa_spell_offscreen_subject(
        "這隻獸人 (offscreen)", true, out, sizeof out),
        "這隻獸人（畫面外）") == 0);
    CHECK(strcmp(angdroid_fa_spell_offscreen_subject(
        "Wormtongue, Agent of Saruman (offscreen)", true,
        out, sizeof out),
        "Wormtongue, Agent of Saruman（畫面外）") == 0);
    CHECK(strcmp(angdroid_fa_spell_offscreen_subject(
        "艾莉絲（幽靈戰士）（畫面外）", true, out, sizeof out),
        "艾莉絲（幽靈戰士）（畫面外）") == 0);
    {
        const char *unchanged = "The orc (offscreen)";
        CHECK(angdroid_fa_spell_offscreen_subject(
            unchanged, false, out, sizeof out) == unchanged);
        CHECK(angdroid_fa_spell_offscreen_subject(
            "The orc", true, out, sizeof out) != out);
        CHECK(angdroid_fa_spell_offscreen_subject(
            NULL, true, out, sizeof out) == NULL);
        CHECK(angdroid_fa_spell_offscreen_subject(
            unchanged, true, out, 0) == unchanged);
        CHECK(angdroid_fa_spell_offscreen_subject(
            unchanged, true, small, 16) == unchanged);
    }
    /* At narrow byte budgets, shorten the name, not a CJK suffix. */
    CHECK(strcmp(angdroid_fa_spell_offscreen_subject(
        "獸龍 (offscreen)", true, small, 20), "獸（畫面外）") == 0);
    CHECK(strcmp(angdroid_fa_spell_offscreen_subject(
        "獸龍 (offscreen)", true, small, 25), "獸龍（畫面外）") == 0);

    CHECK(strcmp(angdroid_fa_spell_subject(
        "the orc", true, out, sizeof out), "這隻orc") == 0);
    CHECK(angdroid_fa_spell_subject(
        "Wormtongue, Agent of Saruman", true, out, sizeof out)
        != out);
    printf("PASS: %u FA lash and ghost display checks\n", checks);
    return 0;
}
