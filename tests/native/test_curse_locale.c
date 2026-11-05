/* Native C99 curse.txt display names, descriptions, English fallback,
 * format string safety, negative strength and unknown custom curse. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-curse.h"

int main(void)
{
    int checks = 0;
    size_t i;
    static const struct { const char *en; const char *zh;
        const char *desc_en; const char *desc_zh; } cases[] = {
        { "vulnerability", "脆弱",
          "attracts opponents and weakens the defences", "吸引敵人並削弱防禦" },
        { "teleportation", "瞬間傳送",
          "randomly makes you teleport", "隨機使你瞬間傳送" },
        { "dullness", "遲鈍",
          "makes you mentally slow", "使你的思考變得遲鈍" },
        { "sickliness", "體弱",
          "makes you frail", "使你的身體變得虛弱" },
        { "enveloping", "束縛",
          "protects you at the expense of free movement", "限制自由行動以換取防護" },
        { "irritation", "激怒",
          "annoys monsters and makes it hard for you to hit them", "激怒怪物，並使你難以命中牠們" },
        { "weakness", "虛弱",
          "weakens your grasp", "削弱你的握力" },
        { "clumsiness", "笨拙",
          "gives you butterfingers", "使你笨手笨腳" },
        { "slowness", "遲緩",
          "drags at your feet", "拖慢你的腳步" },
        { "annoyance", "擾亂",
          "wakes up and annoys nearby monsters, and delays your escape", "吵醒並激怒附近的怪物，妨礙你逃脫" },
        { "poison", "中毒",
          "poisons you from time to time", "不時讓你中毒" },
        { "siren", "號角",
          "occasionally makes a loud noise", "偶爾發出巨大的聲響" },
        { "hallucination", "幻覺",
          "makes you hallucinate sometimes", "有時使你產生幻覺" },
        { "paralysis", "麻痺",
          "paralyses you every now and then", "不時讓你陷入麻痺" },
        { "dragon summon", "召喚巨龍",
          "sometimes summons a dragon", "有時會召喚巨龍" },
        { "demon summon", "召喚惡魔",
          "occasionally summons a demon", "偶爾召喚惡魔" },
        { "undead summon", "召喚不死生物",
          "occasionally summons an undead", "偶爾召喚不死生物" },
        { "impair mana recovery", "妨礙法力恢復",
          "makes you slow to recover mana", "減慢你的法力恢復速度" },
        { "impair hitpoint recovery", "妨礙生命恢復",
          "makes you slow to recover hitpoints", "減慢你的生命值恢復速度" },
        { "cowardice", "膽怯",
          "makes you too scared to fight", "讓你害怕得無法戰鬥" },
        { "stone", "石化皮膚",
          "periodically turns your skin to stone", "定期將你的皮膚變成石頭" },
        { "anti-teleportation", "禁止傳送",
          "prevents you from teleporting", "使你無法瞬間傳送" },
        { "treacherous weapon", "武器反噬",
          "makes your weapon attack you", "使你的武器反過來攻擊你" },
        { "burning up", "灼熱",
          "makes you vulnerable to fire attacks, but resistant to cold", "讓你更怕火焰攻擊，但對寒冷更有抵抗力" },
        { "chilled to the bone", "寒徹骨髓",
          "makes you vulnerable to cold attacks, but resistant to fire", "讓你更怕寒冷攻擊，但對火焰更有抵抗力" },
        { "steelskin", "鋼鐵皮膚",
          "makes your skin harder to damage, but conduct electricity", "使你的皮膚更難受傷，卻也會導電" },
        { "air swing", "揮空",
          "makes it hard to hit anything", "使你難以擊中任何目標" },
    };
    for (i = 0; i < sizeof(cases)/sizeof(cases[0]); ++i) {
        char row[192], english[192];
        assert(strcmp(angdroid_curse_name(cases[i].en, 1),
                      cases[i].zh) == 0);
        assert(strcmp(angdroid_curse_name(cases[i].en, 0),
                      cases[i].en) == 0);
        assert(strcmp(angdroid_curse_description(cases[i].desc_en, 1),
                      cases[i].desc_zh) == 0);
        assert(strcmp(angdroid_curse_description(cases[i].desc_en, 0),
                      cases[i].desc_en) == 0);
        angdroid_curse_row(row, sizeof(row), cases[i].en, 17, 1);
        angdroid_curse_row(english, sizeof(english),
                           cases[i].en, 17, 0);
        assert(strstr(row, cases[i].zh) != NULL);
        assert(strstr(row, "17") != NULL);
        assert(strstr(english, cases[i].en) != NULL);
        checks += 7;
    }
    assert(strcmp(angdroid_curse_name("unknown curse", 1),
                  "unknown curse") == 0);
    assert(strcmp(angdroid_curse_description("unknown effect", 1),
                  "unknown effect") == 0);
    assert(angdroid_curse_name(NULL, 1) == NULL);
    assert(angdroid_curse_description(NULL, 1) == NULL);
    checks += 4;
    {
        char small[8], full[128];
        char sentinel = '#';
        angdroid_curse_row(small, sizeof(small), "poison", -4, 0);
        assert(small[sizeof(small) - 1] == '\0');
        angdroid_curse_row(full, sizeof(full), "unknown", -4, 1);
        assert(strstr(full, "-4") != NULL);
        angdroid_curse_row(&sentinel, 0, "poison", 1, 1);
        assert(sentinel == '#');
        checks += 3;
    }
    printf("PASS: curse UI 27 names + 27 descriptions (%d checks)\n", checks);
    return 0;
}
