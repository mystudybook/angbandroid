/* Seven Traditional Chinese flavored item types: standalone C99 regression. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-flavored-object.h"
static void expect(const char *tval, const char *appearance,
    const char *effect, bool prefix, unsigned int count, const char *value)
{
    char out[256];
    assert(angdroid_zh_tw_flavored_object(out, sizeof(out), tval,
        appearance, effect, prefix, count));
    assert(strcmp(out, value) == 0);
}
int main(void)
{
    char out[256], tiny[4];
    expect("ring", "紅寶石", NULL, true, 1, "紅寶石戒指");
    expect("ring", "紅寶石", "傳送", true, 1, "紅寶石戒指（傳送）");
    expect("ring", "紅寶石", NULL, true, 3, "3 枚紅寶石戒指");
    expect("ring", "紅寶石", NULL, true, 0, "沒有剩餘的紅寶石戒指");
    expect("amulet", "水晶", NULL, true, 2, "2 枚水晶護身符");
    expect("staff", "橡木", NULL, true, 2, "2 根橡木法杖");
    expect("wand", "鍍銅", NULL, true, 2, "2 根鍍銅魔杖");
    expect("rod", "銀", NULL, true, 2, "2 根銀魔棒");
    expect("potion", "黏稠粉紅色", NULL, true, 2, "2 瓶黏稠粉紅色藥水");
    expect("mushroom", "綠色", NULL, true, 2, "2 朵綠色蘑菇");
    expect("potion", "黏稠粉紅色", "治療輕傷", false, 2,
        "黏稠粉紅色藥水（治療輕傷）");
    expect("potion", "紅色", NULL, true, 65535, "65535 瓶紅色藥水");
    assert(!angdroid_zh_tw_flavored_object(out, sizeof(out),
        "scroll", "紅色", NULL, true, 1));
    assert(out[0] == '\0');
    assert(!angdroid_zh_tw_flavored_object(out, sizeof(out),
        "ring", NULL, NULL, true, 1));
    assert(!angdroid_zh_tw_flavored_object(out, sizeof(out),
        "ring", "紅色", "", true, 1));
    assert(!angdroid_zh_tw_flavored_object(tiny, sizeof(tiny),
        "ring", "紅色", NULL, true, 1));
    assert(tiny[0] == '\0');
    assert(angdroid_zh_tw_flavored_object(out, sizeof(out),
        "ring", "紅色", NULL, true, 1));
    {
        size_t length = strlen(out);
        assert(angdroid_zh_tw_flavored_object(out, length + 1,
            "ring", "紅色", NULL, true, 1));
        assert(!angdroid_zh_tw_flavored_object(out, length,
            "ring", "紅色", NULL, true, 1));
        assert(out[0] == '\0');
    }
    puts("PASS: seven flavored noun forms and safe UTF-8 buffers");
    return 0;
}
