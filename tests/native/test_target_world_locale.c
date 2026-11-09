/* Native C99 regression for look/target terrain, traps, item pile phrases. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-target-world.h"
static unsigned checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)
static void known(const char *en, const char *expected,
    const char *(*fn)(const char *, int))
{
    CHECK(strcmp(fn(en, 1), expected) == 0);
    CHECK(fn(en, 0) == en);
}
int main(void)
{
    const char *unknown = "unrecognized feature";
    known("open floor", "開闊地面", angdroid_target_world_terrain);
    known("granite wall", "花崗岩牆", angdroid_target_world_terrain);
    known("unknown grid", "未知地形", angdroid_target_world_terrain);
    known("lava", "熔岩", angdroid_target_world_terrain);
    known("water", "水域", angdroid_target_world_terrain);
    known("lowland trees", "低地樹林", angdroid_target_world_terrain);
    known("easy path north", "北方平緩小徑", angdroid_target_world_terrain);
    known("empty space", "虛空", angdroid_target_world_terrain);
    known("pit", "陷坑", angdroid_target_world_trap_name);
    known("falling branch", "落枝陷阱", angdroid_target_world_trap_name);
    known("stasis trap", "停滯陷阱", angdroid_target_world_trap_name);
    known("A hole dug to snare the unwary.",
        "挖來困住疏忽者的陷坑。", angdroid_target_world_trap_desc);
    known("A falling branch could be random, or an act of an enemy, animal or vegetable.",
        "可能是意外掉落，也可能是敵人、動物或植物造成的落枝。",
        angdroid_target_world_trap_desc);
    known("You are ", "你站在", angdroid_target_world_intro);
    known("You see ", "你看到", angdroid_target_world_intro);
    known("You sense ", "你感應到", angdroid_target_world_intro);
    known("You recall ", "你記得", angdroid_target_world_intro);
    known("You are ", "你所在的位置有", angdroid_target_world_pile_intro);
    known("You see ", "你看到", angdroid_target_world_pile_intro);
    known("on ", "", angdroid_target_world_link);
    CHECK(angdroid_target_world_terrain(unknown, 1) == unknown);
    CHECK(angdroid_target_world_trap_name(unknown, 1) == unknown);
    CHECK(angdroid_target_world_trap_desc(unknown, 1) == unknown);
    CHECK(angdroid_target_world_intro(unknown, 1) == unknown);
    CHECK(angdroid_target_world_pile_intro(unknown, 1) == unknown);
    CHECK(angdroid_target_world_link(unknown, 1) == unknown);
    CHECK(angdroid_target_world_terrain(NULL, 1) == NULL);
    CHECK(angdroid_target_world_trap_name(NULL, 1) == NULL);
    CHECK(angdroid_target_world_trap_desc(NULL, 1) == NULL);
    CHECK(angdroid_target_world_intro(NULL, 1) == NULL);
    CHECK(angdroid_target_world_pile_intro(NULL, 1) == NULL);
    CHECK(angdroid_target_world_link(NULL, 1) == NULL);
    CHECK(strcmp(angdroid_target_world_hallucination(1),
        "某種奇怪的東西") == 0);
    CHECK(strcmp(angdroid_target_world_hallucination(0),
        "something strange") == 0);
    /* Simulate byte-sized strnfmt truncation midway through a Han character. */
    {
        char valid[32] = "繁體中文";
        char broken_head[] = { (char)0xE7, (char)0xB9, '\0' };
        char broken_after[] = { 'A', (char)0xE4, (char)0xB8, '\0' };
        char malformed[] = { 'A', (char)0xFF, 'B', '\0' };
        char literal_replacement[] = "\xEF\xBF\xBD";
        char english_mode[] = { 'A', (char)0xE4, (char)0xB8, '\0' };
        char one_byte[] = { '\0' };
        angdroid_target_world_utf8_trim(valid, sizeof valid, 1);
        CHECK(strcmp(valid, "繁體中文") == 0);
        angdroid_target_world_utf8_trim(broken_head, sizeof broken_head, 1);
        CHECK(broken_head[0] == '\0');
        angdroid_target_world_utf8_trim(broken_after, sizeof broken_after, 1);
        CHECK(strcmp(broken_after, "A") == 0);
        angdroid_target_world_utf8_trim(malformed, sizeof malformed, 1);
        CHECK(strcmp(malformed, "A") == 0);
        angdroid_target_world_utf8_trim(literal_replacement,
                                        sizeof literal_replacement, 1);
        CHECK(strcmp(literal_replacement, "\xEF\xBF\xBD") == 0);
        angdroid_target_world_utf8_trim(english_mode, sizeof english_mode, 0);
        CHECK(english_mode[1] == (char)0xE4);
        angdroid_target_world_utf8_trim(one_byte, sizeof one_byte, 1);
        CHECK(one_byte[0] == '\0');
        angdroid_target_world_utf8_trim(NULL, 0, 1);
    }
    puts("PASS: target-world terrain, visible traps, hallucination, pile prompts");
    return 0;
}
