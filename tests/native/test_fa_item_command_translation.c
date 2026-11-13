/*
 * FAangband item-command dictionary: actual shared display-only header.
 * Strict C99 host test. English mode must retain its exact source string;
 * translation must not change command IDs or item ownership.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct label_test {
    const char *english;
    const char *chinese;
};

static const struct label_test cases[] = {
    {"Uninscribe which item?", "要清除哪件物品的刻字？"},
    {"You have nothing you can uninscribe.", "沒有可以清除刻字的物品。"},
    {"Inscribe which item?", "要為哪件物品刻字？"},
    {"You have nothing to inscribe.", "沒有可以刻字的物品。"},
    {"Inscribe with what? ", "要刻上什麼文字？ "},
    {"Take off or unwield which item?", "要卸下哪件裝備？"},
    {"You have nothing to take off or unwield.", "沒有可以卸下的裝備。"},
    {"Wear or wield which item?", "要穿戴或裝備哪件物品？"},
    {"You have nothing to wear or wield.", "沒有可以穿戴或裝備的物品。"},
    {"Replace which ring? ", "要替換哪枚戒指？ "},
    {"Drop which item?", "要丟棄哪件物品？"},
    {"You have nothing to drop.", "你沒有可以丟棄的物品。"},
    {"Inscription removed.", "已移除物品刻字。"},
    {"Hmmm, it seems to be stuck.", "似乎卡住了。"},
    {"Read which scroll? ", "要閱讀哪張卷軸？ "},
    {"Use which staff? ", "要使用哪根法杖？ "},
    {"That staff has no charges.", "這根法杖已沒有使用次數。"},
    {"Aim which wand? ", "要瞄準哪根魔杖？ "},
    {"That wand has no charges.", "這根魔杖已沒有使用次數。"},
    {"Zap which rod? ", "要使用哪根魔棒？ "},
    {"That rod is still charging.", "這根魔棒仍在充能。"},
    {"Activate which item? ", "要啟動哪件物品？ "},
    {"That item is still charging.", "這件物品仍在充能。"},
    {"You activate it.", "你啟動了它。"},
    {"Eat which food? ", "要吃哪種食物？ "},
    {"Quaff which potion? ", "要喝哪瓶藥水？ "},
    {"Use which item? ", "要使用哪件物品？ "},
    {"Equip the item to use it.", "請先裝備這件物品。"},
    {"The item cannot be used at the moment", "目前無法使用這件物品。"},
    {"You fuel your lamp.", "你為提燈補充了燃料。"},
    {"Your lamp is full.", "提燈的燃料已滿。"},
    {"You are not wielding a light.", "你沒有裝備光源。"},
    {"Your light cannot be refilled.", "目前的光源無法補充燃料。"},
    {"Refuel with which fuel source? ", "要使用哪種燃料補充光源？ "},
    {"Attempt it anyway? ", "仍要嘗試嗎？ "},
    {"There are no spells you can cast.", "目前沒有可施放的法術。"},
    {"That book has no spells that you can cast.", "這本書沒有可施放的法術。"},
    {"You cannot learn any new spells from the books you have.", "目前的書本沒有可以學習的新法術。"},
    {"That book has no spells that you can learn.", "這本書沒有可以學習的法術。"},
    {"Study which book? ", "要研讀哪本書？ "},
    /* FAangband town/wilderness and trap placement prompts. */
    {"You already live here!", "你已經住在這裡了！"},
    {"Your home will be here when you return.", "你回來時，家仍會在這裡。"},
    {"You can only move to another town!", "只能搬到另一座城鎮！"},
    {"This is a path to greater danger.", "這條路通往更危險的地方。"},
    {"This staircase leads down.", "這道樓梯通往下層。"},
    {"This level will never appear again.  Really leave?", "離開後就無法回到這一層。確定要離開嗎？"},
    {"This level will never appear again.  Really leave? ", "離開後就無法回到這一層。確定要離開嗎？ "},
    {"You trigger a magic portal.", "你啟動了一座魔法傳送門。"},
    {"You enter a winding path to less danger.", "你踏上了通往較安全地方的蜿蜒小徑。"},
    {"This is a path to less danger.", "這條路通往較安全的地方。"},
    {"This staircase leads up.", "這道樓梯通往上層。"},
    {"You enter a winding path to greater danger.", "你踏上了通往更危險地方的蜿蜒小徑。"},
    {"You can not see to set a trap.", "你看不清楚，無法設置陷阱。"},
    {"You are too confused.", "你太混亂了。"},
    {"You must disarm an existing trap to free up your equipment.", "必須先拆除一個已設置的陷阱，才能騰出器材。"},
    {"You set a monster trap.", "你設置了一個怪物陷阱。"},
    /* Tunnel-tool phrases; grammar is assembled in native FA cave messages. */
    {"with your hands", "徒手"},
    {"with your weapon", "用武器"},
    {"with your swap digger", "用替換的挖掘工具"}
};

int main(void)
{
    size_t i;
    unsigned checks = 0;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        const char *translated = angdroid_game_ui_text(cases[i].english, 1);
        const char *original = angdroid_game_ui_text(cases[i].english, 0);
        if (!translated || strcmp(translated, cases[i].chinese) ||
                original != cases[i].english) {
            fprintf(stderr, "Translation mismatch for entry %lu: %s\n",
                (unsigned long)i, cases[i].english);
            return 1;
        }
        checks += 2;
    }
    {
        const char *unknown = "unmapped FA item command";
        if (angdroid_game_ui_text(unknown, 1) != unknown ||
                angdroid_game_ui_text(NULL, 1) != NULL ||
                angdroid_game_ui_text(NULL, 0) != NULL) {
            fprintf(stderr, "Fallback mapping failed\n");
            return 1;
        }
        checks += 3;
    }
    printf("PASS: %u FA item/exploration UI bilingual dictionary/fallback checks\n", checks);
    return 0;
}
