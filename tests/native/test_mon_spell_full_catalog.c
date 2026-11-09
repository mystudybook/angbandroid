/* Test the complete production monster spell translation catalog, not a stub.
 * GCC/Clang C99 standalone against actual PR production include fragments.
 * Do not change Angband's canonical English keys or expand tags here. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-text.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

struct spell_pair { const char *english, *translated; };
static const struct spell_pair original_spells[] = {
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-0.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-1.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-2.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-3.inc"
#include "../../app/src/main/cpp/angband/zh-tw-mon-spell-alt.inc"
};
static const struct spell_pair fa_spells[] = {
    { "{name} breathes ice.", "{name}噴吐冰霜氣息。" },
    { "You hear a tinkling roar.", "你聽見清脆作響的咆哮聲。" },
    { "{name} breathes storm.", "{name}噴吐風暴氣息。" },
    { "You hear a tempestuous roar.", "你聽見暴風般的咆哮聲。" },
    { "{name} breathes dragonfire.", "{name}噴吐龍焰。" },
    { "You hear a violent roar.", "你聽見猛烈的咆哮聲。" },
    { "{name} gestures at you, and you suddenly feel hungry.",
      "{name}向你做了個手勢，你突然感到飢餓。" },
    { "You suddenly feel hungry.", "你突然感到飢餓。" },
    { "You resist the effects!", "你抵抗了這些效果！" },
    { "{name} summons swamp creaturess.", "{name}召喚了沼澤生物。" },
    { "You hear many squishy things appear nearby!", "你聽見附近出現許多濕黏生物的聲響！" },
    { "{name} summons ants.", "{name}召喚了螞蟻。" },
    { "You hear crawling things appear nearby!", "你聽見附近有爬行生物出現！" },
    { "{name} summons winged dragons.", "{name}召喚了有翼巨龍。" },
};
static unsigned checks;
#define CHECK(e) do { if (!(e)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #e); \
    return 1; \
} ++checks; } while (0)

/* Every dynamic placeholder that remains in the translation must be
 * supplied by the English template. Pronouns may be omitted in Chinese. */
static int known_tag(const char *s, size_t n)
{
    static const char *const tags[] = {
        "name", "target", "type", "oftype", "pronoun"
    };
    size_t j;
    for (j = 0; j < sizeof(tags) / sizeof(tags[0]); ++j)
        if (strlen(tags[j]) == n && strncmp(s, tags[j], n) == 0)
            return 1;
    return 0;
}
static int check_tags(const char *english, const char *translated)
{
    const char *p = translated;
    while ((p = strchr(p, '{')) != NULL) {
        const char *end = strchr(p + 1, '}');
        char needle[32];
        size_t n;
        if (!end) return 0;
        n = (size_t)(end - p - 1);
        if (!n || n >= sizeof needle || !known_tag(p + 1, n)) return 0;
        memcpy(needle, p, n + 2);
        needle[n + 2] = '\0';
        if (!strstr(english, needle)) return 0;
        p = end + 1;
    }
    /* Preserve all source expansion tags except possessive {pronoun},
     * which is grammatical but unnecessary in several Chinese sentences. */
    p = english;
    while ((p = strchr(p, '{')) != NULL) {
        const char *end = strchr(p + 1, '}');
        char needle[32];
        size_t n;
        if (!end) return 0;
        n = (size_t)(end - p - 1);
        if (!n || n >= sizeof needle || !known_tag(p + 1, n)) return 0;
        memcpy(needle, p, n + 2);
        needle[n + 2] = '\0';
        if (strcmp(needle, "{pronoun}") != 0 &&
            !strstr(translated, needle)) return 0;
        p = end + 1;
    }
    return 1;
}
static int utf8_valid(const char *text)
{
    size_t offset = 0;
    while (text[offset]) {
        uint32_t scalar;
        size_t count = angdroid_utf8_next_scalar(text + offset, &scalar);
        if (!count || (count == 1 && scalar == 0xFFFDu &&
            (unsigned char)text[offset] >= 0x80u)) return 0;
        offset += count;
    }
    return 1;
}
int main(void)
{
    size_t i, j;
    const size_t count = sizeof original_spells / sizeof original_spells[0];
    const size_t fa_count = sizeof fa_spells / sizeof fa_spells[0];
    CHECK(count == 226);
    CHECK(fa_count == 14);
    for (i = 0; i < count; ++i) {
        const char *en = original_spells[i].english;
        const char *zh = original_spells[i].translated;
        CHECK(en && *en && zh && *zh);
        CHECK(strcmp(angdroid_mon_spell_text(en, 1), zh) == 0);
        CHECK(angdroid_mon_spell_text(en, 0) == en);
        CHECK(utf8_valid(zh));
        CHECK(check_tags(en, zh));
        for (j = 0; j < i; ++j)
            CHECK(strcmp(en, original_spells[j].english) != 0);
    }
    for (i = 0; i < fa_count; ++i) {
        const char *en = fa_spells[i].english;
        const char *zh = fa_spells[i].translated;
        CHECK(strcmp(angdroid_mon_spell_text(en, 1), zh) == 0);
        CHECK(angdroid_mon_spell_text(en, 0) == en);
        CHECK(utf8_valid(zh));
        CHECK(check_tags(en, zh));
        for (j = 0; j < count; ++j)
            CHECK(strcmp(en, original_spells[j].english) != 0);
    }
    CHECK(angdroid_mon_spell_text(NULL, 1) == NULL);
    CHECK(strcmp(angdroid_mon_spell_text("unknown spell", 1),
                 "unknown spell") == 0);
    CHECK(strcmp(angdroid_mon_spell_text("{name} casts a missing spell.", 1),
                 "{name} casts a missing spell.") == 0);
    printf("PASS: 226 Angband + 14 FAangband spell templates, %u assertions\n",
           checks);
    return 0;
}
