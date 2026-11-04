#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-store-hints.h"
int main(void)
{
    assert(strcmp(angdroid_store_hint_text("Cure Critical Wounds potions will cure blindness and confusion; carry lots.", 1), "「治療重傷」藥水能解除失明和混亂，建議多帶幾瓶。") == 0);
    assert(strcmp(angdroid_store_hint_text("Cure Critical Wounds potions will cure blindness and confusion; carry lots.", 0), "Cure Critical Wounds potions will cure blindness and confusion; carry lots.") == 0);
    assert(strcmp(angdroid_store_hint_text("Staves can be used even when confused and blinded.", 1), "即使混亂或失明，仍然可以使用法杖。") == 0);
    assert(strcmp(angdroid_store_hint_text("Staves can be used even when confused and blinded.", 0), "Staves can be used even when confused and blinded.") == 0);
    assert(strcmp(angdroid_store_hint_text("Monsters that breathe fire, acid, electricity, or cold can deal big damage.", 1), "會噴吐火焰、強酸、閃電或寒氣的怪物可能造成大量傷害。") == 0);
    assert(strcmp(angdroid_store_hint_text("Monsters that breathe fire, acid, electricity, or cold can deal big damage.", 0), "Monsters that breathe fire, acid, electricity, or cold can deal big damage.") == 0);
    assert(strcmp(angdroid_store_hint_text("Resistances cut damage to 1/3rd for the four basic elements.", 1), "對四種基本元素的抗性，可將相關傷害降至約三分之一。") == 0);
    assert(strcmp(angdroid_store_hint_text("Resistances cut damage to 1/3rd for the four basic elements.", 0), "Resistances cut damage to 1/3rd for the four basic elements.") == 0);
    assert(strcmp(angdroid_store_hint_text("The more hitpoints a monster has, the more damage its breath will do.", 1), "怪物的生命值越高，吐息造成的傷害通常越大。") == 0);
    assert(strcmp(angdroid_store_hint_text("The more hitpoints a monster has, the more damage its breath will do.", 0), "The more hitpoints a monster has, the more damage its breath will do.") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Fire and a potion of the same name.", 1), "可同時使用抗火裝備和同名藥水來強化防護。") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Fire and a potion of the same name.", 0), "You can combine equipment of Resist Fire and a potion of the same name.") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Cold and a potion of the same name.", 1), "可同時使用抗寒裝備和同名藥水來強化防護。") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Cold and a potion of the same name.", 0), "You can combine equipment of Resist Cold and a potion of the same name.") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Poison and a potion of the same name.", 1), "可同時使用抗毒裝備和同名藥水來強化防護。") == 0);
    assert(strcmp(angdroid_store_hint_text("You can combine equipment of Resist Poison and a potion of the same name.", 0), "You can combine equipment of Resist Poison and a potion of the same name.") == 0);
    assert(strcmp(angdroid_store_hint_text("If you don't know what a monster can do, use a rod of probing on it.", 1), "不清楚怪物能力時，可以使用探查魔棒了解牠。") == 0);
    assert(strcmp(angdroid_store_hint_text("If you don't know what a monster can do, use a rod of probing on it.", 0), "If you don't know what a monster can do, use a rod of probing on it.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe items with the '{' command.", 1), "按 { 指令可為物品加上刻字。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe items with the '{' command.", 0), "Inscribe items with the '{' command.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!s\" to get confirmation before selling it.", 1), "刻上 !s，可在出售物品前要求確認。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!s\" to get confirmation before selling it.", 0), "Inscribe an item with \"!s\" to get confirmation before selling it.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!d\" to get confirmation before dropping it.", 1), "刻上 !d，可在丟棄物品前要求確認。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!d\" to get confirmation before dropping it.", 0), "Inscribe an item with \"!d\" to get confirmation before dropping it.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!k\" to get confirmation before destroying it.", 1), "刻上 !k，可在摧毀物品前要求確認。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!k\" to get confirmation before destroying it.", 0), "Inscribe an item with \"!k\" to get confirmation before destroying it.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!v\" to get confirmation before throwing it.", 1), "刻上 !v，可在投擲物品前要求確認。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!v\" to get confirmation before throwing it.", 0), "Inscribe an item with \"!v\" to get confirmation before throwing it.") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!*\" to get confirmation before doing anything to it.", 1), "刻上 !*，對物品進行任何操作前都會要求確認。") == 0);
    assert(strcmp(angdroid_store_hint_text("Inscribe an item with \"!*\" to get confirmation before doing anything to it.", 0), "Inscribe an item with \"!*\" to get confirmation before doing anything to it.") == 0);
    assert(strcmp(angdroid_store_hint_text("You can use the 'I' command to inspect items and get details on them.", 1), "按 I 可查看物品的詳細資訊。") == 0);
    assert(strcmp(angdroid_store_hint_text("You can use the 'I' command to inspect items and get details on them.", 0), "You can use the 'I' command to inspect items and get details on them.") == 0);
    assert(strcmp(angdroid_store_hint_text("Use '=' to check out the options and customize your game.", 1), "按 = 可開啟選項並調整遊戲設定。") == 0);
    assert(strcmp(angdroid_store_hint_text("Use '=' to check out the options and customize your game.", 0), "Use '=' to check out the options and customize your game.") == 0);
    assert(strcmp(angdroid_store_hint_text("Be sure to set your hitpoint warning in the options menu ('=').", 1), "別忘了在 = 選項中設定低生命值警告。") == 0);
    assert(strcmp(angdroid_store_hint_text("Be sure to set your hitpoint warning in the options menu ('=').", 0), "Be sure to set your hitpoint warning in the options menu ('=').") == 0);
    assert(strcmp(angdroid_store_hint_text("Flasks of oil make a good throwing weapon early on.", 1), "遊戲初期，油瓶也是不錯的投擲武器。") == 0);
    assert(strcmp(angdroid_store_hint_text("Flasks of oil make a good throwing weapon early on.", 0), "Flasks of oil make a good throwing weapon early on.") == 0);
    assert(strcmp(angdroid_store_hint_text("Word of Recall will send you to and from the dungeon.", 1), "「召回術」可讓你往返地城與城鎮。") == 0);
    assert(strcmp(angdroid_store_hint_text("Word of Recall will send you to and from the dungeon.", 0), "Word of Recall will send you to and from the dungeon.") == 0);
    {
        const char *other = "Untranslated random hint";
        assert(angdroid_store_hint_text(other, 1) == other);
        assert(angdroid_store_hint_text(NULL, 1) == NULL);
    }
    puts("PASS: 20 canonical hint translations, English fallback, null/unknown");
    return 0;
}
