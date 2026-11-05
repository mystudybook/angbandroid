/* Native Angband timed-condition notifications (display-only).
 * Exact messages come from variant_lib/angband/gamedata/player_timed.txt.
 * Do not translate timed grade names, status IDs, logic or effect records.
 */
#ifndef ANGDROID_ZH_TW_TIMED_MESSAGES_H
#define ANGDROID_ZH_TW_TIMED_MESSAGES_H
#include <stddef.h>
#include <string.h>

struct angdroid_timed_message_pair {
    const char *english;
    const char *traditional;
};

/* Only translate exact canonical strings at the point of displaying a
 * timed-condition message. Unknown messages and other locales stay intact. */
static inline const char *angdroid_timed_message(const char *english, int zh_tw)
{
    static const struct angdroid_timed_message_pair messages[] = {
        { "You resist the effect!", "你抵抗了這次效果！" },
        { "You feel yourself moving faster!", "你感覺自己移動得更快了！" },
        { "You feel yourself slow down.", "你感覺自己的速度慢了下來。" },
        { "You feel yourself moving slower!", "你感覺自己移動得更慢了！" },
        { "You feel yourself speed up.", "你感覺自己的速度恢復了。" },
        { "You are blind.", "你失明了。" },
        { "You blink and your eyes clear.", "你眨了眨眼，視力恢復了。" },
        { "You are paralysed!", "你麻痺了！" },
        { "You can move again.", "你又可以行動了。" },
        { "You are confused!", "你陷入混亂！" },
        { "You are no longer confused.", "你的混亂狀態解除了。" },
        { "You are more confused!", "你變得更加混亂！" },
        { "You feel a little less confused.", "你感覺沒那麼混亂了。" },
        { "You are terrified!", "你嚇壞了！" },
        { "You feel bolder now.", "你現在感覺更有勇氣了。" },
        { "You are more scared!", "你變得更加害怕！" },
        { "You feel drugged!", "你開始出現幻覺！" },
        { "You can see clearly again.", "你又能看清周圍了。" },
        { "You feel more drugged!", "你的幻覺更加嚴重了！" },
        { "You are poisoned!", "你中毒了！" },
        { "You are no longer poisoned.", "你的中毒狀態解除了。" },
        { "You are more poisoned!", "你的中毒情況加重了！" },
        { "You are less poisoned.", "你的中毒情況減輕了。" },
        { "You have been given a graze.", "你受了擦傷。" },
        { "You have been given a light cut.", "你受了輕微割傷。" },
        { "You have been given a bad cut.", "你受了較嚴重的割傷。" },
        { "You have been given a nasty cut.", "你受了嚴重的割傷。" },
        { "You have been given a severe cut.", "你受了非常嚴重的割傷。" },
        { "You have been given a deep gash.", "你身上出現一道深深的傷口。" },
        { "You have been given a mortal wound.", "你受了致命傷！" },
        { "You are no longer bleeding.", "你已經止血了。" },
        { "You have been stunned.", "你暈眩了。" },
        { "You have been heavily stunned.", "你陷入嚴重暈眩。" },
        { "You have been knocked out.", "你被擊昏了！" },
        { "You are no longer stunned.", "你的暈眩狀態解除了。" },
        { "You are starving!!", "你快餓死了！！" },
        { "You are still faint.", "你依然因飢餓而虛弱無力。" },
        { "You are getting faint from hunger!", "你餓得快要昏倒了！" },
        { "You are still weak.", "你依然很虛弱。" },
        { "You are getting weak from hunger!", "飢餓讓你越來越虛弱！" },
        { "You are still hungry.", "你還是覺得餓。" },
        { "You are getting hungry.", "你開始感到飢餓。" },
        { "You are no longer hungry.", "你不再感到飢餓了。" },
        { "You are no longer full.", "你不再有飽足感了。" },
        { "You are full!", "你吃飽了！" },
        { "You feel safe from evil!", "你感覺邪惡無法輕易傷害你！" },
        { "You no longer feel safe from evil.", "你不再受到邪惡防護的保護。" },
        { "You feel even safer from evil!", "你感覺對邪惡的防護更強了！" },
        { "You feel invulnerable!", "你感覺自己刀槍不入！" },
        { "You feel vulnerable once more.", "你的無敵狀態結束了。" },
        { "You feel like a hero!", "你感覺自己充滿英雄氣概！" },
        { "You no longer feel heroic.", "你的英雄氣概消退了。" },
        { "You feel more like a hero!", "你的英雄氣概更盛了！" },
        { "You feel like a killing machine!", "你進入了狂暴狀態！" },
        { "You no longer feel berserk.", "你的狂暴狀態結束了。" },
        { "You feel even more berserk!", "你變得更加狂暴了！" },
        { "A mystic shield forms around your body!", "一道神秘護盾環繞著你的身體！" },
        { "Your mystic shield crumbles away.", "你的神秘護盾消散了。" },
        { "The mystic shield strengthens.", "你的神秘護盾變得更強了。" },
        { "You feel righteous!", "你感受到神聖祝福的力量！" },
        { "The prayer has expired.", "祝福的力量消退了。" },
        { "You feel more righteous!", "你感覺神聖祝福更加強大！" },
        { "Your eyes feel very sensitive!", "你的雙眼變得格外敏銳！" },
        { "Your eyes no longer feel so sensitive.", "你的雙眼不再那麼敏銳了。" },
        { "Your eyes feel more sensitive!", "你的雙眼變得更加敏銳！" },
        { "Your eyes begin to tingle!", "你的雙眼開始微微刺痛！" },
        { "Your eyes stop tingling.", "你的雙眼不再刺痛了。" },
        { "The tingling in your eyes intensifies.", "你眼中的刺痛感更加強烈。" },
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(messages) / sizeof(messages[0]); ++i) {
        if (strcmp(english, messages[i].english) == 0)
            return messages[i].traditional;
    }
    return english;
}
#endif
