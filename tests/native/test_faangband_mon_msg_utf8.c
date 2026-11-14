/*
 * Batch 258 — test real FAangband group-message subject code in strict C99.
 * Deliberately independent of Android SDK and game state.
 */
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/mon-msg-utf8.h"

static unsigned checks;
#define CHECK(x) do { if (!(x)) { \
    fprintf(stderr, "FAILED at line %d: %s\n", __LINE__, #x); \
    return 1; } ++checks; } while (0)

static bool valid_utf8(const char *text)
{
    size_t i = 0, n;
    uint32_t cp;
    while (text[i]) {
        n = angdroid_utf8_next_scalar(text + i, &cp);
        if (!n || (n == 1 && cp == 0xFFFDu &&
                (unsigned char)text[i] >= 0x80u)) return false;
        i += n;
    }
    return true;
}

static bool ends_with(const char *s, const char *tail)
{
    size_t a = strlen(s), b = strlen(tail);
    return a >= b && strcmp(s + a - b, tail) == 0;
}

int main(void)
{
    char subject[256];
    char narrow[32];
    char one[1];
    const char *offscreen = "（畫面外）";

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", "獸人們", NULL, NULL, 1,
        false, false, false, false);
    CHECK(strcmp(subject, "這隻獸人") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", "獸人們", NULL, NULL, 3,
        false, false, false, false);
    CHECK(strcmp(subject, "3 隻獸人們") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 2,
        false, false, false, false);
    CHECK(strcmp(subject, "2 隻獸人") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, "半精靈", NULL, 1,
        false, false, false, false);
    CHECK(strcmp(subject, "這隻半精靈 獸人") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "Morgoth", NULL, NULL, NULL, 1,
        false, false, true, false);
    CHECK(strcmp(subject, "Morgoth") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "幽靈戰士", NULL, NULL, "艾莉絲", 1,
        false, false, false, false);
    CHECK(strcmp(subject, "艾莉絲（幽靈戰士）") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "幽靈戰士", NULL, NULL, "艾莉絲", 1,
        false, true, false, false);
    CHECK(strcmp(subject, "艾莉絲（幽靈戰士）（畫面外）") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 1,
        true, false, false, false);
    CHECK(strcmp(subject, "牠") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 4,
        true, false, false, false);
    CHECK(strcmp(subject, "4 隻怪物") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 1,
        true, true, false, false);
    CHECK(strcmp(subject, "牠（畫面外）") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 1,
        false, false, false, true);
    CHECK(strcmp(subject, "這隻獸人，") == 0);

    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, NULL, NULL, 1,
        false, true, false, true);
    CHECK(strcmp(subject, "這隻獸人（畫面外），") == 0);

    /* In translated Chinese grammar the offscreen location is part of
     * the noun phrase, and must precede its RF_NAME_COMMA punctuation. */
    angdroid_fa_mon_msg_chinese_subject(narrow, sizeof narrow,
        "獸人", NULL, "半精靈", NULL, 1,
        false, true, false, true);
    CHECK(strcmp(narrow, "這隻獸人（畫面外），") == 0);
    CHECK(valid_utf8(narrow));

    /* Long player-race qualifiers must not crowd the actual monster
     * species out of a narrow display buffer. */
    angdroid_fa_mon_msg_chinese_subject(narrow, sizeof narrow,
        "獸人", NULL, "非常非常漫長的玩家種族前綴", NULL, 1,
        false, false, false, false);
    CHECK(strcmp(narrow, "這隻獸人") == 0);
    CHECK(valid_utf8(narrow));

    angdroid_fa_mon_msg_chinese_subject(narrow, sizeof narrow,
        "獸人", NULL, "半精靈", NULL, 1,
        false, true, false, false);
    CHECK(strcmp(narrow, "這隻獸人（畫面外）") == 0);

    /* With enough space, preserve both the qualifier and the species. */
    angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
        "獸人", NULL, "半精靈", NULL, 1,
        false, true, false, true);
    CHECK(strcmp(subject, "這隻半精靈 獸人（畫面外），") == 0);
    CHECK(valid_utf8(subject));

    /* Keep the complete offscreen marker when the race name is huge.
     * Prefix truncation must not introduce any partial Han glyph. */
    angdroid_fa_mon_msg_chinese_subject(narrow, sizeof narrow,
        "超級無敵漫長的獸人種族名稱", NULL, NULL, NULL, 1,
        false, true, false, false);
    CHECK(ends_with(narrow, offscreen));
    CHECK(valid_utf8(narrow));
    CHECK(strlen(narrow) < sizeof narrow);

    angdroid_fa_mon_msg_chinese_subject(narrow, 16,
        "幽靈戰士", NULL, NULL, "艾莉絲", 1,
        false, false, false, false);
    CHECK(strcmp(narrow, "艾莉絲") == 0);
    CHECK(valid_utf8(narrow));

    angdroid_fa_mon_msg_chinese_subject(narrow, 7,
        "獸人", NULL, NULL, NULL, 1,
        false, false, false, false);
    CHECK(strcmp(narrow, "獸人") == 0);
    angdroid_fa_mon_msg_chinese_subject(narrow, 4,
        "獸人", NULL, NULL, NULL, 1,
        false, false, false, false);
    CHECK(strcmp(narrow, "牠") == 0);
    angdroid_fa_mon_msg_chinese_subject(one, sizeof one,
        "獸人", NULL, NULL, NULL, 1,
        false, false, false, false);
    CHECK(one[0] == '\0');

    /* Batch 260: an offscreen marker or comma without any subject is
     * misleading. At EVERY capacity, the visible output must contain a
     * complete useful noun (or be empty when not even "牠" fits). */
    for (size_t cap = 1; cap <= sizeof narrow; ++cap) {
        const char *const race = "超級無敵漫長的獸人種族名稱";
        const char *const ghost = "艾莉絲";
        const char *const unique_name = "Morgoth, Lord of Darkness";
        int mode;
        for (mode = 0; mode < 6; ++mode) {
            bool invisible = mode == 1 || mode == 2;
            bool unique = mode == 4;
            int count = mode == 2 ? INT_MAX : (mode == 3 ? 12 : 1);
            angdroid_fa_mon_msg_chinese_subject(narrow, cap,
                mode == 4 ? unique_name : race, "獸人們",
                mode == 3 ? "極長的玩家種族修飾詞" : NULL,
                mode == 5 ? ghost : NULL, count, invisible,
                true, unique, true);
            CHECK(strlen(narrow) < cap);
            CHECK(valid_utf8(narrow));
            CHECK(strcmp(narrow, offscreen) != 0);
            CHECK(strcmp(narrow, "（畫面外），") != 0);
            CHECK(strcmp(narrow, "，") != 0);
            if (strstr(narrow, "，") != NULL) {
                CHECK(ends_with(narrow, "，"));
                if (strstr(narrow, offscreen) != NULL)
                    CHECK(ends_with(narrow, "（畫面外），"));
            }
            if (strstr(narrow, "（") != NULL)
                CHECK(strstr(narrow, "）") != NULL);
            if (cap >= 4)
                CHECK(narrow[0] != '\0');
        }
    }

    angdroid_fa_mon_msg_chinese_subject(narrow, 17,
        "獸人", NULL, NULL, NULL, 1,
        false, true, false, true);
    CHECK(strcmp(narrow, "這隻獸人，") == 0);
    angdroid_fa_mon_msg_chinese_subject(narrow, 19,
        "獸人", NULL, NULL, NULL, 1,
        true, true, false, false);
    CHECK(strcmp(narrow, "牠（畫面外）") == 0);
    angdroid_fa_mon_msg_chinese_subject(narrow, 22,
        "獸人", NULL, NULL, NULL, 1,
        false, true, true, true);
    CHECK(strcmp(narrow, "牠（畫面外），") == 0);
    angdroid_fa_mon_msg_chinese_subject(narrow, 25,
        "獸人", NULL, NULL, NULL, 1,
        false, true, false, true);
    CHECK(strcmp(narrow, "獸人（畫面外），") == 0);
    angdroid_fa_mon_msg_chinese_subject(narrow, 31,
        "獸人", NULL, NULL, NULL, 1,
        false, true, false, true);
    CHECK(strcmp(narrow, "這隻獸人（畫面外），") == 0);

    /* Batch 261: malformed names must fall back to a complete known
     * subject rather than displaying a broken or misleading proper name. */
    {
        static const char malformed_race[] = "獸\xE4\xB8";
        static const char malformed_ghost[] = "\xC0\xAF";
        static const char malformed_plural[] = "\xF0\x90\x80";
        static const char malformed_qualifier[] = "半\xFF精靈";
        const char *valid_replacement = "受損\xEF\xBF\xBD怪物";

        CHECK(!angdroid_fa_mon_msg_valid_utf8(malformed_race));
        CHECK(!angdroid_fa_mon_msg_valid_utf8(malformed_ghost));
        CHECK(!angdroid_fa_mon_msg_valid_utf8(malformed_plural));
        CHECK(!angdroid_fa_mon_msg_valid_utf8(malformed_qualifier));
        CHECK(angdroid_fa_mon_msg_valid_utf8(valid_replacement));
        CHECK(!angdroid_fa_mon_msg_valid_utf8(NULL));

        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            malformed_race, NULL, NULL, NULL, 1,
            false, false, false, false);
        CHECK(strcmp(subject, "這隻怪物") == 0);
        CHECK(valid_utf8(subject));
        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            malformed_race, NULL, NULL, NULL, 1,
            false, false, true, false);
        CHECK(strcmp(subject, "牠") == 0);

        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            "幽靈戰士", NULL, NULL, malformed_ghost, 1,
            false, false, false, false);
        CHECK(strcmp(subject, "幽靈") == 0);
        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            "獸人", malformed_plural, NULL, NULL, 3,
            false, false, false, false);
        CHECK(strcmp(subject, "3 隻獸人") == 0);

        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            "獸人", NULL, malformed_qualifier, NULL, 1,
            false, false, false, false);
        CHECK(strcmp(subject, "這隻獸人") == 0);
        angdroid_fa_mon_msg_chinese_subject(subject, sizeof subject,
            valid_replacement, NULL, NULL, NULL, 1,
            false, false, false, false);
        CHECK(strcmp(subject, "這隻受損\xEF\xBF\xBD怪物") == 0);
        CHECK(valid_utf8(subject));
    }

    /* The original rounded-up damage average remains stable for normal
     * counts, without count+1 integer overflow near INT_MAX. */
    CHECK(angdroid_fa_mon_msg_average_damage(5, 2) == 3);
    CHECK(angdroid_fa_mon_msg_average_damage(4, 2) == 2);
    CHECK(angdroid_fa_mon_msg_average_damage(1, 2) == 1);
    CHECK(angdroid_fa_mon_msg_average_damage(0, INT_MAX) == 0);
    CHECK(angdroid_fa_mon_msg_average_damage(INT_MAX, INT_MAX) == 1);
    CHECK(angdroid_fa_mon_msg_average_damage(INT_MAX, 2)
        == INT_MAX / 2 + 1);
    CHECK(angdroid_fa_mon_msg_average_damage(-3, 2) == -1);
    CHECK(angdroid_fa_mon_msg_average_damage(100, 0) == 0);
    CHECK(angdroid_fa_mon_msg_average_damage(100, -1) == 0);

    printf("PASS: %u C99 group-message Unicode subject checks\n", checks);
    return 0;
}
