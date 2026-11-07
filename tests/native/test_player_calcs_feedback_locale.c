/* C99 source-matched inventory/weapon/armor feedback translation smoke. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-player-calcs-feedback.h"
static int checks;
static void verify(const char *en, const char *zh)
{
    assert(strcmp(angdroid_player_calcs_feedback(en, 1), zh) == 0);
    assert(angdroid_player_calcs_feedback(en, 0) == en);
    checks += 2;
}
int main(void)
{
    verify("You re-arrange your quiver.", "你重新整理了箭袋。");
    verify("You re-arrange your pack.", "你重新整理了背包。");
    verify("You have trouble wielding such a heavy bow.", "這把弓太重了，讓你難以操作。");
    verify("You have no trouble wielding your bow.", "你現在可以輕鬆使用這把弓。");
    verify("You feel relieved to put down your heavy bow.", "放下沉重的弓後，你感到輕鬆多了。");
    verify("You have trouble wielding such a heavy weapon.", "這把武器太重了，讓你難以揮舞。");
    verify("You have no trouble wielding your weapon.", "你現在可以輕鬆揮舞武器。");
    verify("You feel relieved to put down your heavy weapon.", "放下沉重的武器後，你感到輕鬆多了。");
    verify("You feel attuned to your weapon.", "你感覺自己與武器更加契合。");
    verify("You feel less attuned to your weapon.", "你感覺自己與武器不再那麼契合。");
    verify("The weight of your armor encumbers your movement.", "護甲的重量妨礙了你的行動。");
    verify("You feel able to move more freely.", "你感覺行動變得更加自如。");
    assert(strcmp(angdroid_player_calcs_feedback("future equipment", 1),
                  "future equipment") == 0);
    assert(angdroid_player_calcs_feedback(NULL, 1) == NULL);
    checks += 2;
    printf("PASS: player calcs fixed messages (%d checks)\n", checks);
    return 0;
}
