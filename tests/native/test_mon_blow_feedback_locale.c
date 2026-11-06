/* Independent C99 message lookup / English fallback regression. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-blow-feedback.h"
static int checks = 0;
static void verify(const char *en, const char *zh)
{
    assert(strcmp(angdroid_mon_blow_feedback(en, 1), zh) == 0);
    assert(angdroid_mon_blow_feedback(en, 0) == en);
    checks += 2;
}
int main(void)
{
    verify("You are covered in acid!", "酸液濺滿了你的全身！");
    verify("You are struck by electricity!", "你遭到電流電擊！");
    verify("You are enveloped in flames!", "火焰包圍了你！");
    verify("You are covered with frost!", "你的全身覆滿了冰霜！");
    verify("You keep hold of your life force!", "你成功守住了生命力！");
    verify("You feel your life slipping away!", "你感覺生命力正逐漸流失！");
    verify("You feel your life draining away!", "你感覺生命力正在被抽走！");
    verify("Energy drains from your pack!", "你的背包裡有能量被吸走了！");
    verify("You quickly protect your money pouch!", "你迅速護住了錢袋！");
    verify("Nothing was stolen.", "沒有任何東西被偷走。");
    verify("Your purse feels lighter.", "你感覺錢袋變輕了。");
    verify("All of your coins were stolen!", "你的金幣全被偷走了！");
    verify("You grab hold of your backpack!", "你緊緊抓住了背包！");
    assert(strcmp(angdroid_mon_blow_feedback("future attack", 1), "future attack") == 0);
    assert(angdroid_mon_blow_feedback(NULL, 1) == NULL);
    checks += 2;
    printf("PASS: %d native monster blow locale checks\n", checks);
    return 0;
}
