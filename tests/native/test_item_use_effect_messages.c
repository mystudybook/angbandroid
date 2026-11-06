/* Verify object.txt use/visual messages are presentation-only translations.
 * Original game-data strings, item effects and command IDs stay unchanged.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-item-use-messages.h"

static int checks;
static void mapped(const char *english, const char *chinese)
{
    const char *zh = angdroid_zh_tw_item_use_message(english, 1);
    const char *en = angdroid_zh_tw_item_use_message(english, 0);
    assert(zh && strcmp(zh, chinese) == 0);
    assert(en == english);
    assert(angdroid_zh_tw_item_use_message(english, 2) == zh);
    assert(strlen(zh) > 0);
    checks += 4;
}
int main(void)
{
    const char *unknown = "Future data message: %s";
    mapped("That tastes good.", "味道不錯。");
    mapped("That tastes better than nothing.", "總比什麼都不吃好。");
    mapped("That tastes disgusting.", "味道真噁心。");
    mapped("Delicious!", "真美味！");
    mapped("That tastes great.", "味道很棒。");
    mapped("That tastes great!", "味道真棒！");
    mapped("That tastes great!  A fine vintage.", "味道極佳！真是上等佳釀。");
    mapped("You feel your heart warm.", "你感覺心頭一暖。");
    mapped("It burns your throat.", "它灼燒著你的喉嚨。");
    mapped("Refreshing!", "清爽極了！");
    mapped("Your stomach heaves.", "你感到胃裡一陣翻騰。");
    mapped("There is a high pitched humming noise.", "傳來一陣尖銳的嗡鳴聲。");
    mapped("You hear a low-pitched whistling sound.", "你聽見一陣低沉的哨音。");
    mapped("You feel life flow through your body!", "你感覺生命力流遍全身！");
    mapped("The potion makes you queasy.", "這瓶藥水讓你反胃。");
    mapped("A line of shimmering blue light appears.", "一道閃爍的藍光出現了。");

    assert(angdroid_zh_tw_item_use_message(NULL, 0) == NULL);
    assert(angdroid_zh_tw_item_use_message(NULL, 1) == NULL);
    assert(angdroid_zh_tw_item_use_message(unknown, 0) == unknown);
    assert(angdroid_zh_tw_item_use_message(unknown, 1) == unknown);
    assert(angdroid_zh_tw_item_use_message("", 1)[0] == '\0');
    checks += 5;

    printf("PASS: object use messages bilingual display (%d assertions)\n",
        checks);
    return 0;
}
