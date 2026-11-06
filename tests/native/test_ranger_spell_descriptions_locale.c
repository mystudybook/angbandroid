/* Ranger (11) canonical spell descriptions, including two Herbal Curing variants. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-spell-descriptions.h"
static int checks = 0;
static void check(const char *name, const char *english, const char *zh)
{
    assert(strcmp(angdroid_zh_tw_spell_description(name, english, 1),
        zh) == 0);
    assert(angdroid_zh_tw_spell_description(name, english, 0) == english);
    checks += 2;
}
int main(void)
{
    const char *ranger_herbal = "Cures all cuts, poison, stunning and black breath.";
    const char *druid_herbal = "Cures all cuts, poison, stunning and black breath, and leaves you well-nourished.";
    const char *unknown = "Updated official spell description.";
    check("Remove Hunger", "Magically renders you comfortably fed (but not satiated).", "以魔法讓你恢復到不飢餓、但尚未飽足的狀態。");
    check("Detect Life", "Detects all living monsters in the immediate area, for one turn only.", "偵測附近所有活著的怪物，效果僅持續一回合。");
    check("Herbal Curing", "Cures all cuts, poison, stunning and black breath.", "治癒所有割傷、中毒、暈眩與黑色吐息。");
    check("Resist Poison", "Grants you resistance to poison for 20+1d20 turns.", "獲得毒素抗性，持續 20+1d20 回合。");
    check("Turn Stone to Mud", "Produces a beam that destroys the first section of wall it hits, unless it is a permanent wall.  Each monster caught in the beam that is susceptible to rock remover takes 20+1d30 points of damage.", "發射一道光束，摧毀擊中的第一段非永久牆壁。光束路徑上會受到移除岩石效果影響的每隻怪物，將受到 20+1d30 點傷害。");
    check("Sense Surroundings", "Maps the nearby area.", "繪製附近區域的地圖。");
    check("Cover Tracks", "Prevents you from leaving scent for monsters to track you by for a level-dependent duration.", "在隨角色等級變化的一段時間內，不再留下可供怪物追蹤的氣味。");
    check("Create Arrows", "Allows the player to turn a staff into a stack of arrows, with quality depending on staff level and player level.", "將一根法杖轉化為一疊箭矢，箭矢品質取決於法杖等級及角色等級。");
    check("Haste Self", "Hastes you (+10 to speed) for a level-dependent duration.", "使自身速度提高 10 點，持續時間取決於角色等級。");
    check("Decoy", "Leaves a magical decoy that looks and sounds like the player.", "留下一個外觀與聲音都像玩家本人的魔法誘餌。");
    check("Brand Ammunition", "Brands one stack of ammunition with fire, cold, or poison (selected at random) and at the same time attempts to improve the to-hit bonus and the to-dam bonus of the same ammunition.  The spell has no effect if the ammunition is already branded, has a slay, is broken, or is cursed.", "為一疊彈藥隨機附加火焰、寒冷或毒素屬性，同時嘗試提高這疊彈藥的命中與傷害加值。若彈藥已有元素附魔、帶有對特定怪物的剋制效果、已損壞或受到詛咒，這個法術就不會生效。");
    assert(strcmp(angdroid_zh_tw_spell_description("Herbal Curing",
        ranger_herbal, 1), "治癒所有割傷、中毒、暈眩與黑色吐息。") == 0);
    assert(strcmp(angdroid_zh_tw_spell_description("Herbal Curing",
        druid_herbal, 1), "治癒所有割傷、中毒、暈眩與黑色吐息，並讓你處於營養充足的狀態。") == 0);
    assert(strcmp(angdroid_zh_tw_spell_description("Herbal Curing",
        ranger_herbal, 1), angdroid_zh_tw_spell_description("Herbal Curing",
        druid_herbal, 1)) != 0);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        ranger_herbal, 0) == ranger_herbal);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        unknown, 1) == unknown);
    assert(angdroid_zh_tw_spell_description("Unknown Ranger",
        ranger_herbal, 1) == ranger_herbal);
    assert(angdroid_zh_tw_spell_description(NULL,
        ranger_herbal, 1) == ranger_herbal);
    assert(angdroid_zh_tw_spell_description("Herbal Curing",
        NULL, 1) == NULL);
    checks += 8;
    printf("PASS: Ranger spell descriptions (%d checks)\n", checks);
    return 0;
}
