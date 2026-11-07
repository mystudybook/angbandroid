/* C99 regression covering every canonical monster blow template and save. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-mon-blow-actions.h"
static int checks;

/* UTF-8 boundary tests: length is always bytes, never glyphs. */
static void verify_utf8_append(void)
{
    char buf[12] = "";
    size_t used = 0;
    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "你", strlen("你"));
    assert(used == strlen("你") && strcmp(buf, "你") == 0);
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "a🐱", strlen("a🐱"));
    assert(strcmp(buf, "你a🐱") == 0 && used == strlen("你a🐱"));
    checks += 1;

    /* A 3-byte glyph must not be partially written into only 2 bytes. */
    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "界", strlen("界"));
    assert(strcmp(buf, "你a🐱") == 0 && used == strlen("你a🐱"));
    checks += 1;

    /* Prevent a truncated UTF-8 source slice from leaking a partial glyph. */
    used = 0;
    buf[0] = '\0';
    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "中文", 2);
    assert(used == 0 && buf[0] == '\0');
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "中文", 5);
    assert(used == 3 && strcmp(buf, "中") == 0);
    checks += 1;

    /* Invalid leading and continuation bytes never enter output. */
    used = 0;
    buf[0] = '\0';
    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "\x80", 1);
    assert(used == 0 && buf[0] == '\0');
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "\xE4\x41\x80", 3);
    assert(used == 0 && buf[0] == '\0');
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "\xED\xA0\x80", 3);
    assert(used == 0 && buf[0] == '\0');
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "\xF4\x90\x80\x80", 4);
    assert(used == 0 && buf[0] == '\0');
    checks += 1;

    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "ok", 2);
    assert(used == 2 && strcmp(buf, "ok") == 0);
    checks += 1;

    /* Never write past the end, even if the caller's offset is full. */
    used = sizeof(buf);
    angdroid_mon_blow_append_utf8(buf, sizeof(buf), &used, "你", 3);
    assert(used == sizeof(buf));
    checks += 1;
}

static void verify(const char *(*translate)(const char *, int),
    const char *en, const char *zh)
{
    assert(strcmp(translate(en, 1), zh) == 0);
    assert(translate(en, 0) == en);
    checks += 2;
}
int main(void)
{
    verify_utf8_append();
    verify(angdroid_mon_blow_action_template, "hits {target}", "擊中了{target}");
    verify(angdroid_mon_blow_action_template, "touches {target}", "觸碰了{target}");
    verify(angdroid_mon_blow_action_template, "punches {target}", "揮拳打向{target}");
    verify(angdroid_mon_blow_action_template, "kicks {target}", "踢向{target}");
    verify(angdroid_mon_blow_action_template, "claws {target}", "用利爪抓向{target}");
    verify(angdroid_mon_blow_action_template, "bites {target}", "咬住了{target}");
    verify(angdroid_mon_blow_action_template, "stings {target}", "刺中了{target}");
    verify(angdroid_mon_blow_action_template, "butts {target}", "用頭撞向{target}");
    verify(angdroid_mon_blow_action_template, "crushes {target}", "猛烈擠壓了{target}");
    verify(angdroid_mon_blow_action_template, "engulfs {target}", "吞沒了{target}");
    verify(angdroid_mon_blow_action_template, "crawls on {target}", "爬到了{target}身上");
    verify(angdroid_mon_blow_action_template, "drools on {target}", "對{target}流口水");
    verify(angdroid_mon_blow_action_template, "spits on {target}", "向{target}吐口水");
    verify(angdroid_mon_blow_action_template, "gazes at {target}", "凝視著{target}");
    verify(angdroid_mon_blow_action_template, "wails at {target}", "對{target}嚎叫");
    verify(angdroid_mon_blow_action_template, "releases spores at {target}", "朝{target}釋放孢子");
    verify(angdroid_mon_blow_action_template, "begs {target} for money", "向{target}乞討金錢");
    verify(angdroid_mon_blow_action_template, "insults {target}!", "辱罵了{target}！");
    verify(angdroid_mon_blow_action_template, "insults {oftarget} mother!", "侮辱了{target}的母親！");
    verify(angdroid_mon_blow_action_template, "gives {target} the finger!", "對{target}比出中指！");
    verify(angdroid_mon_blow_action_template, "humiliates {target}!", "羞辱了{target}！");
    verify(angdroid_mon_blow_action_template, "defiles {target}!", "褻瀆了{target}！");
    verify(angdroid_mon_blow_action_template, "dances around {target}!", "繞著{target}跳起舞來！");
    verify(angdroid_mon_blow_action_template, "makes obscene gestures!", "做出下流的手勢！");
    verify(angdroid_mon_blow_action_template, "moons {target}!!!", "對{target}露出屁股！！！");
    verify(angdroid_mon_blow_action_template, "wants his mushrooms back", "想要拿回他的蘑菇");
    verify(angdroid_mon_blow_action_template, "tells {target} to get off his land", "叫{target}離開他的地盤");
    verify(angdroid_mon_blow_action_template, "looks for his dogs", "正在尋找他的狗");
    verify(angdroid_mon_blow_action_template, "says 'Did you kill my Fang?'", "問道：「你殺了我的 Fang 嗎？」");
    verify(angdroid_mon_blow_action_template, "asks 'Do you want to buy any mushrooms?'", "問道：「你要買蘑菇嗎？」");
    verify(angdroid_mon_blow_action_template, "seems sad about something", "似乎在為某件事難過");
    verify(angdroid_mon_blow_action_template, "asks if {target} {has} seen his dogs", "詢問{target}是否見過他的狗");
    verify(angdroid_mon_blow_action_template, "mumbles something about mushrooms", "喃喃說著有關蘑菇的事");
    verify(angdroid_mon_blow_action_template, "tells you it's too hard to cross rivers", "告訴你渡河太困難了");
    verify(angdroid_mon_blow_action_template, "worries about getting lost in the mountains", "擔心自己會在山裡迷路");
    verify(angdroid_mon_blow_action_template, "whines about the gang of novice paladins he met", "抱怨他遇到的一群見習聖騎士");
    verify(angdroid_mon_blow_action_template, "asks you the way to Nowhere Town", "向你打聽無處鎮的方向");
    verify(angdroid_mon_blow_action_template, "says he can hear someone imprisoned in the rock", "說他聽見有人被困在岩石裡");
    verify(angdroid_mon_blow_action_template, "wonders how a trapdoor can take you to a different dungeon", "想不通陷阱門怎麼會通向另一座地下城");
    verify(angdroid_mon_blow_action_template, "babbles about mirages in the swamp", "不停談論沼澤中的海市蜃樓");
    verify(angdroid_mon_blow_action_template, "theorises that time is standing still", "推測時間可能已經停止");
    verify(angdroid_mon_blow_action_template, "tries to give you an order for pizza", "試圖向你點一份披薩");
    verify(angdroid_mon_blow_action_template, "requests some help with map-reading", "請求協助閱讀地圖");
    verify(angdroid_mon_blow_action_template, "claims he can see through walls", "聲稱他可以看穿牆壁");
    verify(angdroid_mon_blow_action_template, "decries the standard of today's arrows", "批評如今箭矢的品質");
    verify(angdroid_mon_blow_action_template, "questions the presence of vorpal bunnies in Beleriand", "質疑貝烈瑞安德為何會有致命的兔子");
    verify(angdroid_mon_blow_action_template, "gasps that he's just drunk a wooden potion", "喘著氣說他剛喝下一瓶木頭藥水");
    verify(angdroid_mon_blow_action_template, "relates his problems with running in the right direction", "訴說自己總是跑錯方向的困擾");
    verify(angdroid_mon_blow_action_template, "testifies that some monsters speak in tongues", "證稱有些怪物會說奇怪的語言");
    verify(angdroid_mon_blow_action_template, "reports that some adventurers lie about their past", "說有些冒險者會謊報過去的經歷");
    verify(angdroid_mon_blow_action_template, "reminisces about bargaining with shopkeepers", "回憶起與店主討價還價的往事");
    verify(angdroid_mon_blow_action_template, "asserts that he has been cloned", "堅稱自己曾被複製");
    verify(angdroid_mon_blow_action_template, "maintains that vaults are really just big larders", "堅持說寶庫其實只是大型食品儲藏室");
    verify(angdroid_mon_blow_action_template, "alleges that patches of grass are really magical staircases", "聲稱草叢其實是魔法階梯");
    verify(angdroid_mon_blow_action_template, "calls Ironbark the Ent a stingy bastard", "罵樹人鐵皮是個小氣鬼");
    verify(angdroid_mon_blow_action_template, "declares that wilderness pathways dilate time", "宣稱荒野小徑會讓時間變慢");
    verify(angdroid_mon_blow_action_template, "observes that the dwarves of Khazad Dûm are getting bigger", "發現凱薩督姆的矮人越長越高大");
    verify(angdroid_mon_blow_action_template, "mutters that shopkeepers won't take no for an answer", "嘟噥著店主總是不肯接受拒絕");
    verify(angdroid_mon_blow_action_template, "wants to know why some doors are purely decorative", "想知道為什麼有些門只能當裝飾");
    verify(angdroid_mon_blow_action_template, "boggles at caps that look like gloves", "對看起來像手套的帽子大惑不解");
    verify(angdroid_mon_blow_action_template, "boasts of walking over lava unharmed", "吹噓自己能毫髮無傷地走過熔岩");
    verify(angdroid_mon_blow_action_template, "screams that there are monsters in the walls", "尖叫著說牆壁裡有怪物");
    verify(angdroid_mon_blow_action_template, "marvels that his memory seems to disappear with the sun", "驚訝自己的記憶似乎隨陽光消失");
    verify(angdroid_mon_blow_action_template, "rails against towns with forbidden exits", "怒斥那些禁止離開的城鎮");
    verify(angdroid_mon_blow_action_template, "relates his difficulties in stopping running", "訴說自己跑起來就停不下來的困擾");
    verify(angdroid_mon_blow_save_feedback, "You stand your ground!", "你堅定地站穩腳步！");
    verify(angdroid_mon_blow_save_feedback, "You resist the effects!", "你抵抗了這次效果！");
    assert(strcmp(angdroid_mon_blow_action_template("future blow", 1), "future blow") == 0);
    assert(angdroid_mon_blow_action_template(NULL, 1) == NULL);
    assert(strcmp(angdroid_mon_blow_save_feedback("future save", 1), "future save") == 0);
    assert(angdroid_mon_blow_save_feedback(NULL, 1) == NULL);
    checks += 4;
    printf("PASS: monster blow action and saving throw (%d checks)\n", checks);
    return 0;
}
