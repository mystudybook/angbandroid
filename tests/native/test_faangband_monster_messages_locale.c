/*
 * Full bilingual coverage for FAangband monster status / injury source text.
 * Uses the production zh-tw-ui.h dictionary, with strict C99 host compilation.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct locale_pair { const char *en; const char *zh; };
static const struct locale_pair cases[] = {
    {"[is|are] hurt.", "受傷了。"},
    {"die[s].", "死去了。"},
    {"[is|are] destroyed.", "被摧毀了。"},
    {"resist[s] a lot.", "大幅抵抗了攻擊。"},
    {"[is|are] hit hard.", "受到沉重打擊。"},
    {"resist[s].", "抵抗了攻擊。"},
    {"[is|are] immune.", "完全免疫。"},
    {"resist[s] somewhat.", "抵抗了部分攻擊。"},
    {"[is|are] unaffected!", "毫髮無傷！"},
    {"spawn[s]!", "繁殖了！"},
    {"look[s] healthier.", "看起來恢復了活力。"},
    {"fall[s] asleep!", "睡著了！"},
    {"wake[s] up.", "醒了過來。"},
    {"cringe[s] from the light!", "在光芒下畏縮！"},
    {"shrivel[s] away in the light!", "在光芒下枯萎消散！"},
    {"[is|are] dismayed by the Light!", "被聖光震懾了！"},
    {"lose[s] some skin!", "脫落了一些皮膚！"},
    {"dissolve[s]!", "溶解了！"},
    {"catch[es] fire!", "燃燒了起來！"},
    {"[is|are] badly frozen.", "被嚴重冰凍了。"},
    {"[is|are] struck by lightning!", "被閃電擊中了！"},
    {"shudder[s].", "顫抖了一下。"},
    {"change[s]!", "發生了變化！"},
    {"disappear[s]!", "消失了！"},
    {"[is|are] even more stunned.", "更加暈眩了。"},
    {"[is|are] stunned.", "陷入暈眩。"},
    {"[is|are] no longer stunned.", "不再暈眩了。"},
    {"look[s] more confused.", "看起來更加混亂。"},
    {"look[s] confused.", "看起來陷入混亂。"},
    {"[is|are] no longer confused.", "不再混亂了。"},
    {"look[s] more slowed.", "看起來更加遲緩。"},
    {"look[s] slowed.", "看起來行動遲緩。"},
    {"speed[s] up.", "加快了速度。"},
    {"look[s] even faster!", "看起來速度更快了！"},
    {"start[s] moving faster.", "開始加速移動。"},
    {"slow[s] down.", "放慢了速度。"},
    {"look[s] more terrified!", "看起來更加恐懼！"},
    {"flee[s] in terror!", "驚恐地逃跑了！"},
    {"[is|are] no longer afraid.", "不再害怕了。"},
    {"[is|are] frozen to the spot!", "僵在原地無法動彈！"},
    {"can move again.", "又能移動了。"},
    {"seem[s] less magical!", "看起來魔力減弱了！"},
    {"seem[s] magical again.", "看起來恢復了魔力。"},
    {"falls under your spell!", "落入你的法術控制之下！"},
    {"is no longer under your control.", "不再受你控制了。"},
    {"shimmers for a moment.", "短暫閃爍了一下。"},
    {"You hear [a|several] scream[|s] of agony!", "你聽見[一聲|數聲]痛苦的尖叫！"},
    {"disintegrate[s]!", "瓦解消失了！"},
    {"freeze[s] and shatter[s]!", "凍結後碎裂了！"},
    {"lose[s] some mana!", "失去了一些魔力！"},
    {"look[s] briefly puzzled.", "短暫露出困惑的神情。"},
    {"maintain[s] the same shape.", "維持了原本的形態。"},
    {"fight[s] off your spell.", "奮力掙脫你的法術。"},
    {"[is|are] unharmed.", "毫髮無傷。"},
    {"appear[s]!", "現身了！"},
    {"There is a puff of smoke!", "一陣煙霧冒了出來！"},
    {"[is|are] embedded in rock!", "被埋進岩石裡！"},
    {"wail[s] out in pain!", "痛苦地哀號！"},
    {"shrug[s] off the attack.", "若無其事地承受了這次攻擊。"},
    {"grunt[s] with pain.", "痛苦地咕噥了一聲。"},
    {"cr[ies|y] out in pain.", "痛苦地叫出聲來。"},
    {"scream[s] in pain.", "痛苦地尖叫。"},
    {"scream[s] in agony.", "發出淒厲的慘叫。"},
    {"writhe[s] in agony.", "痛苦地扭動身軀。"},
    {"cr[ies|y] out feebly.", "虛弱地哀叫。"},
    {"barely notice[s].", "幾乎沒有察覺。"},
    {"flinch[es].", "畏縮了一下。"},
    {"squelch[es].", "發出濕黏的擠壓聲。"},
    {"quiver[s] in pain.", "痛苦地顫動。"},
    {"writhe[s] about.", "不斷扭動。"},
    {"jerk[s] limply.", "無力地抽搐。"},
    {"snarl[s] with pain.", "痛苦地咆哮。"},
    {"yelp[s] in pain.", "痛苦地嗚叫。"},
    {"howl[s] in pain.", "痛苦地嚎叫。"},
    {"howl[s] in agony.", "痛苦地哀嚎。"},
    {"yelp[s] feebly.", "虛弱地嗚叫。"},
    {"ignore[s] the attack.", "完全不理會攻擊。"},
    {"squeal[s] in pain.", "痛苦地尖聲叫喊。"},
    {"shriek[s] in pain.", "痛苦地尖嘯。"},
    {"shriek[s] in agony.", "淒厲地尖嘯。"},
    {"hiss[es].", "發出嘶嘶聲。"},
    {"rear[s] up in anger.", "憤怒地昂起身軀。"},
    {"hiss[es] furiously.", "憤怒地嘶叫。"},
    {"snarl[s].", "發出低沉的咆哮。"},
    {"growl[s] angrily.", "憤怒地低吼。"},
    {"hiss[es] in pain.", "痛苦地嘶叫。"},
    {"mewl[s] in pain.", "痛苦地呻吟。"},
    {"hiss[es] in agony.", "痛苦地嘶鳴。"},
    {"mewl[s] pitifully.", "可憐地哀鳴。"},
    {"drone[s] angrily.", "憤怒地嗡嗡作響。"},
    {"scuttle[s] about.", "四處急速爬動。"},
    {"twitch[es] in pain.", "痛苦地抽動。"},
    {"jerk[s] in pain.", "痛苦地猛然抽搐。"},
    {"jerk[s] in agony.", "在劇痛中抽搐。"},
    {"jerk[s] feebly.", "虛弱地抽搐。"},
    {"flap[s] angrily.", "憤怒地拍動翅膀。"},
    {"jeer[s] in pain.", "痛苦地發出嘲叫。"},
    {"squawk[s] with pain.", "痛苦地嘎嘎叫。"},
    {"twitter[s] in agony.", "在劇痛中啁啾。"},
    {"flutter[s] about.", "四處拍翅亂飛。"},
    {"chirp[s] feebly.", "虛弱地啾叫。"},
    {"jerk[s].", "猛地抽動。"},
    {"rattle[s].", "發出喀啦作響的聲音。"},
    {"clatter[s].", "發出金屬碰撞聲。"},
    {"shake[s].", "不斷搖晃。"},
    {"stagger[s].", "踉蹌了一下。"},
    {"crumple[s].", "癱倒在地。"},
    {"grunt[s].", "咕噥了一聲。"},
    {"moan[s].", "發出呻吟。"},
    {"groan[s].", "低聲呻吟。"},
    {"hesitate[s].", "猶豫不決。"},
    {"spin[s] fiercely.", "猛烈地旋轉。"},
    {"swirl[s] about.", "四處旋動。"},
    {"twist[s] around.", "扭轉身軀。"},
    {"spin[s] slowly.", "緩緩旋轉。"},
    {"swirl[s] weakly.", "虛弱地旋動。"},
    {"twist[s] limply.", "無力地扭曲。"},
    {"laugh[s] off the attack.", "笑著不把攻擊當回事。"},
    {"sneer[s].", "冷笑了一聲。"},
    {"scowl[s].", "皺起眉頭。"},
    {"bellow[s] in rage.", "憤怒地怒吼。"},
    {"scream[s] in fury.", "憤怒地尖叫。"},
    {"wince[s].", "痛得縮起身子。"},
};

static int checked;
#define CHECK(condition) do { if (!(condition)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #condition); \
    return 1; } ++checked; } while (0)

int main(void)
{
    size_t i;
    const char *unknown = "not-a-listed-FAangband-pain-message";
    const char *moria = "You hear [a|several] scream[|s] of agony!";
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *localized = angdroid_game_ui_text(cases[i].en, 1);
        CHECK(strcmp(localized, cases[i].zh) == 0);
        CHECK(angdroid_game_ui_text(cases[i].en, 0) == cases[i].en);
        CHECK(strlen(localized) < 256);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);
    CHECK(strcmp(angdroid_game_ui_text(moria, 1),
        "你聽見[一聲|數聲]痛苦的尖叫！") == 0);
    CHECK(angdroid_game_ui_text(moria, 0) == moria);
    CHECK(strcmp(angdroid_game_ui_text("cr[ies|y] out in pain.", 1),
        "痛苦地叫出聲來。") == 0);

    printf("PASS: %d bilingual monster event and pain checks\n", checked);
    return 0;
}
