/* Exact bilingual source messages and unchanged unknown fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-player-feedback.h"
static int n;
static void check(const char *en, const char *zh)
{
    assert(strcmp(angdroid_player_feedback(en, 1), zh) == 0);
    assert(angdroid_player_feedback(en, 0) == en);
    n += 2;
}
int main(void)
{
    check("You must choose a level you have previously visited.", "你必須選擇曾經到過的樓層。");
    check("Your lust for blood keeps you alive!", "你對鮮血的渴望支撐著你的生命！");
    check("So great was his prowess and skill in warfare, the Elves said: ", "他的武藝如此高強，精靈們因此說道：");
    check("'The Mormegil cannot be slain, save by mischance.'", "「黑劍士只有因意外才能被殺死。」");
    check("Your light has gone out!", "你的光源熄滅了！");
    check("Your light is growing faint.", "你的光源越來越微弱。");
    check("You angrily lash out at a nearby foe!", "你憤怒地向附近的敵人發動攻擊！");
    check("You have damaged your health!", "你因過度勉強而損傷了健康！");
    check("You faint from the effort!", "你因過度勞累而昏倒！");
    check("Wounds appear on your body!", "你的身上出現了傷口！");
    check("You feel suddenly lethargic.", "你突然感到全身乏力。");
    check("You resume your usual shape.", "你恢復了原本的形態。");
    check("You cannot pray or produce magics.", "你無法施法或祈禱。");
    check("You cannot see!", "你看不見！");
    check("You are too confused!", "你太混亂了！");
    check("You can't see anything.", "你什麼都看不見。");
    check("You have no light to read by.", "你沒有足夠的光線可以閱讀。");
    check("You are too confused to read!", "你太混亂了，無法閱讀！");
    check("You can't remember how to read!", "你想不起來該如何閱讀！");
    check("You have nothing to fire with.", "你沒有可以使用的遠程武器。");
    check("Your light cannot be refuelled.", "你的光源無法補充燃料。");
    check("You are too confused.", "你太混亂了。");
    check("You are confused.", "你陷入混亂。");
    check("There is a scream and the door slams shut!", "一聲尖叫傳來，門砰地關上了！");
    check("You have found a secret door.", "你發現了一扇暗門。");
    check("You have discovered a trap on the chest!", "你發現寶箱上有陷阱！");
    check("You die.", "你死了。");
    check("*** LOW HITPOINT WARNING! ***", "＊＊＊ 生命值過低警告！ ＊＊＊");
    check("You are about to enter 'wizard' mode for the very first time!", "你即將第一次進入「巫師模式」！");
    check("This is a form of cheating, and your game will not be scored!", "這屬於作弊模式，本次遊戲將不列入排行榜！");
    check("Wizard mode off.", "巫師模式已關閉。");
    check("Wizard mode on.", "巫師模式已開啟。");
    check("You are about to use the dangerous, unsupported, borg commands!", "你即將使用危險且不受支援的 Borg 指令！");
    check("Your machine may crash, and your savefile may become corrupted!", "程式可能當機，存檔也可能因此損毀！");
    check("Die? ", "確定要讓角色死亡嗎？");
    check("Are you sure you want to enter wizard mode? ", "確定要進入巫師模式嗎？");
    check("Are you sure you want to use the borg commands? ", "確定要使用 Borg 指令嗎？");
    check("Note: ", "筆記：");
    check("The lava burns you!", "熔岩灼傷了你！");
    assert(angdroid_player_feedback("unknown future prompt", 1) &&
           strcmp(angdroid_player_feedback("unknown future prompt", 1),
                  "unknown future prompt") == 0);
    assert(angdroid_player_feedback(NULL, 1) == NULL);
    n += 2;
    printf("PASS: Angband player and misc command messages (%d checks)\n", n);
    return 0;
}
