/* Exact display-only Traditional Chinese translations of the canonical
 * Angband activation.txt (48 msg lines) and artifact.txt (5 msg lines).
 * These resolve to 50 distinct templates. Preserve {name} for the original
 * print_custom_message() formatter and avoid English {kind}/{s}/{is}
 * inflections in Traditional Chinese. Unknown alt-msg strings fall back.
 */
#ifndef ANGDROID_ZH_TW_ACTIVATION_MESSAGES_H
#define ANGDROID_ZH_TW_ACTIVATION_MESSAGES_H

#include <stddef.h>
#include <string.h>

static inline const char *angdroid_zh_tw_activation_message(
    const char *english, int zh_tw)
{
    static const struct { const char *en; const char *zh; } map[] = {
        { "Your {kind} radiate{s} deep purple...", "{name} 散發深紫色光芒……" },
        { "You feel life flow through your body!", "你感覺生命力流遍全身！" },
        { "You feel a warm tingling inside...", "你感覺體內湧起一股溫暖的酥麻感……" },
        { "{name} glows a bright white...", "{name} 發出耀眼的白光……" },
        { "{name} glows many colours...", "{name} 閃耀著五彩光芒……" },
        { "{name} forms an image in your mind...", "{name} 讓你的腦海浮現一幅影像……" },
        { "{name} twists space around you...", "{name} 扭曲了你周圍的空間……" },
        { "Your {kind} twist{s} space around you.", "{name} 扭曲了你周圍的空間。" },
        { "The {kind} shine{s} brightly...", "{name} 綻放明亮光芒……" },
        { "Your {kind} glow{s} bright red...", "{name} 發出鮮紅色光芒……" },
        { "Your {kind} glow{s} bright yellow...", "{name} 發出明黃色光芒……" },
        { "Your {kind} glow{s} deep blue...", "{name} 發出深藍色光芒……" },
        { "The {kind} let{s} out a shrill wail...", "{name} 發出尖銳的哀號……" },
        { "{name} pulses a soft white.", "{name} 閃動著柔和的白光。" },
        { "Your {kind} let{s} out a long, shrill note...", "{name} 發出悠長尖銳的聲響……" },
        { "The {kind} well{s} with clear light...", "{name} 湧現清澈的光芒……" },
        { "The {kind} form{s} an image in your mind...", "{name} 讓你的腦海浮現一幅影像……" },
        { "{name} glows brightly...", "{name} 明亮地發光……" },
        { "Your {kind} glow{s} bright green...", "{name} 發出鮮綠色光芒……" },
        { "The {kind} glow{s} brightly...", "{name} 發出明亮光芒……" },
        { "The {kind} flood{s} the area with goodness...", "{name} 讓四周充滿神聖的力量……" },
        { "{name} glows deep blue...", "{name} 發出深藍色光芒……" },
        { "Lightning surrounds your {kind}!", "{name} 被閃電環繞！" },
        { "{name} glow{s} many colors...", "{name} 閃耀著五彩光芒……" },
        { "Your {kind} glow{s} a deep red...", "{name} 發出深紅色光芒……" },
        { "Your {kind} {is} covered in fire...", "{name} 被烈焰包覆……" },
        { "{name} rages in fire...", "{name} 燃起熊熊烈焰……" },
        { "The {kind} glow{s} deep red...", "{name} 發出深紅色光芒……" },
        { "Your {kind} {is} covered in frost...", "{name} 被冰霜覆蓋……" },
        { "{name} glows a pale blue...", "{name} 發出淡藍色光芒……" },
        { "{name} glows an intense blue...", "{name} 發出強烈的藍光……" },
        { "Your {kind} {is} covered in acid...", "{name} 被酸液覆蓋……" },
        { "Your {kind} {is} covered in sparks...", "{name} 迸發出火花……" },
        { "{name} glows white...", "{name} 發出白光……" },
        { "Your {kind} glow{s} black...", "{name} 散發黑色光芒……" },
        { "{name} glows very brightly...", "{name} 發出極為耀眼的光芒……" },
        { "Your {kind} glow{s} white...", "{name} 發出白光……" },
        { "{name} glows intensely black...", "{name} 散發濃烈的黑色光芒……" },
        { "Your {kind} form{s} a magical arrow...", "{name} 凝聚出一支魔法箭矢……" },
        { "{name} throws deep green shadows...", "{name} 投射出深綠色陰影……" },
        { "Your {kind} pulsate{s}...", "{name} 開始脈動……" },
        { "Your {kind} glow{s} deep red...", "{name} 發出深紅色光芒……" },
        { "{name} glows in scintillating colours...", "{name} 閃爍著絢麗的色彩……" },
        { "A line of shimmering blue light appears.", "一道閃爍的藍光出現了。" },
        { "Your {kind} glow{s} with the light of a thousand stars...", "{name} 綻放宛如千顆星辰的光芒……" },
        { "{name} glows in anger!", "{name} 因憤怒而發光！" },
        { "Your {kind} grow{s} magical spikes...", "{name} 長出魔法尖刺……" },
        { "Your {kind} throbs deep blue...", "{name} 閃動著深藍色光芒……" },
        { "{name} glows deep red...", "{name} 發出深紅色光芒……" },
        { "{name} glows bright white...", "{name} 發出明亮的白光……" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
