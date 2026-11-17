/*
 * FAangband melee consequences and player-ghost challenges.
 * Directly checks the real shared production translation dictionary in C99.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct fa_case { const char *english; const char *chinese; };
static const struct fa_case cases[] = {
    {"challenges you from beyond the grave!", "從墳墓彼端向你發出挑戰！"},
    {"thunders 'Prove worthy of your traditions - or die ashamed!'.", "怒吼：「證明你配得上祖先的榮耀，否則就羞愧地死去！」"},
    {"desires to test your mettle!", "想試試你的膽識！"},
    {"has risen from the dead to test you!", "從死者之中復甦，前來考驗你！"},
    {"roars 'Fight, or know yourself for a coward!'.", "咆哮：「來戰，否則就承認自己是懦夫！」"},
    {"summons you to a duel of life and death!", "向你提出生死決鬥！"},
    {"desires you to know that you face a mighty champion of yore!", "要讓你知道，眼前是昔日強大的勇士！"},
    {"demands that you prove your worthiness in combat!", "要求你在戰鬥中證明自己的價值！"},
    {"calls you unworthy of your ancestors!", "斥責你有辱祖先！"},
    {"challenges you to a deathmatch!", "向你發出決一死戰的挑戰！"},
    {"walks Middle-Earth once more!", "再度行走於中土世界！"},
    {"challenges you to demonstrate your prowess!", "向你挑戰，要你展現真正的本領！"},
    {"demands you prove yourself here and now!", "要你此時此刻證明自己的實力！"},
    {"asks 'Can ye face the best of those who came before?'.", "問道：「你敢面對昔日最傑出的勇士嗎？」"},
    {"You are covered in acid!", "你被酸液淋得全身都是！"},
    {"You are struck by electricity!", "電流擊中了你！"},
    {"You are enveloped in flames!", "烈焰包圍了你！"},
    {"You are covered with frost!", "你的身上覆滿寒霜！"},
    {"You keep hold of your life force!", "你成功守住了自己的生命力！"},
    {"You feel your life slipping away!", "你感覺生命力正一點點流逝！"},
    {"You feel your life draining away!", "你感覺生命力正迅速流失！"},
    {"Energy drains from your pack!", "你背包中的能量被吸走了！"},
    {"You quickly protect your money pouch!", "你迅速護住了錢袋！"},
    {"Nothing was stolen.", "沒有東西被偷走。"},
    {"Your purse feels lighter.", "你覺得錢袋變輕了。"},
    {"All of your coins were stolen!", "你的錢幣全被偷走了！"},
    {"You grab hold of your backpack!", "你趕緊抓緊背包！"},
};

static int checks = 0;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    char result[256];
    const char *unknown = "unknown FA ghost challenge";
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        CHECK(strcmp(angdroid_game_ui_text(cases[i].english, 1),
            cases[i].chinese) == 0);
        CHECK(angdroid_game_ui_text(cases[i].english, 0) == cases[i].english);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);

    (void)snprintf(result, sizeof(result), "你受到了 %d 點傷害。", 25);
    CHECK(strcmp(result, "你受到了 25 點傷害。") == 0);
    (void)snprintf(result, sizeof(result), "%d 枚錢幣被偷走了！", 125);
    CHECK(strcmp(result, "125 枚錢幣被偷走了！") == 0);
    (void)snprintf(result, sizeof(result), "你的一件%s（%c）被偷走了！",
        "短劍", 'a');
    CHECK(strcmp(result, "你的一件短劍（a）被偷走了！") == 0);
    (void)snprintf(result, sizeof(result), "%s（%s）%s",
        "幽靈", "劍士", angdroid_game_ui_text(cases[0].english, 1));
    CHECK(strcmp(result, "幽靈（劍士）從墳墓彼端向你發出挑戰！") == 0);
    (void)snprintf(result, sizeof(result), "%s, the %s %s",
        "Ghost", "Warrior", angdroid_game_ui_text(cases[0].english, 0));
    CHECK(strcmp(result, "Ghost, the Warrior challenges you from beyond the grave!") == 0);

    printf("PASS: %d FA melee/ghost translation checks\n", checks);
    return 0;
}
