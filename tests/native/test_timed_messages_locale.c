/* Original Angband timed-condition notifications in both locales. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-timed-messages.h"

static int checked;
static void verify(const char *en, const char *zh)
{
    const char *localized = angdroid_timed_message(en, 1);
    assert(localized != NULL);
    assert(strcmp(localized, zh) == 0);
    assert(angdroid_timed_message(en, 0) == en);
    checked += 2;
}
int main(void)
{
    verify("You resist the effect!", "你抵抗了這次效果！");
    verify("You feel yourself moving faster!", "你感覺自己移動得更快了！");
    verify("You feel yourself slow down.", "你感覺自己的速度慢了下來。");
    verify("You feel yourself moving slower!", "你感覺自己移動得更慢了！");
    verify("You feel yourself speed up.", "你感覺自己的速度恢復了。");
    verify("You are blind.", "你失明了。");
    verify("You blink and your eyes clear.", "你眨了眨眼，視力恢復了。");
    verify("You are paralysed!", "你麻痺了！");
    verify("You can move again.", "你又可以行動了。");
    verify("You are confused!", "你陷入混亂！");
    verify("You are no longer confused.", "你的混亂狀態解除了。");
    verify("You are more confused!", "你變得更加混亂！");
    verify("You feel a little less confused.", "你感覺沒那麼混亂了。");
    verify("You are terrified!", "你嚇壞了！");
    verify("You feel bolder now.", "你現在感覺更有勇氣了。");
    verify("You are more scared!", "你變得更加害怕！");
    verify("You feel drugged!", "你開始出現幻覺！");
    verify("You can see clearly again.", "你又能看清周圍了。");
    verify("You feel more drugged!", "你的幻覺更加嚴重了！");
    verify("You are poisoned!", "你中毒了！");
    verify("You are no longer poisoned.", "你的中毒狀態解除了。");
    verify("You are more poisoned!", "你的中毒情況加重了！");
    verify("You are less poisoned.", "你的中毒情況減輕了。");
    verify("You have been given a graze.", "你受了擦傷。");
    verify("You have been given a light cut.", "你受了輕微割傷。");
    verify("You have been given a bad cut.", "你受了較嚴重的割傷。");
    verify("You have been given a nasty cut.", "你受了嚴重的割傷。");
    verify("You have been given a severe cut.", "你受了非常嚴重的割傷。");
    verify("You have been given a deep gash.", "你身上出現一道深深的傷口。");
    verify("You have been given a mortal wound.", "你受了致命傷！");
    verify("You are no longer bleeding.", "你已經止血了。");
    verify("You have been stunned.", "你暈眩了。");
    verify("You have been heavily stunned.", "你陷入嚴重暈眩。");
    verify("You have been knocked out.", "你被擊昏了！");
    verify("You are no longer stunned.", "你的暈眩狀態解除了。");
    verify("You are starving!!", "你快餓死了！！");
    verify("You are still faint.", "你依然因飢餓而虛弱無力。");
    verify("You are getting faint from hunger!", "你餓得快要昏倒了！");
    verify("You are still weak.", "你依然很虛弱。");
    verify("You are getting weak from hunger!", "飢餓讓你越來越虛弱！");
    verify("You are still hungry.", "你還是覺得餓。");
    verify("You are getting hungry.", "你開始感到飢餓。");
    verify("You are no longer hungry.", "你不再感到飢餓了。");
    verify("You are no longer full.", "你不再有飽足感了。");
    verify("You are full!", "你吃飽了！");
    verify("You feel safe from evil!", "你感覺邪惡無法輕易傷害你！");
    verify("You no longer feel safe from evil.", "你不再受到邪惡防護的保護。");
    verify("You feel even safer from evil!", "你感覺對邪惡的防護更強了！");
    verify("You feel invulnerable!", "你感覺自己刀槍不入！");
    verify("You feel vulnerable once more.", "你的無敵狀態結束了。");
    verify("You feel like a hero!", "你感覺自己充滿英雄氣概！");
    verify("You no longer feel heroic.", "你的英雄氣概消退了。");
    verify("You feel more like a hero!", "你的英雄氣概更盛了！");
    verify("You feel like a killing machine!", "你進入了狂暴狀態！");
    verify("You no longer feel berserk.", "你的狂暴狀態結束了。");
    verify("You feel even more berserk!", "你變得更加狂暴了！");
    verify("A mystic shield forms around your body!", "一道神秘護盾環繞著你的身體！");
    verify("Your mystic shield crumbles away.", "你的神秘護盾消散了。");
    verify("The mystic shield strengthens.", "你的神秘護盾變得更強了。");
    verify("You feel righteous!", "你感受到神聖祝福的力量！");
    verify("The prayer has expired.", "祝福的力量消退了。");
    verify("You feel more righteous!", "你感覺神聖祝福更加強大！");
    verify("Your eyes feel very sensitive!", "你的雙眼變得格外敏銳！");
    verify("Your eyes no longer feel so sensitive.", "你的雙眼不再那麼敏銳了。");
    verify("Your eyes feel more sensitive!", "你的雙眼變得更加敏銳！");
    verify("Your eyes begin to tingle!", "你的雙眼開始微微刺痛！");
    verify("Your eyes stop tingling.", "你的雙眼不再刺痛了。");
    verify("The tingling in your eyes intensifies.", "你眼中的刺痛感更加強烈。");
    {
        const char *future = "A brand-new timed effect!";
        const char *tagged = "Your {name} glow{s}!";
        assert(angdroid_timed_message(future, 1) == future);
        assert(angdroid_timed_message(tagged, 1) == tagged);
        assert(angdroid_timed_message(NULL, 1) == NULL);
        assert(angdroid_timed_message(NULL, 0) == NULL);
        checked += 4;
    }
    printf("PASS: %d native timed-condition bilingual checks\n", checked);
    return 0;
}
