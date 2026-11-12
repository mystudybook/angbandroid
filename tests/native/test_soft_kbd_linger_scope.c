/* Host-side regression for a modal picker nested in a lingering prompt. */
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/curses/soft-kbd-linger-scope.h"

static int checks;
#define CHECK(x) do { ++checks; if (!(x)) {     fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

int main(void)
{
    soft_kbd_linger_snapshot saved;
    char current[256];
    char compact[4] = "xyz";
    char full[256];
    size_t i;

    /* Preserve Android control tokens byte-for-byte, including repeats. */
    angdroid_soft_kbd_capture_linger(&saved, "${quant}**", 0);
    CHECK(saved.lingering);
    CHECK(strcmp(saved.keys, "${quant}**") == 0);

    strcpy(current, "0123456789abcdef");
    /* Source switches may replace/clear the keys but must not restore yet. */
    CHECK(strcmp(current, saved.keys) != 0);
    strcpy(current, "");
    CHECK(angdroid_soft_kbd_recover_linger(
        current, sizeof(current), &saved, 1));
    CHECK(strcmp(current, "${quant}**") == 0);

    strcpy(current, "new command");
    CHECK(!angdroid_soft_kbd_recover_linger(
        current, sizeof(current), &saved, 0));
    CHECK(strcmp(current, "new command") == 0);

    /* Normal flashing keys are never parent prompts to be restored. */
    angdroid_soft_kbd_capture_linger(&saved, "abc", 1);
    CHECK(!saved.lingering);
    CHECK(!angdroid_soft_kbd_recover_linger(
        current, sizeof(current), &saved, 1));
    CHECK(strcmp(current, "new command") == 0);

    angdroid_soft_kbd_capture_linger(&saved, "", 0);
    CHECK(!saved.lingering);
    angdroid_soft_kbd_capture_linger(&saved, NULL, 0);
    CHECK(!saved.lingering);
    angdroid_soft_kbd_capture_linger(NULL, "abc", 0);
    CHECK(!angdroid_soft_kbd_recover_linger(
        current, sizeof(current), NULL, 1));
    CHECK(!angdroid_soft_kbd_recover_linger(
        NULL, sizeof(current), &saved, 1));

    /* Overflow refuses restoration atomically. */
    angdroid_soft_kbd_capture_linger(&saved, "abcdef", 0);
    CHECK(!angdroid_soft_kbd_recover_linger(
        compact, sizeof(compact), &saved, 1));
    CHECK(strcmp(compact, "xyz") == 0);
    CHECK(!angdroid_soft_kbd_recover_linger(
        current, 0, &saved, 1));
    CHECK(strcmp(current, "new command") == 0);

    /* Full 255-byte valid buffer fits a 256-byte snapshot and restores. */
    for (i = 0; i < sizeof(full) - 1; ++i) full[i] = 'a';
    full[sizeof(full) - 1] = '\0';
    angdroid_soft_kbd_capture_linger(&saved, full, 0);
    CHECK(saved.lingering);
    CHECK(strlen(saved.keys) == sizeof(full) - 1);
    strcpy(current, "other");
    CHECK(angdroid_soft_kbd_recover_linger(
        current, sizeof(current), &saved, 1));
    CHECK(strcmp(current, full) == 0);
    CHECK(!angdroid_soft_kbd_recover_linger(
        compact, sizeof(compact), &saved, 1));
    CHECK(strcmp(compact, "xyz") == 0);

    puts("PASS: item-picker lingering parent keys (25 assertions incl. boundary cases)");
    return 0;
}
