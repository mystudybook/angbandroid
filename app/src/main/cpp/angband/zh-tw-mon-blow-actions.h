/* Source-matched, display-only Angband/Faangband monster blow action text.
 * Canonical blow_methods.txt and random effect selection remain untouched. */
#ifndef ANGDROID_ZH_TW_MON_BLOW_ACTIONS_H
#define ANGDROID_ZH_TW_MON_BLOW_ACTIONS_H
#include <stddef.h>
#include <string.h>

static inline const char *angdroid_mon_blow_action_template(
    const char *english, int zh_tw)
{
    static const struct { const char *en, *zh; } map[] = {
        { "hits {target}", "擊中了{target}" },
        { "touches {target}", "觸碰了{target}" },
        { "punches {target}", "揮拳打向{target}" },
        { "kicks {target}", "踢向{target}" },
        { "claws {target}", "用利爪抓向{target}" },
        { "bites {target}", "咬住了{target}" },
        { "stings {target}", "刺中了{target}" },
        { "butts {target}", "用頭撞向{target}" },
        { "crushes {target}", "猛烈擠壓了{target}" },
        { "engulfs {target}", "吞沒了{target}" },
        { "crawls on {target}", "爬到了{target}身上" },
        { "drools on {target}", "對{target}流口水" },
        { "spits on {target}", "向{target}吐口水" },
        { "gazes at {target}", "凝視著{target}" },
        { "wails at {target}", "對{target}嚎叫" },
        { "releases spores at {target}", "朝{target}釋放孢子" },
        { "begs {target} for money", "向{target}乞討金錢" },
        { "insults {target}!", "辱罵了{target}！" },
        { "insults {oftarget} mother!", "侮辱了{target}的母親！" },
        { "gives {target} the finger!", "對{target}比出中指！" },
        { "humiliates {target}!", "羞辱了{target}！" },
        { "defiles {target}!", "褻瀆了{target}！" },
        { "dances around {target}!", "繞著{target}跳起舞來！" },
        { "makes obscene gestures!", "做出下流的手勢！" },
        { "moons {target}!!!", "對{target}露出屁股！！！" },
        { "wants his mushrooms back", "想要拿回他的蘑菇" },
        { "tells {target} to get off his land", "叫{target}離開他的地盤" },
        { "looks for his dogs", "正在尋找他的狗" },
        { "says 'Did you kill my Fang?'", "問道：「你殺了我的 Fang 嗎？」" },
        { "asks 'Do you want to buy any mushrooms?'", "問道：「你要買蘑菇嗎？」" },
        { "seems sad about something", "似乎在為某件事難過" },
        { "asks if {target} {has} seen his dogs", "詢問{target}是否見過他的狗" },
        { "mumbles something about mushrooms", "喃喃說著有關蘑菇的事" },
        { "tells you it's too hard to cross rivers", "告訴你渡河太困難了" },
        { "worries about getting lost in the mountains", "擔心自己會在山裡迷路" },
        { "whines about the gang of novice paladins he met", "抱怨他遇到的一群見習聖騎士" },
        { "asks you the way to Nowhere Town", "向你打聽無處鎮的方向" },
        { "says he can hear someone imprisoned in the rock", "說他聽見有人被困在岩石裡" },
        { "wonders how a trapdoor can take you to a different dungeon", "想不通陷阱門怎麼會通向另一座地下城" },
        { "babbles about mirages in the swamp", "不停談論沼澤中的海市蜃樓" },
        { "theorises that time is standing still", "推測時間可能已經停止" },
        { "tries to give you an order for pizza", "試圖向你點一份披薩" },
        { "requests some help with map-reading", "請求協助閱讀地圖" },
        { "claims he can see through walls", "聲稱他可以看穿牆壁" },
        { "decries the standard of today's arrows", "批評如今箭矢的品質" },
        { "questions the presence of vorpal bunnies in Beleriand", "質疑貝烈瑞安德為何會有致命的兔子" },
        { "gasps that he's just drunk a wooden potion", "喘著氣說他剛喝下一瓶木頭藥水" },
        { "relates his problems with running in the right direction", "訴說自己總是跑錯方向的困擾" },
        { "testifies that some monsters speak in tongues", "證稱有些怪物會說奇怪的語言" },
        { "reports that some adventurers lie about their past", "說有些冒險者會謊報過去的經歷" },
        { "reminisces about bargaining with shopkeepers", "回憶起與店主討價還價的往事" },
        { "asserts that he has been cloned", "堅稱自己曾被複製" },
        { "maintains that vaults are really just big larders", "堅持說寶庫其實只是大型食品儲藏室" },
        { "alleges that patches of grass are really magical staircases", "聲稱草叢其實是魔法階梯" },
        { "calls Ironbark the Ent a stingy bastard", "罵樹人鐵皮是個小氣鬼" },
        { "declares that wilderness pathways dilate time", "宣稱荒野小徑會讓時間變慢" },
        { "observes that the dwarves of Khazad Dûm are getting bigger", "發現凱薩督姆的矮人越長越高大" },
        { "mutters that shopkeepers won't take no for an answer", "嘟噥著店主總是不肯接受拒絕" },
        { "wants to know why some doors are purely decorative", "想知道為什麼有些門只能當裝飾" },
        { "boggles at caps that look like gloves", "對看起來像手套的帽子大惑不解" },
        { "boasts of walking over lava unharmed", "吹噓自己能毫髮無傷地走過熔岩" },
        { "screams that there are monsters in the walls", "尖叫著說牆壁裡有怪物" },
        { "marvels that his memory seems to disappear with the sun", "驚訝自己的記憶似乎隨陽光消失" },
        { "rails against towns with forbidden exits", "怒斥那些禁止離開的城鎮" },
        { "relates his difficulties in stopping running", "訴說自己跑起來就停不下來的困擾" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}

static inline const char *angdroid_mon_blow_save_feedback(
    const char *english, int zh_tw)
{
    static const struct { const char *en, *zh; } map[] = {
        { "You stand your ground!", "你堅定地站穩腳步！" },
        { "You resist the effects!", "你抵抗了這次效果！" },
    };
    size_t i;
    if (!english || !zh_tw) return english;
    for (i = 0; i < sizeof(map) / sizeof(map[0]); ++i)
        if (strcmp(english, map[i].en) == 0) return map[i].zh;
    return english;
}
#endif
