/* This mapping is for presentation only, never game IDs or commands. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-ui.h"
#include "../../app/src/main/cpp/angband/zh-tw-combat.h"
#include "../../app/src/main/cpp/curses/unicode_display.h"

int main(void)
{
    const char *unknown = "Untranslated data";
    assert(strcmp(angdroid_game_ui_text("New game", 1), "建立新遊戲") == 0);
    assert(strcmp(angdroid_game_ui_text("Point-based", 1), "點數分配") == 0);
    assert(strcmp(angdroid_game_ui_text("['r' to reroll", 1), "[r 重擲") == 0);
    assert(strcmp(angdroid_game_ui_text(", 'p' for previous roll", 1),
                  "，p 上次") == 0);
    assert(strcmp(angdroid_game_ui_text(" or 'Enter' to accept]", 1),
                  "，Enter 確認]") == 0);
    assert(strcmp(angdroid_game_ui_text("Save and quit", 1), "儲存並離開") == 0);
    assert(strcmp(angdroid_game_ui_text("Inscribe an object", 1), "為物品刻字") == 0);
    assert(strcmp(angdroid_game_ui_text("Cast a spell", 1), "施放法術") == 0);
    assert(strcmp(angdroid_game_ui_text("Ignore an item", 1), "忽略物品") == 0);
    assert(strcmp(angdroid_game_ui_text("Walk Towards", 1), "走向此處") == 0);
    assert(strcmp(angdroid_game_ui_text("Use %s: %s", 1), "使用 %s：%s") == 0);
    assert(strcmp(angdroid_game_ui_text("Saving game... failed!", 1),
                  "遊戲儲存失敗！") == 0);
    /* The source Angband gamedata p_race.txt/class.txt is unchanged. */
    assert(strcmp(angdroid_game_ui_text("Human", 1), "人類") == 0);
    assert(strcmp(angdroid_game_ui_text("Half-Troll", 1), "半食人妖") == 0);
    assert(strcmp(angdroid_game_ui_text("Kobold", 1), "狗頭人") == 0);
    assert(strcmp(angdroid_game_ui_text("Warrior", 1), "戰士") == 0);
    assert(strcmp(angdroid_game_ui_text("Necromancer", 1), "死靈法師") == 0);
    assert(strcmp(angdroid_game_ui_text("Blackguard", 1), "黑衛士") == 0);
    assert(strcmp(angdroid_game_ui_text("Race", 1), "種族") == 0);
    assert(strcmp(angdroid_game_ui_text("Class", 1), "職業") == 0);
    assert(strcmp(angdroid_game_ui_text("Human", 0), "Human") == 0);
    assert(strcmp(angdroid_game_ui_text("Your Home", 1), "我的家") == 0);
    assert(strcmp(angdroid_game_ui_text("Store Inventory", 1), "商店物品") == 0);
    assert(strcmp(angdroid_game_ui_text("You have nothing in your inventory.", 1),
                  "背包裡沒有物品。") == 0);
    assert(strcmp(angdroid_game_ui_text("Saving failed.  Try again? ", 1),
                  "儲存失敗，要重試嗎？ ") == 0);
    assert(strcmp(angdroid_game_ui_text("Cancelled.", 1), "已取消。") == 0);
    assert(strcmp(angdroid_game_ui_text("[Press any key to continue]", 1),
                  "按任意鍵繼續") == 0);
    assert(strcmp(angdroid_game_ui_text("Buy", 1), "提高") == 0);
    assert(strcmp(angdroid_game_ui_text("Sell", 1), "降低") == 0);
    assert(strcmp(angdroid_game_ui_text("Your Home", 0), "Your Home") == 0);
    assert(strcmp(angdroid_game_ui_text("Target Aborted.", 1),
                  "已取消選擇目標。") == 0);
    assert(strcmp(angdroid_game_ui_text("Inven:", 1), "背包：") == 0);
    assert(strcmp(angdroid_game_ui_text("This item only", 1),
                  "只忽略這件物品") == 0);
    assert(strcmp(angdroid_game_ui_text("Target Aborted.", 0),
                  "Target Aborted.") == 0);
    assert(strcmp(angdroid_game_ui_text("You stumble!", 1),
                  "你踉蹌了一下！") == 0);
    assert(strcmp(angdroid_game_ui_text("You have nothing to fire with.", 1),
                  "你沒有可用來射擊的武器。") == 0);
    assert(strcmp(angdroid_game_ui_text("You stumble!", 0),
                  "You stumble!") == 0);
    assert(strcmp(angdroid_game_ui_text("There is a wall in the way!", 1),
                  "一面牆擋住了去路！") == 0);
    assert(strcmp(angdroid_game_ui_text("You have picked the lock.", 1),
                  "你成功撬開了鎖。") == 0);
    assert(strcmp(angdroid_game_ui_text("You clear the web.", 1),
                  "你清除了蜘蛛網。") == 0);
    assert(strcmp(angdroid_game_ui_text("You clear the web.", 0),
                  "You clear the web.") == 0);
    assert(strcmp(angdroid_game_ui_text("That wand has no charges.", 1),
                  "這根魔杖已沒有使用次數。") == 0);
    assert(strcmp(angdroid_game_ui_text("Attempt it anyway? ", 1),
                  "仍要嘗試嗎？ ") == 0);
    assert(strcmp(angdroid_game_ui_text("Inscribe which item?", 1),
                  "要為哪件物品刻字？") == 0);
    assert(strcmp(angdroid_game_ui_text("That wand has no charges.", 0),
                  "That wand has no charges.") == 0);
    assert(strcmp(angdroid_game_ui_text("You cannot browse that.", 1),
                  "無法瀏覽這本書。") == 0);
    assert(strcmp(angdroid_game_ui_text(" untried", 1), " 未使用") == 0);
    assert(strcmp(angdroid_game_ui_text("You cannot browse that.", 0),
                  "You cannot browse that.") == 0);
    assert(strcmp(angdroid_game_ui_text("New game", 0), "New game") == 0);
    assert(strcmp(angdroid_game_ui_text(unknown, 1), unknown) == 0);
    assert(angdroid_game_ui_text(NULL, 1) == NULL);
    assert(angdroid_utf8_display_columns("繁體中文") == 8);
    assert(strcmp(angdroid_melee_verb_zh_tw("hit"), "擊中了") == 0);
    assert(strcmp(angdroid_melee_verb_zh_tw("poison"),
                  "以毒素傷害了") == 0);
    assert(strcmp(angdroid_ranged_verb_zh_tw("poisons"),
                  "以毒素傷害了") == 0);
    assert(strcmp(angdroid_melee_verb_zh_tw("fiercely smite"),
                  "狠狠重擊了") == 0);
    assert(strcmp(angdroid_ranged_verb_zh_tw("pierces"), "貫穿了") == 0);
    assert(strcmp(angdroid_ranged_verb_zh_tw("freezes"), "冰凍了") == 0);
    assert(strcmp(angdroid_hit_quality_zh_tw("It was a great hit!"),
                  "這次攻擊非常有效！") == 0);
    assert(angdroid_melee_verb_zh_tw("unknown shape verb") == NULL);
    assert(angdroid_ranged_verb_zh_tw("unknown missile verb") == NULL);
    assert(angdroid_hit_quality_zh_tw("unknown result") == NULL);
    assert(angdroid_melee_verb_zh_tw(NULL) == NULL);
    assert(angdroid_utf8_display_columns("Half-Troll") == 10);
    assert(angdroid_utf8_display_columns("半食人妖") == 8);
    /* A narrow panel must not cut in the middle of a UTF-8 code point. */
    {
        char clipped[32] = "半食人妖";
        angdroid_utf8_clip_columns(clipped, 5);
        assert(strcmp(clipped, "半食") == 0);
        assert(angdroid_utf8_display_columns(clipped) == 4);
    }
    puts("PASS: Angband game UI Traditional Chinese labels");
    return 0;
}
