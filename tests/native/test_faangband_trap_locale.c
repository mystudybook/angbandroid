/* FAangband-only trap knowledge and both variants' activation messages. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-terrain-trap.h"
#include "../../app/src/main/cpp/angband/zh-tw-trap-activation-messages.h"
static unsigned checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)
int main(void)
{
    static const struct { const char *name, *translated, *lore, *lore_zh; } traps[] = {
        { "falling branch", "落枝陷阱",
          "A falling branch could be random, or an act of an enemy, animal or vegetable.", "可能是意外掉落，也可能是敵人、動物或植物造成的落枝。" },
        { "basic trap", "基礎怪物陷阱",
          "A basic monster trap.", "最基本的怪物陷阱。" },
        { "sturdy trap", "堅固怪物陷阱",
          "A monster trap that doesn't break easily.", "不容易損壞的怪物陷阱。" },
        { "netted trap", "捕網陷阱",
          "A net trap to hit flying monsters.", "用網子捕捉飛行怪物的陷阱。" },
        { "confusion trap", "混亂陷阱",
          "A trap to confuse monsters.", "使怪物陷入混亂的陷阱。" },
        { "poison trap", "毒霧怪物陷阱",
          "A monster trap that releases a cloud of poison.", "會釋放毒霧的怪物陷阱。" },
        { "immaterial trap", "無實體怪物陷阱",
          "A trap to hurt immaterial monsters.", "能傷害無實體怪物的陷阱。" },
        { "electric trap", "閃電怪物陷阱",
          "An electrified monster trap.", "帶有電流的怪物陷阱。" },
        { "explosive trap", "爆炸怪物陷阱",
          "A monster trap that creates an explosion.", "會引發爆炸的怪物陷阱。" },
        { "portal trap", "傳送門陷阱",
          "A trap that teleports monsters.", "會傳送怪物的陷阱。" },
        { "stasis trap", "停滯陷阱",
          "A trap that holds a monster in stasis.", "能讓怪物陷入停滯狀態的陷阱。" },
        { "drain life trap", "生命吸取陷阱",
          "A trap that hurts nearby living monsters.", "能傷害附近活物的陷阱。" },
        { "unmagic trap", "破魔陷阱",
          "A trap that disenchants monsters.", "能解除怪物魔法效果的陷阱。" },
        { "dispelling trap", "驅散陷阱",
          "A trap that hurts all monsters in sight.", "能傷害視野內所有怪物的陷阱。" },
        { "banishment trap", "放逐陷阱",
          "A trap that removes nearby like monsters.", "能將附近同類怪物移除的陷阱。" },
    };
    static const struct { const char *source, *translated; } messages[] = {
        { "You fall through a trap door!", "你掉進了一道活板門！" },
        { "You float gently down to the next level.", "你輕輕飄落到下一層。" },
        { "You fall into a pit!", "你跌進陷坑！" },
        { "You float gently to the bottom of the pit.", "你緩緩飄落到陷坑底部。" },
        { "You fall into a spiked pit!", "你跌進尖刺陷坑！" },
        { "You float gently to the bottom of the pit, neatly avoiding the spikes.", "你緩緩飄落到陷坑底部，巧妙避開了尖刺。" },
        { "You are impaled!", "尖刺刺穿了你的身體！" },
        { "You are impaled on poisonous spikes!", "你被帶毒的尖刺刺穿了！" },
        { "A new foe appears from nowhere!", "一名新敵人憑空出現！" },
        { "You are enveloped in a cloud of smoke!", "一團煙霧籠罩了你！" },
        { "A sense of dread washes over you...", "一陣恐懼感湧上心頭……" },
        { "You hear a dragon's call!", "你聽見龍的召喚聲！" },
        { "Demons appear!", "惡魔出現了！" },
        { "You hit a teleport trap!", "你觸發了傳送陷阱！" },
        { "You are enveloped in flames!", "烈焰籠罩著你！" },
        { "You are splashed with acid!", "酸液濺到了你身上！" },
        { "A small dart barely misses you.", "一支小飛鏢擦身而過。" },
        { "A small dart hits you!", "一支小飛鏢擊中了你！" },
        { "You are surrounded by a black gas!", "一團黑色氣體包圍了你！" },
        { "You are surrounded by a gas of scintillating colors!", "一團閃爍著各色光芒的氣體包圍了你！" },
        { "You are surrounded by a pungent green gas!", "一團刺鼻的綠色氣體包圍了你！" },
        { "You are surrounded by a strange white mist!", "一團奇異的白霧包圍了你！" },
        { "A high-pitched whistle blares through the dungeon!", "尖銳的哨音響徹地城！" },
        { "A horn blast sounds!", "一聲號角響起！" },
        { "The floor explodes around you!", "你周圍的地板爆炸了！" },
        { "The air seems to catch fire!", "周圍的空氣彷彿燃燒了起來！" },
        { "You feel pressure on your mind.", "你感覺到心靈受到壓迫。" },
        { "You shake your head and are back to normal.", "你搖了搖頭，恢復了正常。" },
        { "Your mind is blasted by psionic energy!", "你的心智遭到靈能衝擊！" },
        { "A rock falls on your head!", "一塊岩石砸在你的頭上！" },
        { "The ground shakes.", "地面震動了。" },
        { "A section of the ceiling falls in behind you!", "你身後的一部分洞頂塌了下來！" },
        { "The trap explodes, blasting the area clear!", "陷阱爆炸，將周圍區域炸得一片空曠！" },
        { "A blazing light descends on you!", "一道熾烈的光芒照向你！" },
        { "Everything starts to fade...", "周圍的一切開始逐漸消失……" },
        { "You feel your energies sap away.", "你感覺自己的精力正在流失。" },
        { "Blades whirl around you, slicing your skin!", "刀刃在你身邊旋轉，割破你的皮膚！" },
        { "You feel yourself turning to stone!", "你感覺自己正逐漸石化！" },
        { "A high-pitched whistle blares out!", "一聲尖銳的哨音響起！" },
        { "A branch hits you from above.", "一根樹枝從上方砸中了你。" },
        { "It was heavy!", "那根樹枝相當沉重！" },
    };
    size_t i;
    CHECK(sizeof traps / sizeof traps[0] == 15);
    CHECK(sizeof messages / sizeof messages[0] == 41);
    for (i = 0; i < sizeof traps / sizeof traps[0]; ++i) {
        CHECK(strcmp(angdroid_trap_name(traps[i].name, 1), traps[i].translated) == 0);
        CHECK(angdroid_trap_name(traps[i].name, 0) == traps[i].name);
        CHECK(strcmp(angdroid_trap_lore(traps[i].lore, 1), traps[i].lore_zh) == 0);
        CHECK(angdroid_trap_lore(traps[i].lore, 0) == traps[i].lore);
    }
    for (i = 0; i < sizeof messages / sizeof messages[0]; ++i) {
        CHECK(strcmp(angdroid_trap_activation_message(messages[i].source, 1),
                     messages[i].translated) == 0);
        CHECK(angdroid_trap_activation_message(messages[i].source, 0) == messages[i].source);
    }
    CHECK(strcmp(angdroid_trap_name("spiked pit", 1), "尖刺陷坑") == 0);
    CHECK(strcmp(angdroid_trap_lore("A hole dug to snare the unwary.", 1),
        "挖來困住疏忽者的陷坑。") == 0);
    CHECK(strcmp(angdroid_trap_activation_message("Not yet translated.", 1),
        "Not yet translated.") == 0);
    CHECK(strcmp(angdroid_trap_name("Future monster trap", 1),
        "Future monster trap") == 0);
    CHECK(angdroid_trap_activation_message(NULL, 1) == NULL);
    CHECK(angdroid_trap_name(NULL, 1) == NULL);
    CHECK(angdroid_trap_lore(NULL, 1) == NULL);
    printf("PASS: FAangband trap names, lore and activation messages (%u checks)\n", checks);
    return 0;
}
