/* C99: UTF-8 complete glyphs in confirmation and choice prompts.
 * Key handling and canonical item names remain unchanged by these helpers.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/confirmation-prompt-display.h"

static unsigned long checks;

static void build(const char *prompt, const char *suffix, size_t columns,
    size_t capacity, int expect_suffix)
{
    char buf[512];
    size_t displayed;
    memset(buf, 0x7f, sizeof(buf));
    displayed = angdroid_confirmation_prompt_display(
        buf, capacity, prompt, suffix, columns);
    assert(strlen(buf) < capacity);
    assert(displayed == angdroid_utf8_display_columns(buf));
    assert(displayed <= columns);
    if (expect_suffix) assert(strstr(buf, suffix) != NULL);
    checks += 4;
}

int main(void)
{
    size_t columns, capacity;
    char clipped[16];

    for (columns = 6; columns <= 96; ++columns) {
        build("確定卸下火焰之劍（已鑑定的裝備）嗎？ ", "[y/n] ",
            columns, 512, 1);
        build("Really take off the long named Sword of Dawn? ", "[y/n] ",
            columns, 512, 1);
        build("好長好長的中文名稱 ", "[a/b] ", columns, 512, 1);
        build("abc\xE4\xB8", "[y/n] ", columns, 512, 1);
        build(NULL, "[y/n] ", columns, 512, 1);
    }
    for (columns = 0; columns < 6; ++columns)
        build("中文超長提示", "[y/n] ", columns, 512, 0);

    /* Ensure a long prompt never eats the required response keys. */
    for (capacity = 1; capacity <= 96; ++capacity) {
        char out[96];
        size_t display = angdroid_confirmation_prompt_display(
            out, capacity, "火焰之劍與附加屬性很長的提示 ",
            "[y/n] ", 40);
        assert(display <= 40);
        assert(out[capacity - 1] == '\0' || strlen(out) < capacity);
        assert(angdroid_utf8_display_columns(out) == display);
        checks += 3;
    }
    assert(angdroid_confirmation_prompt_display(
        clipped, sizeof(clipped), "ABC", "[y/n] ", 16) == 9);
    assert(strcmp(clipped, "ABC[y/n] ") == 0);
    checks += 2;
    assert(angdroid_confirmation_prompt_display(
        clipped, sizeof(clipped), NULL, NULL, 16) == 0);
    assert(clipped[0] == '\0');
    checks += 2;
    assert(angdroid_confirmation_prompt_display(
        NULL, 10, "hi", "[y/n] ", 16) == 0);
    checks++;
    printf("PASS: UTF-8-safe confirmation prompts and suffixes (%lu checks)\n",
        checks);
    return 0;
}
