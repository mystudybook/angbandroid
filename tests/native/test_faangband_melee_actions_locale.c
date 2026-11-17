/*
 * FAangband 57 blow_methods.txt templates and two timed-effect save notices.
 * Compile against the production zh-tw-ui.h dictionary in strict C99 mode.
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "app/src/main/cpp/angband/zh-tw-ui.h"

struct translation_case { const char *english; const char *chinese; };
static const struct translation_case translations[] = {
    {"hits {target}", "擊中了{target}"},
    {"touches {target}", "碰觸了{target}"},
    {"punches {target}", "揮拳打向{target}"},
    {"kicks {target}", "踢了{target}"},
    {"claws {target}", "用利爪抓向{target}"},
    {"bites {target}", "咬了{target}"},
    {"stings {target}", "螫了{target}"},
    {"butts {target}", "用頭頂撞了{target}"},
    {"crushes {target}", "狠狠壓向{target}"},
    {"engulfs {target}", "吞沒了{target}"},
    {"crawls on {target}", "爬上了{target}"},
    {"drools on {target}", "對{target}流口水"},
    {"spits on {target}", "朝{target}吐口水"},
    {"gazes at {target}", "凝視著{target}"},
    {"wails at {target}", "對{target}發出哀號"},
    {"releases spores at {target}", "朝{target}釋放孢子"},
    {"begs {target} for money", "向{target}乞討金錢"},
    {"insults {target}!", "辱罵了{target}！"},
    {"insults {oftarget} mother!", "辱罵了{oftarget}母親！"},
    {"gives {target} the finger!", "對{target}比出中指！"},
    {"humiliates {target}!", "羞辱了{target}！"},
    {"defiles {target}!", "褻瀆了{target}！"},
    {"dances around {target}!", "繞著{target}跳舞！"},
    {"makes obscene gestures!", "做出猥褻手勢！"},
    {"moons {target}!!!", "對{target}露出屁股！！！"},
    {"tells you it's too hard to cross rivers", "抱怨過河實在太難"},
    {"worries about getting lost in the mountains", "擔心自己會迷失在群山之間"},
    {"whines about the gang of novice paladins he met", "抱怨自己遇上的那群見習聖騎士"},
    {"asks you the way to Nowhere Town", "向你打聽無名鎮的方向"},
    {"says he can hear someone imprisoned in the rock", "說他聽見有人被困在岩石裡"},
    {"wonders how a trapdoor can take you to a different dungeon", "不明白活板門怎麼會通往另一座地城"},
    {"babbles about mirages in the swamp", "喋喋不休地談起沼澤中的幻影"},
    {"theorises that time is standing still", "推測時間可能已經停滯"},
    {"tries to give you an order for pizza", "試著向你訂購披薩"},
    {"requests some help with map-reading", "請你幫忙看懂地圖"},
    {"claims he can see through walls", "聲稱自己能透視牆壁"},
    {"decries the standard of today's arrows", "抱怨現在的箭矢品質太差"},
    {"questions the presence of vorpal bunnies in Beleriand", "質疑貝烈瑞安德為何會有斬首兔"},
    {"gasps that he's just drunk a wooden potion", "驚呼自己剛喝下一瓶木製藥水"},
    {"relates his problems with running in the right direction", "訴說自己老是跑錯方向的煩惱"},
    {"testifies that some monsters speak in tongues", "堅稱有些怪物會說奇怪的語言"},
    {"reports that some adventurers lie about their past", "指出有些冒險者會捏造自己的過去"},
    {"reminisces about bargaining with shopkeepers", "回憶起與店主討價還價的往事"},
    {"asserts that he has been cloned", "聲稱自己遭到複製"},
    {"maintains that vaults are really just big larders", "堅稱寶庫其實只是大型儲藏室"},
    {"alleges that patches of grass are really magical staircases", "宣稱草地其實是魔法樓梯"},
    {"calls Ironbark the Ent a stingy bastard", "罵樹人鐵皮是個小氣鬼"},
    {"declares that wilderness pathways dilate time", "斷言荒野小徑會使時間變慢"},
    {"observes that the dwarves of Khazad Dûm are getting bigger", "發現卡扎督姆的矮人越長越高"},
    {"mutters that shopkeepers won't take no for an answer", "嘟囔店主根本不接受拒絕"},
    {"wants to know why some doors are purely decorative", "想知道為何有些門只是裝飾"},
    {"boggles at caps that look like gloves", "對看起來像手套的帽子感到困惑"},
    {"boasts of walking over lava unharmed", "吹噓自己能毫髮無傷地走過熔岩"},
    {"screams that there are monsters in the walls", "尖叫說牆裡藏著怪物"},
    {"marvels that his memory seems to disappear with the sun", "驚訝自己的記憶似乎隨日落而消失"},
    {"rails against towns with forbidden exits", "怒斥那些禁止出城的城鎮"},
    {"relates his difficulties in stopping running", "講述自己停不下腳步的困境"},
    {"You stand your ground!", "你挺身站穩，毫不退縮！"},
    {"You resist the effects!", "你抵抗了這股影響！"},
};

static int checks;
#define CHECK(expr) do { if (!(expr)) { \
    fprintf(stderr, "FAIL at line %d: %s\n", __LINE__, #expr); \
    return 1; } ++checks; } while (0)

int main(void)
{
    size_t i;
    const char *unknown = "unknown-monster-blow-template";
    char out[256];

    for (i = 0; i < sizeof(translations) / sizeof(translations[0]); ++i) {
        CHECK(strcmp(angdroid_game_ui_text(translations[i].english, 1),
            translations[i].chinese) == 0);
        CHECK(angdroid_game_ui_text(translations[i].english, 0) ==
            translations[i].english);
    }
    CHECK(angdroid_game_ui_text(unknown, 1) == unknown);
    CHECK(angdroid_game_ui_text(NULL, 1) == NULL);

    /* Chinese templates keep {target} for runtime name substitution. */
    {
        const char *tmpl = angdroid_game_ui_text("hits {target}", 1);
        const char *tag = strstr(tmpl, "{target}");
        CHECK(tag != NULL);
        (void)snprintf(out, sizeof(out), "獸人%.*s%s%s",
            (int)(tag - tmpl), tmpl, "你", tag + strlen("{target}"));
        CHECK(strcmp(out, "獸人擊中了你") == 0);
    }
    {
        const char *tmpl = angdroid_game_ui_text(
            "insults {oftarget} mother!", 1);
        const char *tag = strstr(tmpl, "{oftarget}");
        CHECK(tag != NULL);
        (void)snprintf(out, sizeof(out), "獸人%.*s%s%s",
            (int)(tag - tmpl), tmpl, "你的", tag + strlen("{oftarget}"));
        CHECK(strcmp(out, "獸人辱罵了你的母親！") == 0);
    }
    CHECK(strcmp(angdroid_game_ui_text("You stand your ground!", 1),
        "你挺身站穩，毫不退縮！") == 0);
    CHECK(strcmp(angdroid_game_ui_text("You resist the effects!", 1),
        "你抵抗了這股影響！") == 0);

    printf("PASS: %d FA melee action locale checks\n", checks);
    return 0;
}
