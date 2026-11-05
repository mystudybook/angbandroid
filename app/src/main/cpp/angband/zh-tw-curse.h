/* Original Angband curse.txt: display-only UI translations.
 * Object curse names, indices, strengths, triggers and save data remain
 * canonical English. Unknown names/descriptions fall back unchanged. */
#ifndef ANGDROID_ZH_TW_CURSE_H
#define ANGDROID_ZH_TW_CURSE_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>

struct angdroid_curse_pair { const char *en; const char *zh; };

static inline const char *angdroid_curse_lookup(const char *en, int zh_tw,
    const struct angdroid_curse_pair *pairs, size_t count)
{
    size_t i;
    if (!en || !zh_tw) return en;
    for (i = 0; i < count; ++i)
        if (strcmp(en, pairs[i].en) == 0) return pairs[i].zh;
    return en;
}

static inline const char *angdroid_curse_name(const char *en, int zh_tw)
{
    static const struct angdroid_curse_pair names[] = {
        { "vulnerability", "脆弱" },
        { "teleportation", "瞬間傳送" },
        { "dullness", "遲鈍" },
        { "sickliness", "體弱" },
        { "enveloping", "束縛" },
        { "irritation", "激怒" },
        { "weakness", "虛弱" },
        { "clumsiness", "笨拙" },
        { "slowness", "遲緩" },
        { "annoyance", "擾亂" },
        { "poison", "中毒" },
        { "siren", "號角" },
        { "hallucination", "幻覺" },
        { "paralysis", "麻痺" },
        { "dragon summon", "召喚巨龍" },
        { "demon summon", "召喚惡魔" },
        { "undead summon", "召喚不死生物" },
        { "impair mana recovery", "妨礙法力恢復" },
        { "impair hitpoint recovery", "妨礙生命恢復" },
        { "cowardice", "膽怯" },
        { "stone", "石化皮膚" },
        { "anti-teleportation", "禁止傳送" },
        { "treacherous weapon", "武器反噬" },
        { "burning up", "灼熱" },
        { "chilled to the bone", "寒徹骨髓" },
        { "steelskin", "鋼鐵皮膚" },
        { "air swing", "揮空" },
    };
    return angdroid_curse_lookup(en, zh_tw, names,
        sizeof(names) / sizeof(names[0]));
}

static inline const char *angdroid_curse_description(const char *en, int zh_tw)
{
    static const struct angdroid_curse_pair descriptions[] = {
        { "attracts opponents and weakens the defences", "吸引敵人並削弱防禦" },
        { "randomly makes you teleport", "隨機使你瞬間傳送" },
        { "makes you mentally slow", "使你的思考變得遲鈍" },
        { "makes you frail", "使你的身體變得虛弱" },
        { "protects you at the expense of free movement", "限制自由行動以換取防護" },
        { "annoys monsters and makes it hard for you to hit them", "激怒怪物，並使你難以命中牠們" },
        { "weakens your grasp", "削弱你的握力" },
        { "gives you butterfingers", "使你笨手笨腳" },
        { "drags at your feet", "拖慢你的腳步" },
        { "wakes up and annoys nearby monsters, and delays your escape", "吵醒並激怒附近的怪物，妨礙你逃脫" },
        { "poisons you from time to time", "不時讓你中毒" },
        { "occasionally makes a loud noise", "偶爾發出巨大的聲響" },
        { "makes you hallucinate sometimes", "有時使你產生幻覺" },
        { "paralyses you every now and then", "不時讓你陷入麻痺" },
        { "sometimes summons a dragon", "有時會召喚巨龍" },
        { "occasionally summons a demon", "偶爾召喚惡魔" },
        { "occasionally summons an undead", "偶爾召喚不死生物" },
        { "makes you slow to recover mana", "減慢你的法力恢復速度" },
        { "makes you slow to recover hitpoints", "減慢你的生命值恢復速度" },
        { "makes you too scared to fight", "讓你害怕得無法戰鬥" },
        { "periodically turns your skin to stone", "定期將你的皮膚變成石頭" },
        { "prevents you from teleporting", "使你無法瞬間傳送" },
        { "makes your weapon attack you", "使你的武器反過來攻擊你" },
        { "makes you vulnerable to fire attacks, but resistant to cold", "讓你更怕火焰攻擊，但對寒冷更有抵抗力" },
        { "makes you vulnerable to cold attacks, but resistant to fire", "讓你更怕寒冷攻擊，但對火焰更有抵抗力" },
        { "makes your skin harder to damage, but conduct electricity", "使你的皮膚更難受傷，卻也會導電" },
        { "makes it hard to hit anything", "使你難以擊中任何目標" },
    };
    return angdroid_curse_lookup(en, zh_tw, descriptions,
        sizeof(descriptions) / sizeof(descriptions[0]));
}

/* Always interpret localized names as data, never a printf format string. */
static inline void angdroid_curse_row(char *out, size_t cap,
    const char *name, int power, int zh_tw)
{
    const char *display;
    if (!out || !cap) return;
    display = angdroid_curse_name(name, zh_tw);
    if (!display) display = "";
    if (zh_tw)
        snprintf(out, cap, "  %s（詛咒強度 %d）", display, power);
    else
        snprintf(out, cap, "  %s (curse strength %d)", display, power);
}
#endif
