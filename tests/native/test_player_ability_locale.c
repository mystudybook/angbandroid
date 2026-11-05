/* Native C99 bilingual player ability names and long descriptions. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-player-ability.h"

int main(void)
{
    int checks = 0;
    size_t i;
    static const struct { const char *en; const char *zh;
        const char *desc; const char *translated; } cases[] = {
        { "Rock", "岩石之軀",
          "You are made of rock.", "你的身體由岩石構成。" },
        { "Mushroom Lore", "蘑菇知識",
          "You easily recognize mushrooms.", "你可以輕易辨識蘑菇。" },
        { "Device Lore", "裝置知識",
          "You easily recognize magic devices.", "你可以輕易辨識魔法裝置。" },
        { "Miner", "礦脈感知",
          "You can sense ore in the walls.", "你能感知牆壁裡的礦石。" },
        { "Extra Shots", "額外射擊",
          "Your shooting speed improves with tension bows every 10 levels.", "使用張力弓時，每提升 10 級，射擊速度就會提高。" },
        { "Relentless [30]", "不屈意志 [30]",
          "You become immune to fear at level 30.", "達到 30 級時，你會免疫恐懼。" },
        { "Blessed Fighter", "祝福戰士",
          "You get improved melee with blessed or hafted weapons.", "使用受祝福的武器或鈍擊武器時，近戰能力提高。" },
        { "Full Spellcaster", "完美施法者",
          "You may obtain a perfect success rate with magic.", "你的法術有機會達到完全不失敗的成功率。" },
        { "Extra Spell Beaming", "法術光束強化",
          "Your bolt spells frequently turn into beams.", "你的飛彈法術更容易轉變為光束。" },
        { "Spell Choice", "自選法術",
          "You may choose your own spells to study.", "你可以自行選擇要研讀的法術。" },
        { "No Magic", "無法施法",
          "You cannot cast spells.", "你無法施放法術。" },
        { "Charms", "魅惑",
          "You are extra persuasive to monsters.", "你對怪物有更強的說服力。" },
        { "Unlight", "暗影親和",
          "You gain stealth in, can see in, and resist the dark.", "你在黑暗中更隱密，能在黑暗中看清事物，並具有黑暗抗性。" },
        { "Pickpocketing", "扒竊",
          "You can steal from monsters.", "你可以從怪物身上偷取物品。" },
        { "Shield Bash", "盾擊",
          "You can bash monsters with a shield in melee.", "你可以在近戰中用盾牌撞擊怪物。" },
        { "Evil", "邪惡",
          "You resist nether, but are hurt by holy attacks.", "你能抵抗冥界能量，但會受到神聖攻擊的傷害。" },
        { "Combat Regeneration", "戰鬥再生",
          "You draw power from the thrill of combat, represented by Spell Points (SP).  You gain SP when damaged by an enemy or when you attack in melee.  As your blood cools your power fades; you lose SP at half the rate other classes gain them.  When you spend SP by casting a spell you regain some health, and fading SP replenish your health more efficiently.  The more damaged you are, the bigger these health gains will be.", "戰鬥的刺激會賦予你力量，以法術點數（SP）表示。敵人使你受傷，或你在近戰中攻擊時，就能獲得 SP。戰鬥逐漸平息時，SP 會消退，消退速度為其他職業獲得 SP 速度的一半。施放法術消耗 SP 時，你會恢復部分生命值；逐漸消退的 SP 能更有效地恢復生命值。你受傷越重，恢復的生命值越多。" },
        { "Sustain Strength", "維持力量",
          "Your strength is sustained.", "你的力量受到維持效果保護。" },
        { "Sustain Intelligence", "維持智力",
          "Your intelligence is sustained.", "你的智力受到維持效果保護。" },
        { "Sustain Wisdom", "維持智慧",
          "Your wisdom is sustained.", "你的智慧受到維持效果保護。" },
        { "Sustain Dexterity", "維持敏捷",
          "Your dexterity is sustained.", "你的敏捷受到維持效果保護。" },
        { "Sustain Constitution", "維持體質",
          "Your constitution is sustained.", "你的體質受到維持效果保護。" },
        { "Fear Immunity", "免疫恐懼",
          "You are immune to fear.", "你免疫恐懼。" },
        { "Blindness Immunity", "免疫致盲",
          "You cannot be blinded.", "你不會被致盲。" },
        { "Confusion Immunity", "免疫混亂",
          "You cannot be confused.", "你不會陷入混亂。" },
        { "Stun Immunity", "免疫震懾",
          "You cannot be stunned.", "你不會被震懾。" },
        { "Slow Digestion", "消化緩慢",
          "You have slow metabolism.", "你的新陳代謝較慢。" },
        { "Feather Falling", "緩慢墜落",
          "You fall lightly.", "你會輕緩地落下。" },
        { "Regeneration", "生命再生",
          "You regenerate quickly.", "你能快速恢復生命值。" },
        { "Telepathy", "心靈感應",
          "You have telepathy.", "你擁有心靈感應能力。" },
        { "See Invisible", "看見隱形",
          "You can see invisible creatures.", "你可以看見隱形生物。" },
        { "Free Action", "自由行動",
          "You are immune to paralysis.", "你免疫麻痺。" },
        { "Hold Life", "維持生命力",
          "Your life force is sustained.", "你的生命力受到維持效果保護。" },
        { "Earthquakes", "地震攻擊",
          "You sometimes create earthquakes on impact.", "你在擊中目標時，有時會引發地震。" },
        { "Slow Regeneration", "生命恢復減慢",
          "Your hitpoint recovery is impaired.", "你的生命值恢復速度降低。" },
        { "Slow Mana Regeneration", "法力恢復減慢",
          "Your mana recovery is impaired.", "你的法力恢復速度降低。" },
        { "Constant Fear", "持續恐懼",
          "You are afraid of melee, and bad at shooting and casting spells.", "你害怕近戰，且不擅長射擊和施法。" },
        { "Teleport Ban", "禁止傳送",
          "You cannot teleport.", "你無法瞬間傳送。" },
        { "Aggravate", "激怒怪物",
          "You aggravate creatures nearby.", "你會激怒附近的生物。" },
        { "Experience Drain", "經驗值流失",
          "Your experience constantly drains away.", "你的經驗值會持續流失。" },
        { "Trap Immune", "免疫陷阱",
          "You are immune to traps.", "你免疫陷阱。" },
        { "Resistance", "抗性",
          "You resist", "你具有以下抗性：" },
        { "Immunity", "免疫",
          "You are immune to", "你免疫：" },
        { "Vulnerability", "弱點",
          "You are vulnerable to", "你容易受到以下傷害：" },
    };
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); ++i) {
        assert(strcmp(angdroid_ability_name(cases[i].en, 1),
                      cases[i].zh) == 0);
        assert(strcmp(angdroid_ability_name(cases[i].en, 0),
                      cases[i].en) == 0);
        assert(strcmp(angdroid_ability_description(cases[i].desc, 1),
                      cases[i].translated) == 0);
        assert(strcmp(angdroid_ability_description(cases[i].desc, 0),
                      cases[i].desc) == 0);
        checks += 4;
    }
    assert(angdroid_ability_name(NULL, 1) == NULL);
    assert(angdroid_ability_description(NULL, 1) == NULL);
    assert(strcmp(angdroid_ability_name("Custom Ability", 1),
                  "Custom Ability") == 0);
    assert(strcmp(angdroid_ability_description("Custom description.", 1),
                  "Custom description.") == 0);
    checks += 4;
    printf("PASS: 44 player-property names/descriptions (%d checks)\n", checks);
    return 0;
}
