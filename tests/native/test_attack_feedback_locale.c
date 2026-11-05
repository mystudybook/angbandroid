/* Effect-handler UI messages and all original projection descriptions. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-attack-feedback.h"
#include "../../app/src/main/cpp/angband/zh-tw-projection-terms.h"
static int checks;
static void verify(const char *(*translate)(const char *, int),
    const char *english, const char *chinese)
{
    assert(strcmp(translate(english, 1), chinese) == 0);
    assert(translate(english, 0) == english);
    checks += 2;
}
int main(void)
{
    verify(angdroid_attack_feedback, "You feel a little better.", "你感覺好了一點。");
    verify(angdroid_attack_feedback, "You feel better.", "你感覺好多了。");
    verify(angdroid_attack_feedback, "You feel much better.", "你感覺大幅好轉。");
    verify(angdroid_attack_feedback, "You feel very good.", "你感覺身心舒暢。");
    verify(angdroid_attack_feedback, "Light shoots in all directions!", "光芒向四面八方射出！");
    verify(angdroid_attack_feedback, "The ground shakes for a moment.", "地面短暫震動了一下。");
    verify(angdroid_attack_feedback, "There is a searing blast of light!", "一道刺眼的強光爆發了！");
    verify(angdroid_attack_feedback, "Darkness seems to crush you!", "黑暗彷彿要將你壓碎！");
    verify(angdroid_attack_feedback, "The ground shakes! The ceiling caves in!", "地面劇烈搖晃！洞頂崩塌了！");
    verify(angdroid_attack_feedback, "The cave ceiling collapses on you!", "洞頂朝你塌了下來！");
    verify(angdroid_attack_feedback, "The cave floor twists in an unnatural way!", "洞穴地面詭異地扭曲！");
    verify(angdroid_attack_feedback, "The cave quakes!", "洞穴劇烈震動！");
    verify(angdroid_attack_feedback, "You are pummeled with debris!", "碎石不斷砸向你！");
    verify(angdroid_attack_feedback, "No monster selected!", "尚未選擇怪物！");
    verify(angdroid_attack_feedback, "This spell must target a monster.", "這個法術必須以怪物為目標。");
    verify(angdroid_attack_feedback, "The way is barred.", "前方道路被阻擋了。");
    verify(angdroid_attack_feedback, "You are already in single combat!", "你已經處於單挑狀態！");
    verify(angdroid_attack_feedback, "You must attack a monster.", "你必須攻擊一隻怪物。");
    verify(angdroid_attack_feedback, "You feel a surge of power!", "你感受到一股力量湧現！");
    verify(angdroid_attack_feedback, "You nimbly dodge the blast!", "你靈巧地閃過了衝擊！");
    verify(angdroid_attack_feedback, "You are bashed by rubble!", "碎石猛烈地撞擊了你！");
    verify(angdroid_attack_feedback, "You are crushed between the floor and ceiling!", "你被地面與洞頂夾住了！");
    verify(angdroid_projection_description, "acid", "酸液");
    verify(angdroid_projection_description, "lightning", "閃電");
    verify(angdroid_projection_description, "fire", "火焰");
    verify(angdroid_projection_description, "cold", "寒氣");
    verify(angdroid_projection_description, "poison", "毒素");
    verify(angdroid_projection_description, "light", "光明");
    verify(angdroid_projection_description, "dark", "黑暗");
    verify(angdroid_projection_description, "sound", "聲波");
    verify(angdroid_projection_description, "shards", "碎片");
    verify(angdroid_projection_description, "nexus", "次元之力");
    verify(angdroid_projection_description, "nether", "冥界之力");
    verify(angdroid_projection_description, "chaos", "混沌");
    verify(angdroid_projection_description, "disenchantment", "解除魔法");
    verify(angdroid_projection_description, "water", "水流");
    verify(angdroid_projection_description, "ice", "冰霜");
    verify(angdroid_projection_description, "gravity", "重力");
    verify(angdroid_projection_description, "inertia", "慣性");
    verify(angdroid_projection_description, "force", "衝擊力");
    verify(angdroid_projection_description, "time", "時間之力");
    verify(angdroid_projection_description, "plasma", "電漿");
    verify(angdroid_projection_description, "a meteor", "流星");
    verify(angdroid_projection_description, "a missile", "魔法飛彈");
    verify(angdroid_projection_description, "mana", "魔力");
    verify(angdroid_projection_description, "a holy orb", "神聖光球");
    verify(angdroid_projection_description, "an arrow", "箭矢");
    verify(angdroid_projection_description, "darkness", "黑暗之力");
    verify(angdroid_projection_description, "rock remover", "移除岩石");
    verify(angdroid_projection_description, "destroys doors", "摧毀門扉");
    verify(angdroid_projection_description, "disables traps", "解除陷阱");
    verify(angdroid_projection_description, "creates doors", "創造門扉");
    verify(angdroid_projection_description, "creates traps", "製造陷阱");
    verify(angdroid_projection_description, "teleports undead away", "將不死生物傳送離開");
    verify(angdroid_projection_description, "teleports evil monsters away", "將邪惡怪物傳送離開");
    verify(angdroid_projection_description, "teleports monsters with a spirit away", "將有靈魂的怪物傳送離開");
    verify(angdroid_projection_description, "teleports monsters away", "將怪物傳送離開");
    verify(angdroid_projection_description, "turns undead", "驅退不死生物");
    verify(angdroid_projection_description, "frightens evil monsters", "使邪惡怪物恐懼");
    verify(angdroid_projection_description, "scares living monsters", "使活物恐懼");
    verify(angdroid_projection_description, "causes monsters to flee", "使怪物逃跑");
    verify(angdroid_projection_description, "damages undead", "傷害不死生物");
    verify(angdroid_projection_description, "damages evil monsters", "傷害邪惡怪物");
    verify(angdroid_projection_description, "damages all monsters", "傷害所有怪物");
    verify(angdroid_projection_description, "attempts to put undead to sleep", "試圖使不死生物沉睡");
    verify(angdroid_projection_description, "attempts to put evil monsters to sleep", "試圖使邪惡怪物沉睡");
    verify(angdroid_projection_description, "attempts to put all monsters to sleep", "試圖使所有怪物沉睡");
    verify(angdroid_projection_description, "hastes, heals and magically duplicates monsters", "加速、治療並複製怪物");
    verify(angdroid_projection_description, "polymorphs monsters into other kinds of creatures", "將怪物變成其他生物");
    verify(angdroid_projection_description, "heals monsters", "治療怪物");
    verify(angdroid_projection_description, "hastes monsters", "加速怪物");
    verify(angdroid_projection_description, "attempts to slow monsters", "試圖減緩怪物速度");
    verify(angdroid_projection_description, "attempts to confuse monsters", "試圖使怪物混亂");
    verify(angdroid_projection_description, "attempts to hold monsters still", "試圖使怪物無法行動");
    verify(angdroid_projection_description, "attempts to stun monsters", "試圖使怪物暈眩");
    verify(angdroid_projection_description, "damages living monsters", "傷害活物");
    verify(angdroid_projection_description, "kills monsters below a hitpoint threshold", "消滅生命值低於門檻的怪物");
    {
        const char *future = "new magical projection";
        assert(angdroid_projection_description(future, 1) == future);
        assert(angdroid_attack_feedback(future, 1) == future);
        assert(angdroid_projection_description(NULL, 1) == NULL);
        assert(angdroid_attack_feedback(NULL, 1) == NULL);
        checks += 4;
    }
    printf("PASS: effect-handler and projection locale (%d C99 checks)\n",
        checks);
    return 0;
}
