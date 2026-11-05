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
    verify("You feel resistant to acid!", "你獲得了酸液抗性！");
    verify("You are no longer resistant to acid.", "你的酸液抗性消失了。");
    verify("You feel more resistant to acid!", "你的酸液抗性增強了！");
    verify("You feel resistant to electricity!", "你獲得了閃電抗性！");
    verify("You are no longer resistant to electricity.", "你的閃電抗性消失了。");
    verify("You feel more resistant to electricity!", "你的閃電抗性增強了！");
    verify("You feel resistant to fire!", "你獲得了火焰抗性！");
    verify("You are no longer resistant to fire.", "你的火焰抗性消失了。");
    verify("You feel more resistant to fire!", "你的火焰抗性增強了！");
    verify("You feel resistant to cold!", "你獲得了寒冷抗性！");
    verify("You are no longer resistant to cold.", "你的寒冷抗性消失了。");
    verify("You feel more resistant to cold!", "你的寒冷抗性增強了！");
    verify("You feel resistant to poison!", "你獲得了毒素抗性！");
    verify("You are no longer resistant to poison.", "你的毒素抗性消失了。");
    verify("You feel more resistant to poison!", "你的毒素抗性增強了！");
    verify("You feel resistant to confusion!", "你獲得了混亂抗性！");
    verify("You are no longer resistant to confusion.", "你的混亂抗性消失了。");
    verify("You feel more resistant to confusion!", "你的混亂抗性增強了！");
    verify("You feel your memories fade.", "你感覺記憶正在逐漸消退。");
    verify("Your memories come flooding back.", "你的記憶再次湧現。");
    verify("Your mind expands.", "你的心靈感知擴展了。");
    verify("Your horizons are once more limited.", "你的心靈感知恢復了原本的範圍。");
    verify("Your mind expands further.", "你的心靈感知變得更加敏銳。");
    verify("Your skin turns to stone.", "你的皮膚變得像石頭一樣堅硬。");
    verify("A fleshy shade returns to your skin.", "你的皮膚恢復了原本的柔軟。");
    verify("You feel the need to run away and fast!", "你感到強烈的逃跑衝動！");
    verify("The urge to run dissipates.", "逃跑的衝動逐漸消失了。");
    verify("You start sprinting.", "你開始全力衝刺。");
    verify("You suddenly stop sprinting.", "你突然停止了衝刺。");
    verify("You feel bold.", "你變得勇敢起來。");
    verify("You no longer feel bold.", "你的勇氣消退了。");
    verify("You feel even bolder!", "你變得更加勇敢！");
    verify("Your body starts to scramble...", "你的身體開始發生扭曲……");
    verify("Your body reasserts its true nature.", "你的身體恢復了原本的形態。");
    verify("You are more scrambled!", "你的身體變得更加扭曲！");
    verify("You feel safe from traps.", "你感覺自己不會受到陷阱傷害。");
    verify("You feel vulnerable to traps again.", "你再次感覺到陷阱的威脅。");
    verify("You feel even safer from traps!", "你感覺自己更能避開陷阱！");
    verify("You feel your mind accelerate.", "你感覺思緒運轉得更快了。");
    verify("You feel your mind slow again.", "你感覺思緒恢復了正常速度。");
    verify("Your {kind} start{s} to drip with acid!", "你的{kind}開始滴下酸液！");
    verify("The acid disappears from your {kind}.", "你{kind}上的酸液消失了。");
    verify("Sparks gather around your {kind}!", "火花聚集在你的{kind}周圍！");
    verify("The sparks fade from your {kind}.", "你{kind}上的火花逐漸消散。");
    verify("Flames surround your {kind}!", "火焰環繞著你的{kind}！");
    verify("The flames on your {kind} subside.", "你{kind}上的火焰逐漸熄滅。");
    verify("Your {kind} {is} covered in frost!", "你的{kind}覆蓋了一層冰霜！");
    verify("The frost evaporates from your {kind}.", "你{kind}上的冰霜蒸發了。");
    verify("Your {kind} drip{s} with poison!", "你的{kind}開始滴下毒液！");
    verify("The poison disappears from your {kind}.", "你{kind}上的毒液消失了。");
    verify("Your hands begin to glow!", "你的雙手開始發光！");
    verify("The glow fades from your hands.", "你雙手上的光芒消失了。");
    verify("Your {kind} {is} imbued with holy force!", "你的{kind}充滿了神聖力量！");
    verify("Holy force fades from your {kind}.", "你{kind}上的神聖力量消散了。");
    verify("Your {kind} become{s} a bane of demons!", "你的{kind}獲得了剋制惡魔的力量！");
    verify("Your {kind} return{s} to normal.", "你的{kind}恢復了原狀。");
    verify("Your can drain life from your victims!", "你能從敵人身上吸取生命力！");
    verify("Your life-draining power passes.", "你的吸取生命能力消失了。");
    verify("Your metabolism speeds up!", "你的新陳代謝加快了！");
    verify("Your metabolism return to normal.", "你的新陳代謝恢復了正常。");
    verify("You focus on your target.", "你集中精神控制目標。");
    verify("Your resume control of your own body.", "你重新取得了自己身體的控制權。");
    verify("You tense up ready to escape!", "你繃緊身體，準備隨時逃脫！");
    verify("You relax again.", "你再次放鬆下來。");
    verify("You begin covering your tracks.", "你開始掩蓋自己的行蹤。");
    verify("You stop covering your tracks.", "你不再掩蓋自己的行蹤。");
    verify("You prepare to shoot powerfully!", "你準備使出強力射擊！");
    verify("Your shooting power returns to normal.", "你的射擊威力恢復了正常。");
    verify("You start aggravating your enemies!", "你開始挑釁周圍的敵人！");
    verify("You stop aggravating your enemies.", "你停止挑釁敵人。");
    verify("A red mist descends over your eyes!", "你的眼前籠罩著一片紅霧！");
    verify("Your thirst for blood increases!", "你對鮮血的渴望加深了！");
    verify("Your thirst for blood is insatiable!", "你對鮮血的渴望已無法滿足！");
    verify("The mist clears.", "你眼前的紅霧散去了。");
    verify("You are overcome by the Black Breath.", "你受到黑暗氣息的侵蝕。");
    verify("The Black Shadow lifts.", "籠罩你的黑暗陰影消散了。");
    verify("You feel silent!", "你變得悄然無聲！");
    verify("You feel less stealthy.", "你的潛行能力減弱了。");
    verify("Your limbs fill with energy!", "你的四肢充滿了力量！");
    verify("Your limbs return to normal.", "你的四肢恢復了正常。");
    verify("Your limbs feel more energetic!", "你的四肢變得更加靈活有力！");
    /* Chinese text preserves the object kind tag while dropping English
     * singular/plural verb suffix tags ({s}, {is}) when they are unnecessary. */
    {
        const char *acid = angdroid_timed_message(
            "Your {kind} start{s} to drip with acid!", 1);
        const char *frost = angdroid_timed_message(
            "Your {kind} {is} covered in frost!", 1);
        assert(strstr(acid, "{kind}") != NULL);
        assert(strstr(frost, "{kind}") != NULL);
        assert(strstr(acid, "{s}") == NULL);
        assert(strstr(frost, "{is}") == NULL);
        checked += 4;
    }
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
