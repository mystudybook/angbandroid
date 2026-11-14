/* Standalone C99 tests: equipment detail labels and buffer boundaries. */
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-item-details.h"

static void expect(enum angdroid_zh_tw_combat_field type,
                   int a, int b, const char *label)
{
    char buf[128];
    assert(angdroid_zh_tw_item_combat_text(buf, sizeof(buf), type, a, b));
    assert(strcmp(buf, label) == 0);
}
int main(void)
{
    char buf[128], tiny[12], overflow[1] = {'X'};
    int mods[] = {+3, -2, +12};
    size_t end;

    expect(ANGDROID_ITEM_DICE, 2, 6, "（傷害 2d6）");
    expect(ANGDROID_ITEM_MULTIPLIER, 3, 0, "（倍率 ×3）");
    expect(ANGDROID_ITEM_ATTACK_PAIR, 4, -1, "（命中 +4／傷害 -1）");
    expect(ANGDROID_ITEM_HIT, -3, 0, "（命中 -3）");
    expect(ANGDROID_ITEM_DAMAGE, 0, 0, "（傷害 +0）");
    expect(ANGDROID_ITEM_ARMOR_PAIR, 12, 6, "〔護甲 12／防護 +6〕");
    expect(ANGDROID_ITEM_ARMOR, 5, 0, "〔護甲 5〕");
    expect(ANGDROID_ITEM_ARMOR_BONUS, -2, 0, "〔防護 -2〕");
    expect(ANGDROID_ITEM_HIT, INT_MIN, 0, "（命中 -2147483648）");

    assert(angdroid_zh_tw_item_modifiers_text(buf, sizeof(buf), mods, 3));
    assert(strcmp(buf, "〈+3、-2、+12〉") == 0);
    assert(!angdroid_zh_tw_item_modifiers_text(tiny, sizeof(tiny), mods, 3));
    assert(tiny[0] == '\0');
    assert(!angdroid_zh_tw_item_modifiers_text(buf, sizeof(buf), NULL, 3));
    assert(!angdroid_zh_tw_item_modifiers_text(buf, sizeof(buf), mods, 0));
    assert(!angdroid_zh_tw_item_combat_text(tiny, sizeof(tiny),
        ANGDROID_ITEM_ATTACK_PAIR, 4, -1));
    assert(tiny[0] == '\0');
    assert(!angdroid_zh_tw_item_combat_text(buf, sizeof(buf),
        (enum angdroid_zh_tw_combat_field)99, 0, 0));

    strcpy(buf, "長劍");
    end = strlen(buf);
    assert(angdroid_zh_tw_item_append_whole(buf, sizeof(buf), &end, "（屠龍）"));
    assert(strcmp(buf, "長劍（屠龍）") == 0);
    assert(!angdroid_zh_tw_item_append_whole(buf, end + 1, &end, "（強效屠龍）"));
    assert(strcmp(buf, "長劍（屠龍）") == 0);
    assert(angdroid_zh_tw_item_append_amount(
        buf, sizeof(buf), &end, "（剩餘 ", 45, " 次充能）"));
    assert(strcmp(buf, "長劍（屠龍）（剩餘 45 次充能）") == 0);

    /* A translated ego/artifact suffix must fit with BOTH brackets.
     * A codepoint-only append would leave "長劍（屠" in the short buffer. */
    {
        char suffix_text[64], narrow[16], exact[19];
        assert(angdroid_zh_tw_item_suffix(suffix_text, sizeof(suffix_text),
            "屠龍"));
        strcpy(narrow, "長劍");
        end = strlen(narrow);
        assert(!angdroid_zh_tw_item_append_whole(narrow, sizeof(narrow),
            &end, suffix_text));
        assert(end == strlen("長劍"));
        assert(strcmp(narrow, "長劍") == 0);
        strcpy(exact, "長劍");
        end = strlen(exact);
        assert(angdroid_zh_tw_item_append_whole(exact, sizeof(exact),
            &end, suffix_text));
        assert(strcmp(exact, "長劍（屠龍）") == 0);
        assert(end == strlen("長劍（屠龍）"));
    }

    strcpy(tiny, "OK");
    end = strlen(tiny);
    assert(!angdroid_zh_tw_item_append_whole(
        tiny, sizeof(tiny), &end, "（傷害 2d6）"));
    assert(end == 2 && strcmp(tiny, "OK") == 0);

    buf[0] = '\0';
    end = 0;
    assert(!angdroid_zh_tw_item_append_whole(buf, sizeof(buf), &end,
                                             "\xC0\xAF"));
    assert(buf[0] == '\0' && end == 0);
    assert(!angdroid_zh_tw_item_append_whole(NULL, sizeof(buf), &end, "測試"));
    assert(!angdroid_zh_tw_item_append_whole(buf, sizeof(buf), NULL, "測試"));
    assert(!angdroid_zh_tw_item_append_amount(buf, sizeof(buf), &end,
                                               NULL, 1, " 次"));
    assert(!angdroid_zh_tw_item_append_amount(buf, sizeof(buf), &end,
                                               "剩餘 ", 1, NULL));
    assert(!angdroid_zh_tw_item_append_whole(overflow, 0, &end, "測試"));
    assert(overflow[0] == 'X');
    puts("PASS: equipment combat labels, modifiers and full UTF-8 annotations");
    return 0;
}
