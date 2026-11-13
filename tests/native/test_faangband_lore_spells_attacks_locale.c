/*
 * Source-exact FAangband monster lore data: 116 spell descriptions,
 * 19 melee method descriptions, 26 melee effect descriptions.
 * Data overlaps are intentionally translated once in the scoped header.
 */
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/faangband/mon-lore-terms.h"
#include "app/src/main/cpp/faangband/mon-lore-display.h"

struct locale_pair { const char *english; const char *chinese; };
static const struct locale_pair expected[] = {
    {"shriek for help", "發出求援尖叫"},
    {"lash you if nearby", "在近距離以鞭子抽打你"},
    {"spit at you from a distance", "從遠處向你吐出液體"},
    {"fling pebbles", "投擲小石子"},
    {"sling lead shots", "以投石索投出鉛彈"},
    {"sling seeker shots", "以投石索發射尋敵彈"},
    {"fire small arrows", "射出小箭"},
    {"fire arrows", "射箭"},
    {"fire large arrows", "射出大箭"},
    {"fire seeker arrows", "射出尋敵箭"},
    {"fire small bolts", "射出小弩箭"},
    {"fire bolts", "射出弩箭"},
    {"fire seeker bolts", "射出尋敵弩箭"},
    {"acid", "酸液"},
    {"lightning", "閃電"},
    {"fire", "火焰"},
    {"frost", "霜寒"},
    {"poison", "毒素"},
    {"nether", "冥界能量"},
    {"light", "光芒"},
    {"darkness", "黑暗"},
    {"sound", "聲波"},
    {"chaos", "混沌"},
    {"disenchantment", "解除魔法"},
    {"nexus", "空間扭曲"},
    {"time", "時間能量"},
    {"inertia", "慣性力量"},
    {"gravity", "重力"},
    {"shards", "碎片"},
    {"plasma", "等離子"},
    {"ice", "冰霜"},
    {"storm", "暴風"},
    {"dragonfire", "龍焰"},
    {"force", "力場"},
    {"mana", "魔力"},
    {"throw boulders", "投擲巨石"},
    {"weave webs", "編織蛛網"},
    {"produce acid balls", "製造酸液球"},
    {"produce lightning balls", "製造閃電球"},
    {"produce fire balls", "製造火焰球"},
    {"produce firestorms", "製造火焰風暴"},
    {"produce balls of cold", "製造寒冰球"},
    {"produce blizzards", "製造暴風雪"},
    {"produce poison balls", "製造毒素球"},
    {"produce storms of poison", "製造毒素風暴"},
    {"produce shardstorms", "製造碎片風暴"},
    {"invoke storms of knives", "召喚刀刃風暴"},
    {"produce nether balls", "製造冥界能量球"},
    {"produce nether storms", "製造冥界風暴"},
    {"produce water balls", "製造水球"},
    {"invoke mana storms", "召喚魔力風暴"},
    {"cast orbs of draining", "施放能量吸取法球"},
    {"produce balls of darkness", "製造黑暗球"},
    {"invoke darkness storms", "召喚黑暗風暴"},
    {"produce balls of light", "製造光芒球"},
    {"invoke starbursts", "召喚星光爆發"},
    {"create storms", "製造風暴"},
    {"create tempests", "製造強烈暴風"},
    {"drain mana", "吸取魔力"},
    {"cause mind blasting", "造成心靈衝擊"},
    {"cause brain smashing", "摧毀心智"},
    {"cause light wounds", "造成輕傷"},
    {"cause medium wounds", "造成中度傷害"},
    {"cause serious wounds", "造成重傷"},
    {"cause critical wounds", "造成致命傷"},
    {"cause mortal wounds", "造成瀕死傷"},
    {"produce acid bolts", "發射酸液箭"},
    {"produce lightning bolts", "發射閃電箭"},
    {"produce fire bolts", "發射火焰箭"},
    {"produce frost bolts", "發射冰霜箭"},
    {"produce poison bolts", "發射毒素箭"},
    {"produce nether bolts", "發射冥界能量箭"},
    {"produce water bolts", "發射水箭"},
    {"produce mana bolts", "發射魔力箭"},
    {"produce plasma bolts", "發射等離子箭"},
    {"produce ice bolts", "發射寒冰箭"},
    {"produce magic missiles", "發射魔法飛彈"},
    {"shoot sparks of lightning", "射出閃電火花"},
    {"hurl lances of nether", "擲出冥界長矛"},
    {"shoot rays of death", "發射死亡射線"},
    {"create hunger", "使你飢餓"},
    {"terrify", "使你恐懼"},
    {"blind", "使你失明"},
    {"confuse", "使你混亂"},
    {"slow", "使你緩速"},
    {"paralyze", "使你麻痺"},
    {"haste-self", "加速自己"},
    {"heal-self", "治療自己"},
    {"heal kin", "治療同伴"},
    {"blink-self", "短距離瞬移"},
    {"teleport-self", "瞬移自身"},
    {"teleport to", "瞬移至目標處"},
    {"teleport toward", "向目標瞬移"},
    {"teleport away", "將目標傳送走"},
    {"teleport level", "將目標傳送至其他樓層"},
    {"create darkness", "製造黑暗"},
    {"create traps", "設置陷阱"},
    {"cause amnesia", "使你失憶"},
    {"change shape", "改變形態"},
    {"summon similar monsters", "召喚相似的怪物"},
    {"summon a monster", "召喚一隻怪物"},
    {"summon monsters", "召喚怪物"},
    {"summon animals", "召喚動物"},
    {"summon swamp creatures", "召喚沼澤生物"},
    {"summon spiders", "召喚蜘蛛"},
    {"summon ants", "召喚螞蟻"},
    {"summon hounds", "召喚獵犬"},
    {"summon hydras", "召喚多頭蛇"},
    {"summon an ainu", "召喚一位埃努"},
    {"summon a demon", "召喚一隻惡魔"},
    {"summon an undead", "召喚一隻不死怪物"},
    {"summon a dragon", "召喚一條龍"},
    {"summon greater demons", "召喚高等惡魔"},
    {"summon greater undead", "召喚高等不死怪物"},
    {"summon winged dragons", "召喚有翼巨龍"},
    {"summon uniques", "召喚獨特怪物"},
    {"hit", "擊打"},
    {"touch", "觸碰"},
    {"punch", "拳擊"},
    {"kick", "踢擊"},
    {"claw", "爪擊"},
    {"bite", "咬"},
    {"sting", "針刺"},
    {"butt", "頭撞"},
    {"crush", "壓碎"},
    {"engulf", "吞沒"},
    {"crawl on you", "爬到你身上"},
    {"drool on you", "對你流口水"},
    {"spit", "吐唾液"},
    {"gaze", "凝視"},
    {"wail", "哀號"},
    {"release spores", "釋放孢子"},
    {"beg", "乞討"},
    {"insult", "辱罵"},
    {"moan", "呻吟"},
    {"attack", "造成傷害"},
    {"disenchant", "解除魔法"},
    {"drain charges", "吸取充能"},
    {"steal gold", "偷走金幣"},
    {"steal items", "偷走物品"},
    {"eat your food", "吃掉你的食物"},
    {"absorb light", "吸收光芒"},
    {"shoot acid", "發射酸液"},
    {"electrify", "施加電擊"},
    {"burn", "灼燒"},
    {"freeze", "凍結"},
    {"reduce strength", "降低力量"},
    {"reduce intelligence", "降低智力"},
    {"reduce wisdom", "降低智慧"},
    {"reduce dexterity", "降低敏捷"},
    {"reduce constitution", "降低體質"},
    {"reduce all stats", "降低所有能力值"},
    {"shatter", "粉碎"},
    {"lower experience", "降低經驗值"},
    {"cause hallucinations", "造成幻覺"},
    {"inflict Black Breath", "施加黑暗吐息"},
};

static int checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    const char *unknown = "FA-mon-lore-never-translated";
    char sentence[256];
    CHECK(sizeof expected / sizeof expected[0] == 156);
    for (i = 0; i < sizeof expected / sizeof expected[0]; ++i) {
        CHECK(strcmp(angdroid_fa_lore_term(expected[i].english, true),
            expected[i].chinese) == 0);
        CHECK(angdroid_fa_lore_term(expected[i].english, false) ==
            expected[i].english);
    }
    CHECK(angdroid_fa_lore_term(unknown, true) == unknown);
    CHECK(angdroid_fa_lore_term(NULL, true) == NULL);
    CHECK(angdroid_fa_lore_term(NULL, false) == NULL);
    (void)snprintf(sentence, sizeof sentence, "牠可能吐出%s%s%s",
        angdroid_fa_lore_term("fire", true),
        angdroid_fa_lore_separator(1, 2, true),
        angdroid_fa_lore_term("frost", true));
    CHECK(strcmp(sentence, "牠可能吐出火焰或霜寒") == 0);
    (void)snprintf(sentence, sizeof sentence, "牠能%s，效果為%s（2d6，75%%）",
        angdroid_fa_lore_term("bite", true),
        angdroid_fa_lore_term("poison", true));
    CHECK(strcmp(sentence, "牠能咬，效果為毒素（2d6，75%）") == 0);
    CHECK(strcmp(angdroid_fa_lore_separator(2, 3, true), "或") == 0);
    printf("PASS: %d FAangband spell and melee lore locale checks\n", checks);
    return 0;
}
