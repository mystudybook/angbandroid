/* C99 native glyph editor regression: hex digits, scalar validity,
 * cancellation/invalid input cannot replace the selected character.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/knowledge-glyph-codepoint.h"
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"

static int checks;
static void check_hex(const char *input, bool valid, uint32_t expected)
{
    uint32_t output = 0x1234u;
    bool result = angdroid_knowledge_parse_glyph_hex(input, &output);
    assert(result == valid);
    assert(output == (valid ? expected : 0x1234u));
    checks += 2;
}
static void check_locale(const char *english, const char *chinese)
{
    assert(strcmp(angdroid_game_ui_text(english, 0), english) == 0);
    assert(strcmp(angdroid_game_ui_text(english, 1), chinese) == 0);
    checks += 2;
}
int main(void)
{
    check_hex("20", true, 0x20);
    check_hex("0041", true, 0x41);
    check_hex("4E2D", true, 0x4e2d);
    check_hex("4e2d", true, 0x4e2d);
    check_hex("1F431", true, 0x1f431);
    check_hex("FFFD", true, 0xfffd);
    check_hex("ffffD", true, 0xffffd);
    check_hex("00A0", true, 0xa0);
    check_hex("0", false, 0);
    check_hex("", false, 0);
    check_hex(NULL, false, 0);
    check_hex("0001", false, 0);
    check_hex("001F", false, 0);
    check_hex("007f", false, 0);
    check_hex("0080", false, 0);
    check_hex("009f", false, 0);
    check_hex("D800", false, 0);
    check_hex("dFFF", false, 0);
    check_hex("FDD0", false, 0);
    check_hex("FDEF", false, 0);
    check_hex("FFFE", false, 0);
    check_hex("FFFF", false, 0);
    check_hex("1FFFF", false, 0);
    check_hex("1F44A0", false, 0);
    check_hex("12G4", false, 0);
    check_hex("0x41", false, 0);
    check_hex(" 41", false, 0);
    check_hex("41 ", false, 0);
    check_hex("41\n", false, 0);
    check_hex("41+", false, 0);
    check_hex("\xE4\xB8\xAD", false, 0);
    { uint32_t unchanged = 7;
      assert(!angdroid_knowledge_parse_glyph_hex("41", NULL));
      assert(unchanged == 7);
      checks += 2;
    }
    check_locale("(up to 5 hex digits):",
                 "Unicode 十六進位（1～5 位）：");
    check_locale("Invalid Unicode code point.",
                 "Unicode 字元碼無效，請輸入合法字元。");
    check_locale("Inscribe with: ", "刻上文字：");
    assert(strcmp(angdroid_game_ui_text("unmapped", 1), "unmapped") == 0);
    checks++;
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    checks++;
    puts("PASS: native glyph code-point parser and bilingual inscription UI");
    printf("Assertions checked: %d\n", checks);
    return 0;
}
