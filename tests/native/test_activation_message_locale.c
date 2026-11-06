/* Native C99: canonical Angband activation templates are presentation only.
 * No gameplay object or Android environment is needed to exercise mapping.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-activation-messages.h"

static int checked;
static void check(const char *en, const char *zh)
{
    const char *result = angdroid_zh_tw_activation_message(en, 1);
    const char *cursor;
    assert(result && strcmp(result, zh) == 0);
    assert(angdroid_zh_tw_activation_message(en, 0) == en);
    assert(angdroid_zh_tw_activation_message(en, 2) == result);
    /* Chinese sentences must not contain English grammar tags. The
     * original custom printer still resolves a leading {name} tag. */
    cursor = strchr(result, '{');
    if (cursor) {
        assert(cursor == result);
        assert(strncmp(cursor, "{name}", 6) == 0);
        assert(strchr(cursor + 6, '{') == NULL);
    }
    assert(strstr(result, "{kind}") == NULL);
    assert(strstr(result, "{s}") == NULL);
    assert(strstr(result, "{is}") == NULL);
    checked += 6;
}

int main(void)
{
    const char *unknown = "A future unknown artifact {name} pulses.";
    check("Your {kind} radiate{s} deep purple...", "{name} 散發深紫色光芒……");
    check("You feel life flow through your body!", "你感覺生命力流遍全身！");
    check("You feel a warm tingling inside...", "你感覺體內湧起一股溫暖的酥麻感……");
    check("{name} glows a bright white...", "{name} 發出耀眼的白光……");
    check("{name} glows many colours...", "{name} 閃耀著五彩光芒……");
    check("{name} forms an image in your mind...", "{name} 讓你的腦海浮現一幅影像……");
    check("{name} twists space around you...", "{name} 扭曲了你周圍的空間……");
    check("Your {kind} twist{s} space around you.", "{name} 扭曲了你周圍的空間。");
    check("The {kind} shine{s} brightly...", "{name} 綻放明亮光芒……");
    check("Your {kind} glow{s} bright red...", "{name} 發出鮮紅色光芒……");
    check("Your {kind} glow{s} bright yellow...", "{name} 發出明黃色光芒……");
    check("Your {kind} glow{s} deep blue...", "{name} 發出深藍色光芒……");
    check("The {kind} let{s} out a shrill wail...", "{name} 發出尖銳的哀號……");
    check("{name} pulses a soft white.", "{name} 閃動著柔和的白光。");
    check("Your {kind} let{s} out a long, shrill note...", "{name} 發出悠長尖銳的聲響……");
    check("The {kind} well{s} with clear light...", "{name} 湧現清澈的光芒……");
    check("The {kind} form{s} an image in your mind...", "{name} 讓你的腦海浮現一幅影像……");
    check("{name} glows brightly...", "{name} 明亮地發光……");
    check("Your {kind} glow{s} bright green...", "{name} 發出鮮綠色光芒……");
    check("The {kind} glow{s} brightly...", "{name} 發出明亮光芒……");
    check("The {kind} flood{s} the area with goodness...", "{name} 讓四周充滿神聖的力量……");
    check("{name} glows deep blue...", "{name} 發出深藍色光芒……");
    check("Lightning surrounds your {kind}!", "{name} 被閃電環繞！");
    check("{name} glow{s} many colors...", "{name} 閃耀著五彩光芒……");
    check("Your {kind} glow{s} a deep red...", "{name} 發出深紅色光芒……");
    check("Your {kind} {is} covered in fire...", "{name} 被烈焰包覆……");
    check("{name} rages in fire...", "{name} 燃起熊熊烈焰……");
    check("The {kind} glow{s} deep red...", "{name} 發出深紅色光芒……");
    check("Your {kind} {is} covered in frost...", "{name} 被冰霜覆蓋……");
    check("{name} glows a pale blue...", "{name} 發出淡藍色光芒……");
    check("{name} glows an intense blue...", "{name} 發出強烈的藍光……");
    check("Your {kind} {is} covered in acid...", "{name} 被酸液覆蓋……");
    check("Your {kind} {is} covered in sparks...", "{name} 迸發出火花……");
    check("{name} glows white...", "{name} 發出白光……");
    check("Your {kind} glow{s} black...", "{name} 散發黑色光芒……");
    check("{name} glows very brightly...", "{name} 發出極為耀眼的光芒……");
    check("Your {kind} glow{s} white...", "{name} 發出白光……");
    check("{name} glows intensely black...", "{name} 散發濃烈的黑色光芒……");
    check("Your {kind} form{s} a magical arrow...", "{name} 凝聚出一支魔法箭矢……");
    check("{name} throws deep green shadows...", "{name} 投射出深綠色陰影……");
    check("Your {kind} pulsate{s}...", "{name} 開始脈動……");
    check("Your {kind} glow{s} deep red...", "{name} 發出深紅色光芒……");
    check("{name} glows in scintillating colours...", "{name} 閃爍著絢麗的色彩……");
    check("A line of shimmering blue light appears.", "一道閃爍的藍光出現了。");
    check("Your {kind} glow{s} with the light of a thousand stars...", "{name} 綻放宛如千顆星辰的光芒……");
    check("{name} glows in anger!", "{name} 因憤怒而發光！");
    check("Your {kind} grow{s} magical spikes...", "{name} 長出魔法尖刺……");
    check("Your {kind} throbs deep blue...", "{name} 閃動著深藍色光芒……");
    check("{name} glows deep red...", "{name} 發出深紅色光芒……");
    check("{name} glows bright white...", "{name} 發出明亮的白光……");
    assert(angdroid_zh_tw_activation_message(unknown, 1) == unknown);
    assert(angdroid_zh_tw_activation_message(unknown, 0) == unknown);
    assert(angdroid_zh_tw_activation_message("", 1)[0] == '\0');
    assert(angdroid_zh_tw_activation_message(NULL, 1) == NULL);
    assert(angdroid_zh_tw_activation_message(NULL, 0) == NULL);
    checked += 5;
    printf("PASS: activation and artifact dynamic zh-TW templates (%d checks)\n", checked);
    return 0;
}
