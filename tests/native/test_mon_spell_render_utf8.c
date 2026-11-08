/* Monster spell template display-only: strict tags and UTF-8 limits. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-render.h"
static int checks;
#define CHECK(expr) do { assert(expr); ++checks; } while (0)
int main(void)
{
    static const char *tags[] = {
        "name", "pronoun", "target", "type", "oftype"
    };
    size_t i, used;
    char out[10];
    char invalid[] = { (char)0xE4, 'X', '\0' };
    char tail[] = { (char)0xE4, (char)0xB8, '\0' };
    for (i = 0; i < sizeof(tags) / sizeof(tags[0]); ++i) {
        CHECK(angdroid_mon_spell_tag_matches(
            tags[i], strlen(tags[i]), tags[i]));
        CHECK(!angdroid_mon_spell_tag_matches(
            tags[i], strlen(tags[i]) - 1, tags[i]));
    }
    CHECK(!angdroid_mon_spell_tag_matches("named", 5, "name"));
    CHECK(!angdroid_mon_spell_tag_matches("pronouns", 8, "pronoun"));
    CHECK(!angdroid_mon_spell_tag_matches("targeted", 8, "target"));
    CHECK(!angdroid_mon_spell_tag_matches("typed", 5, "type"));
    CHECK(!angdroid_mon_spell_tag_matches("oftypes", 7, "oftype"));
    CHECK(angdroid_mon_spell_tag_matches("name} tail", 4, "name"));
    CHECK(!angdroid_mon_spell_tag_matches(NULL, 4, "name"));
    CHECK(!angdroid_mon_spell_tag_matches("name", 4, NULL));
    used = angdroid_mon_spell_append_utf8(
        out, sizeof(out), 0, "龍龍龍龍", strlen("龍龍龍龍"));
    CHECK(used == strlen("龍龍龍"));
    CHECK(strcmp(out, "龍龍龍") == 0);
    used = angdroid_mon_spell_append_utf8(
        out, sizeof(out), used, "。", strlen("。"));
    CHECK(used == strlen("龍龍龍"));
    CHECK(strcmp(out, "龍龍龍") == 0);
    used = angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, "怪物{name}攻擊", strlen("怪物"));
    CHECK(strcmp(out, "怪物") == 0);
    CHECK(used == strlen("怪物"));
    used = angdroid_mon_spell_append_utf8(
        out, sizeof(out), used, "!", 1);
    CHECK(strcmp(out, "怪物!") == 0);
    used = angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, invalid, strlen(invalid));
    CHECK(used == 2);
    CHECK(strcmp(out, "?X") == 0);
    /* A genuinely broken UTF-8 source is sanitized to question marks. */
    used = angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, tail, strlen(tail));
    CHECK(used == 2);
    CHECK(strcmp(out, "??") == 0);
    /* A valid Han glyph whose requested slice is only 2 bytes is dropped. */
    used = angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, "中文", 2);
    CHECK(used == 0);
    CHECK(strcmp(out, "") == 0);
    CHECK(angdroid_mon_spell_append_utf8(NULL, 0, 0, "中", 3) == 0);
    CHECK(angdroid_mon_spell_append_utf8(out, 0, 0, "中", 3) == 0);
    CHECK(angdroid_mon_spell_append_utf8(out, sizeof(out),
        sizeof(out), "中", 3) == sizeof(out));
    CHECK(angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, NULL, 3) == 0);
    CHECK(angdroid_mon_spell_append_utf8(out, sizeof(out),
        0, "中文", 0) == 0);
    printf("PASS: monster spell exact tags and UTF-8 rendering (%d assertions)\n",
        checks);
    return 0;
}
