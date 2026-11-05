#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-item-grammar.h"

static void name(const char *tval, const char *kind, unsigned int count,
                 const char *expected)
{
    char buf[128];
    assert(angdroid_zh_tw_known_item_name(buf, sizeof(buf),
        tval, kind, true, count));
    assert(strcmp(buf, expected) == 0);
}
static void suffix(const char *label, const char *expected)
{
    char buf[128];
    assert(angdroid_zh_tw_item_suffix(buf, sizeof(buf), label));
    assert(strcmp(buf, expected) == 0);
}
int main(void)
{
    char buf[128], small[10], sentinel[1] = { 'X' };
    size_t end;
    assert(ANGDROID_ZH_TW_ITEM_CLASSIFIERS == 36);
    name("arrow", "箭矢", 15, "15 支箭矢");
    name("bolt", "秘銀弩箭", 8, "8 支秘銀弩箭");
    name("ring", "力量戒指", 3, "3 枚力量戒指");
    name("boots", "皮靴", 2, "2 雙皮靴");
    name("scroll", "卷軸", 6, "6 張卷軸");
    name("sword", "長劍", 2, "2 把長劍");
    name("potion", "治療藥水", 4, "4 瓶治療藥水");
    name("magic book", "魔法書", 2, "2 本魔法書");
    name("bow", "長弓", 1, "長弓");
    name("bow", "長弓", 0, "沒有剩餘的長弓");
    name("unknown tval", "道具", 2, "2 件道具");
    assert(angdroid_zh_tw_identified_flavor_name(buf, sizeof(buf),
        "ring", "力量"));
    assert(strcmp(buf, "力量戒指") == 0);
    assert(angdroid_zh_tw_identified_flavor_name(buf, sizeof(buf),
        "scroll", "短距離傳送"));
    assert(strcmp(buf, "短距離傳送卷軸") == 0);
    assert(angdroid_zh_tw_identified_flavor_name(buf, sizeof(buf),
        "amulet", "智慧護身符"));
    assert(strcmp(buf, "智慧護身符") == 0);
    assert(angdroid_zh_tw_identified_flavor_name(buf, sizeof(buf),
        "sword", "長劍"));
    assert(strcmp(buf, "長劍") == 0);
    assert(!angdroid_zh_tw_identified_flavor_name(small, sizeof(small),
        "potion", "治療致命傷"));
    assert(small[0] == '\0');
    suffix("屠龍", "（屠龍）");
    suffix("（神聖復仇者）", "（神聖復仇者）");
    suffix("「安都瑞爾」", "「安都瑞爾」");
    suffix("凱蘭崔爾之物", "（凱蘭崔爾之物）");

    assert(!angdroid_zh_tw_item_suffix(small, sizeof(small),
        "（神聖復仇者）"));
    assert(small[0] == '\0');
    assert(!angdroid_zh_tw_known_item_name(small, sizeof(small),
        "ring", "力量戒指", true, 3));
    assert(small[0] == '\0');
    assert(angdroid_zh_tw_known_item_name(buf, sizeof(buf),
        "ring", "力量戒指", false, 3));
    assert(strcmp(buf, "力量戒指") == 0);
    assert(!angdroid_zh_tw_known_item_name(buf, sizeof(buf),
        "ring", NULL, true, 3));
    assert(!angdroid_zh_tw_item_suffix(buf, sizeof(buf), NULL));
    assert(!angdroid_zh_tw_item_suffix(NULL, 1, "屠龍"));
    assert(!angdroid_zh_tw_known_item_name(sentinel, 0,
        "ring", "力量戒指", true, 1));
    assert(sentinel[0] == 'X');
    assert(strcmp(angdroid_zh_tw_item_classifier(NULL), "件") == 0);
    assert(strcmp(angdroid_zh_tw_item_classifier("scroll"), "張") == 0);

    end = 0;
    assert(angdroid_zh_tw_item_append_utf8(small, sizeof(small),
        &end, "紅寶石戒指") == 9);
    assert(strcmp(small, "紅寶石") == 0 && end == 9);
    end = 0;
    assert(angdroid_zh_tw_item_append_utf8(small, 8,
        &end, "紅寶石戒指") == 6);
    assert(strcmp(small, "紅寶") == 0 && end == 6);
    end = 0;
    assert(angdroid_zh_tw_item_append_utf8(small, 3,
        &end, "紅寶石戒指") == 0);
    assert(strcmp(small, "") == 0 && end == 0);
    end = 0;
    assert(angdroid_zh_tw_item_append_utf8(buf, sizeof(buf),
        &end, "Hi「長劍」") == strlen("Hi「長劍」"));
    assert(strcmp(buf, "Hi「長劍」") == 0);
    end = 0;
    assert(angdroid_zh_tw_item_append_utf8(buf, sizeof(buf),
        &end, "\xC0\xAF") == 0);
    assert(buf[0] == '\0' && end == 0);
    puts("PASS: 36 canonical classes, Chinese suffixes and UTF-8 clipping");
    return 0;
}
