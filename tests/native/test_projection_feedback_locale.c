/* Source-matched gameplay projection feedback and exact locale fallback. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-projection-feedback.h"
static int checks;
static void verify(const char *(*translate)(const char *, int),
    const char *english, const char *chinese)
{
    const char *got = translate(english, 1);
    assert(got && strcmp(got, chinese) == 0);
    assert(translate(english, 0) == english);
    checks += 2;
}
int main(void)
{
    verify(angdroid_project_feedback, "The intense heat saps you.", "強烈的熱氣讓你虛弱不堪。");
    verify(angdroid_project_feedback, "Your eyes fill with smoke!", "濃煙灌進你的雙眼！");
    verify(angdroid_project_feedback, "You are assailed by poisonous fumes!", "有毒煙霧向你襲來！");
    verify(angdroid_project_feedback, "The cold seeps into your bones.", "寒氣滲入你的骨髓。");
    verify(angdroid_project_feedback, "The cold withers your life force!", "嚴寒正在侵蝕你的生命力！");
    verify(angdroid_project_feedback, "You resist the effect!", "你抵抗了這次效果！");
    verify(angdroid_project_feedback, "The venom stings your skin!", "毒液刺痛了你的皮膚！");
    verify(angdroid_project_feedback, "The stench sickens you.", "惡臭讓你感到噁心。");
    verify(angdroid_project_feedback, "You are dazzled!", "強光讓你眼花撩亂！");
    verify(angdroid_project_feedback, "The darkness steals your life force!", "黑暗奪走了你的生命力！");
    verify(angdroid_project_feedback, "You feel unsure of yourself in the darkness.", "你在黑暗中感到不安。");
    verify(angdroid_project_feedback, "Darkness penetrates your mind!", "黑暗侵入了你的心智！");
    verify(angdroid_project_feedback, "The noise disorients you.", "刺耳的聲響讓你失去方向感。");
    verify(angdroid_project_feedback, "You avoid the effect!", "你避開了這次效果！");
    verify(angdroid_project_feedback, "You feel your life force draining away!", "你感覺生命力正在流失！");
    verify(angdroid_project_feedback, "Your mind is dulled.", "你的思緒變得遲鈍。");
    verify(angdroid_project_feedback, "Your energy is sapped!", "你的精力被抽走了！");
    verify(angdroid_project_feedback, "Gravity warps around you.", "你周圍的重力發生扭曲。");
    verify(angdroid_project_feedback, "You're not as powerful as you used to be...", "你感覺自己不再像以前那麼強大……");
    verify(angdroid_project_stat, "strong", "力量");
    verify(angdroid_project_stat, "bright", "智力");
    verify(angdroid_project_stat, "wise", "智慧");
    verify(angdroid_project_stat, "agile", "敏捷");
    verify(angdroid_project_stat, "hale", "體魄");
    verify(angdroid_project_blind_term, "acid", "酸液");
    verify(angdroid_project_blind_term, "lightning", "閃電");
    verify(angdroid_project_blind_term, "fire", "火焰");
    verify(angdroid_project_blind_term, "cold", "寒氣");
    verify(angdroid_project_blind_term, "poison", "毒素");
    verify(angdroid_project_blind_term, "something", "不明力量");
    verify(angdroid_project_blind_term, "noise", "聲波");
    verify(angdroid_project_blind_term, "something sharp", "尖銳物體");
    verify(angdroid_project_blind_term, "something strange", "奇異力量");
    verify(angdroid_project_blind_term, "something cold", "寒冷之物");
    verify(angdroid_project_blind_term, "water", "水流");
    verify(angdroid_project_blind_term, "something hard", "堅硬物體");
    assert(angdroid_project_feedback("unseen future event", 1) &&
           strcmp(angdroid_project_feedback("unseen future event", 1),
               "unseen future event") == 0);
    assert(angdroid_project_stat("mystical", 1) &&
           strcmp(angdroid_project_stat("mystical", 1), "mystical") == 0);
    assert(angdroid_project_blind_term(NULL, 1) == NULL);
    checks += 3;
    printf("PASS: projection, stat, and blind attack message locale (%d checks)\n",
           checks);
    return 0;
}
