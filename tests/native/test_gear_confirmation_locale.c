/* C99: UTF-8 complete glyphs in confirmation and item-selection prompts.
 * Key handling and canonical item names remain unchanged by these helpers.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/confirmation-prompt-display.h"
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"

static unsigned long checks;

static void build(const char *prompt, const char *suffix, size_t columns,
    size_t capacity, int expect_suffix)
{
    char buf[512];
    size_t displayed;
    memset(buf, 0x7f, sizeof(buf));
    displayed = angdroid_confirmation_prompt_display(
        buf, capacity, prompt, suffix, columns);
    assert(buf[capacity - 1] != 0x7f || capacity == sizeof(buf));
    assert(displayed == angdroid_utf8_display_columns(buf));
    assert(displayed <= columns);
    if (expect_suffix) assert(strstr(buf, suffix) != NULL);
    checks += 4;
}

static void phrase(const char *english, const char *traditional)
{
    assert(strcmp(angdroid_game_ui_text(english, 0), english) == 0);
    assert(strcmp(angdroid_game_ui_text(english, 1), traditional) == 0);
    checks += 2;
}

int main(void)
{
    size_t columns, capacity;
    char clipped[16];

    phrase("Wear or wield which item?", "要穿戴或裝備哪件物品？");
    phrase("You have nothing to wear or wield.", "沒有可以穿戴或裝備的物品。");
    phrase("Take off or unwield which item?", "要卸下哪件裝備？");
    phrase("You have nothing to take off or unwield.", "沒有可以卸下的裝備。");
    phrase("Really take off %s? ", "確定卸下 %s 嗎？ ");
    phrase("Inscribing %s.", "正在為 %s 刻字。");

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
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    checks++;
    assert(strcmp(angdroid_game_ui_text("unknown confirmation", 1),
        "unknown confirmation") == 0);
    checks++;
    printf("PASS: bilingual gear/inscription prompts and UTF-8 confirmation (%lu checks)\n",
        checks);
    return 0;
}
