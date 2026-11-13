/*
 * FAangband monster-race alternate spell notifications (batch 250).
 * Test the production source-exact alternate table and UTF-8 subject helpers.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/mon-spell-message-locale.h"

struct translation { const char *en; const char *zh; };
static const struct translation alternates[] = {
    {"Something slurps.", "某處傳來啜飲聲。"},
    {"{name} caws three times.", "{name}連續啼叫了三聲。"},
    {"Something caws three times.", "某處傳來連續三聲啼叫。"},
    {"Something hums softly.", "某處傳來輕微的嗡嗡聲。"},
    {"Something yips.", "某處傳來尖細的吠叫聲。"},
    {"Something rumbles.", "某處傳來低沉的隆隆聲。"},
    {"Something chitters.", "某處傳來吱吱聲。"},
    {"Something howls.", "某處傳來嚎叫聲。"},
    {"Something hisses.", "某處傳來嘶嘶聲。"},
    {"The baleful presence of {name} inspires your enemies.", "{name}散發的不祥氣息鼓舞了敵人。"},
    {"Something drains your mana away.", "某種力量正在吸走你的魔力。"},
    {"Your muscles lock up for no apparent reason.", "你的肌肉莫名其妙地僵住了。"},
    {"Something inspires your enemies.", "某種力量鼓舞了敵人。"},
    {"The constant crackling subsides briefly and then resumes.", "持續的劈啪聲短暫停止，隨後又再次響起。"},
    {"{name} crackles softly.", "{name}發出輕微的劈啪聲。"},
    {"Something crackles softly.", "某處傳來輕微的劈啪聲。"},
    {"Something clatters.", "某處傳來喀啦喀啦的聲響。"},
};

static int checked;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAILED line %d: %s\n", __LINE__, #x); \
    return 1; } ++checked; } while (0)

int main(void)
{
    size_t i;
    char subject[64];
    char tiny[16];
    const char *unknown = "A rare monster speaks in a strange voice.";
    const char *name = "The 龍";

    CHECK(sizeof alternates / sizeof alternates[0] == 17);
    for (i = 0; i < sizeof alternates / sizeof alternates[0]; ++i) {
        CHECK(strcmp(angdroid_fa_spell_message_locale(alternates[i].en,
            true), alternates[i].zh) == 0);
        CHECK(angdroid_fa_spell_message_locale(alternates[i].en,
            false) == alternates[i].en);
    }
    CHECK(angdroid_fa_spell_message_locale(unknown, true) == unknown);
    CHECK(angdroid_fa_spell_message_locale(NULL, true) == NULL);
    CHECK(strcmp(angdroid_fa_spell_message_locale(
        "Something slurps.", true), "某處傳來啜飲聲。") == 0);
    CHECK(strcmp(angdroid_fa_spell_message_locale(
        "{name} caws three times.", true),
        "{name}連續啼叫了三聲。") == 0);

    CHECK(strcmp(angdroid_fa_spell_subject(name, true,
        subject, sizeof subject), "這隻龍") == 0);
    CHECK(strcmp(angdroid_fa_spell_subject(name, true,
        tiny, 10), "這隻龍") == 0);
    CHECK(strcmp(angdroid_fa_spell_subject(name, true,
        tiny, 9), "這隻") == 0);
    CHECK(strcmp(angdroid_fa_spell_subject(name, true,
        tiny, 4), "這") == 0);
    CHECK(strcmp(angdroid_fa_spell_subject(name, true,
        tiny, 3), "") == 0);
    CHECK(angdroid_fa_spell_subject(name, false,
        tiny, sizeof tiny) == name);
    CHECK(angdroid_fa_spell_subject("Wormtongue", true,
        tiny, sizeof tiny) != tiny);
    CHECK(strcmp(angdroid_fa_spell_subject("it", true,
        tiny, sizeof tiny), "牠") == 0);
    CHECK(strcmp(angdroid_fa_spell_subject("something", true,
        tiny, sizeof tiny), "某個東西") == 0);
    CHECK(angdroid_fa_spell_subject(NULL, true, tiny,
        sizeof tiny) == NULL);
    CHECK(angdroid_fa_spell_subject(name, true, tiny, 0) == name);
    CHECK(angdroid_utf8_copy_complete(tiny, sizeof tiny,
        "火焰閃電魔法") == 15);
    CHECK(strcmp(tiny, "火焰閃電魔法") == 0);
    CHECK(angdroid_utf8_copy_complete(tiny, 8,
        "火焰閃電魔法") == 6);
    CHECK(strcmp(tiny, "火焰") == 0);
    CHECK(angdroid_utf8_copy_complete(tiny, 4, "冰霜") == 3);
    CHECK(strcmp(tiny, "冰") == 0);

    /* Simulate strnfcat truncating immediately after a Han lead byte.
     * The sanitized message must never pass that broken UTF-8 to msgt(). */
    {
        const char torn[] = "\xE7\x81\xAB\xE9";
        CHECK(angdroid_utf8_copy_complete(tiny, sizeof tiny, torn) == 4);
        CHECK(strcmp(tiny, "火?") == 0);
    }

    printf("PASS: %d FA race-specific spell and UTF-8 checks\n", checked);
    return 0;
}
