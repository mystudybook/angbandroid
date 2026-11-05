/* Display-only native player events and non-scoring command warnings.
 * Save-file death causes, history entries and gameplay command identifiers
 * remain in their original canonical format.
 */
#ifndef ANGDROID_ZH_TW_PLAYER_FEEDBACK_H
#define ANGDROID_ZH_TW_PLAYER_FEEDBACK_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_player_feedback(const char *english,
    int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "You must choose a level you have previously visited.", "你必須選擇曾經到過的樓層。" },
        { "Your lust for blood keeps you alive!", "你對鮮血的渴望支撐著你的生命！" },
        { "So great was his prowess and skill in warfare, the Elves said: ", "他的武藝如此高強，精靈們因此說道：" },
        { "'The Mormegil cannot be slain, save by mischance.'", "「黑劍士只有因意外才能被殺死。」" },
        { "Your light has gone out!", "你的光源熄滅了！" },
        { "Your light is growing faint.", "你的光源越來越微弱。" },
        { "You angrily lash out at a nearby foe!", "你憤怒地向附近的敵人發動攻擊！" },
        { "You have damaged your health!", "你因過度勉強而損傷了健康！" },
        { "You faint from the effort!", "你因過度勞累而昏倒！" },
        { "Wounds appear on your body!", "你的身上出現了傷口！" },
        { "You feel suddenly lethargic.", "你突然感到全身乏力。" },
        { "You resume your usual shape.", "你恢復了原本的形態。" },
        { "You cannot pray or produce magics.", "你無法施法或祈禱。" },
        { "You cannot see!", "你看不見！" },
        { "You are too confused!", "你太混亂了！" },
        { "You can't see anything.", "你什麼都看不見。" },
        { "You have no light to read by.", "你沒有足夠的光線可以閱讀。" },
        { "You are too confused to read!", "你太混亂了，無法閱讀！" },
        { "You can't remember how to read!", "你想不起來該如何閱讀！" },
        { "You have nothing to fire with.", "你沒有可以使用的遠程武器。" },
        { "Your light cannot be refuelled.", "你的光源無法補充燃料。" },
        { "You are too confused.", "你太混亂了。" },
        { "You are confused.", "你陷入混亂。" },
        { "There is a scream and the door slams shut!", "一聲尖叫傳來，門砰地關上了！" },
        { "You have found a secret door.", "你發現了一扇暗門。" },
        { "You have discovered a trap on the chest!", "你發現寶箱上有陷阱！" },
        { "You die.", "你死了。" },
        { "*** LOW HITPOINT WARNING! ***", "＊＊＊ 生命值過低警告！ ＊＊＊" },
        { "You are about to enter 'wizard' mode for the very first time!", "你即將第一次進入「巫師模式」！" },
        { "This is a form of cheating, and your game will not be scored!", "這屬於作弊模式，本次遊戲將不列入排行榜！" },
        { "Wizard mode off.", "巫師模式已關閉。" },
        { "Wizard mode on.", "巫師模式已開啟。" },
        { "You are about to use the dangerous, unsupported, borg commands!", "你即將使用危險且不受支援的 Borg 指令！" },
        { "Your machine may crash, and your savefile may become corrupted!", "程式可能當機，存檔也可能因此損毀！" },
        { "Die? ", "確定要讓角色死亡嗎？" },
        { "Are you sure you want to enter wizard mode? ", "確定要進入巫師模式嗎？" },
        { "Are you sure you want to use the borg commands? ", "確定要使用 Borg 指令嗎？" },
        { "Note: ", "筆記：" },
        { "The lava burns you!", "熔岩灼傷了你！" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
