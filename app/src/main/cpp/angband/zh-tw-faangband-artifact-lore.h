/* Additional FAangband-only artifact story text, for display only.
 * All 41 names come from FAangband gamedata/artifact.txt and are absent
 * from standard Angband artifact.txt. Never modify artifact->text or saves.
 * Lookups return NULL for any original or unknown artifact.
 */
#ifndef ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE_H
#define ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE 41
static inline const char *angdroid_zh_tw_faangband_artifact_flavor(
    const char *name)
{
    static const struct { const char *english, *traditional; }
        entries[ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE] = {
        { "of Gwindor", "這是諾多精靈傳說中的明燈之一，燃著澄澈的藍光。光芒永不熄滅，附近的黑暗無法停留。" },
        { "'Nimphelos'", "這顆來自巴拉爾灣的絕世珍珠，光澤宛如星光映在海浪泡沫之上。灰斗篷辛葛將它贈予貝烈戈斯特的矮人首領。" },
        { "of Avallónë", "這顆水晶由費諾在阿門洲製成。透過它，遙遠的事物也能縮小而清晰地呈現，彷彿具有雄鷹的目光。" },
        { "of Idril", "這把劍雖然小巧，卻蘊含驚人的力量；貢多林的伊綴爾·凱勒布琳達爾將它留作貼身防身之用。" },
        { "of Egalmoth", "這柄帶有大馬士革鋼紋的彎刀，揮舞起來輕巧得不可思議。它曾屬於貢多林天弓家族的領主埃加爾莫斯，他是諾多族中唯一使用彎刀的人。" },
        { "'Narsil'", "這把劍由諾格羅德最偉大的矮人工匠特爾查鍛造，閃耀著火焰、太陽與月亮的光輝。持有者在戰鬥中威力驚人，魔苟斯的造物無一能夠抵擋。" },
        { "of Manwë", "「……他們在他的怒風中枯萎，在他的劍所召下的雷霆中喪命……」" },
        { "of Rog", "這是無畏者羅格的長柄巨錘；他是貢多林怒錘家族的領主。諾多族最傑出的工匠打造了它，擊碎安格班大軍時，錘頭四周迸出火花。" },
        { "of Boldog", "這是半獸人大軍隊長博爾多格的巨型鐵矛。博爾多格被辛葛殺死後，馬布隆在獵狼之役中使用此矛。" },
        { "of the Marches", "這柄巨大戰斧有兩片剃刀般鋒利的斧刃，昔日由多瑞亞斯的邊境守衛使用。它龐大得通常必須雙手揮舞，斧身則精細刻滿抵禦元素的咒文。" },
        { "'Dramborleg'", "「重擊利刃」是貢多林的圖爾所持的巨斧。它既能像棍棒般重擊敵人，也能如利劍般將敵人劈開。" },
        { "of Bregor", "這是巴拉希爾之父布瑞戈爾的大型紫杉木弓，後來代代相傳，成為子孫珍藏的傳家寶。" },
        { "of Hithlum", "這雙靴子以熊皮製成，鑲嵌著蛋白石，能讓穿戴者快速而悄然無聲地移動。" },
        { "of Mablung", "這是多瑞亞斯的馬布隆所戴的帽子；他是辛葛國王麾下的獵人首領。" },
        { "of Vinyamar", "這頂頭盔是圖爾鞏留在維尼亞瑪，等待烏歐牟的使者前來領取的物品。盔頂飾有三根天鵝羽毛，賦予穿戴者更堅定的勇氣與清晰的意志。" },
        { "of Ecthelion", "這頂帶尖刺的鋼盔屬於噴泉家族的領主埃克塞里昂；他也是貢多林大門的守衛者。" },
        { "of Thuringwethil", "這頂深夜色的鋼製頭環曾屬於吸血鬼信使圖林微希爾。它能以沉重的肉體代價，賦予穿戴者異常敏銳的視力與感知。" },
        { "of Nargothrond", "這頂王冠曾由芬羅德·費拉剛佩戴，後來傳給他的弟弟歐羅隹斯。" },
        { "of the Hidden Kingdom", "這是貢多林國王圖爾鞏的王盔，四周鑲嵌著鑽石。" },
        { "(Dwarven) of the Mormegil", "這套鎖子甲由相互扣合的鋼環製成，配有護腿與護袖，內側以皮革厚實襯墊。穿上它時，你感受到納國斯隆德的黑劍客圖林·圖倫拔所具有的活力。" },
        { "of Nevrast", "這套閃耀的鋼甲自頸部覆蓋至雙腳，表面深深刻著守護與穩定的符文。" },
        { "of Valinor", "這件披風曾由來自維林諾的英雄穿戴；那片土地遠離元素之間的一切爭鬥。" },
        { "of Glorfindel", "這是英勇的葛羅芬戴爾所穿的金色披風；他曾是貢多林金花家族的領主。" },
        { "of Maeglin", "這雙包覆鐵片的護手屬於埃歐爾之子梅格林，其中流動著他能在戰鬥中運用的魔力。" },
        { "of the Elements", "這面圓盾包覆著純灰色皮革，即使身處元素交鋒的戰場也堅固穩定。" },
        { "of Khazad-dum", "這面小型金屬盾喚起矮人王族的力量與堅忍，足以抵擋大地元素之力。" },
        { "of Haleth", "安格班的半獸人襲擊並圍困哈烈絲的族人時，哈烈絲女士曾用這面盾牌守護他們。盾牌四周維持著神秘的平衡，將元素之間的衝突封存其中。" },
        { "of the Swan", "這面精靈式長盾呈尖長形，由木材製成並覆以金屬片，藍色底面繪有天鵝翅膀。它長年留在圖爾鞏荒廢的維尼亞瑪大廳，卻絲毫沒有失去光彩，並將同樣的持久耐力賦予持有者。" },
        { "'Ilkorin'", "致命的毒液不斷從這根魔杖滲出，足以使生命枯萎凋零。" },
        { "of Beguiling", "每個生物都有自身的弱點……" },
        { "of Unmaking", "最危險的毀滅法術，莫過於撕裂受造世界的結構，湮滅秩序，任由原初混沌狂亂咆哮。那股力量敵視一切存在，終有一天可能吞噬萬物。" },
        { "of Ossë", "邁雅歐希曾一度受到米爾寇的誘惑，至今仍對海洋的毀滅之力懷有喜愛。" },
        { "of Starfire", "這根帶刺的法杖從杖底到杖頂都覆蓋著旋渦狀紋路與微微發光的刻痕。" },
        { "of Winds", "四方之風急切地等待釋放，使這根法杖始終輕微顫動。" },
        { "of Holding", "突然間，那些死後仍執著於存在的生靈，哀號聲變得微弱無力。你以洪亮的聲音命令牠們停下，等待你的旨意！" },
        { "of the Kelvar", "這根法杖蘊藏著雅凡娜傾聽自己所創造生靈的力量。" },
        { "of Delving", "即使是矮人的精湛技藝，也必須臣服於能以一句話溶解山脈筋骨的人。" },
        { "'Mantle of Shadow'", "古代傳奇盜賊能將陰影披在身上，化作霧氣與傳聞，讓毫無戒心的敵人難以察覺。" },
        { "of Air", "阿爾達上空廣袤天空的軍勢，掌握著下方眾生未知的力量。只要你召喚牠們的憤怒，牠們便樂於痛擊你指定的敵人。" },
        { "of Portals", "如今幾乎無人知曉如何掌握神秘的出入口；能找到這根「隱秘道路魔棒」的人，實屬幸運。" },
        { "of Ungoliant", "「她在他們四周織起黑暗的斗篷……那是吞噬光芒的『無光』，萬物彷彿不再存在，目光無法穿透，因為其中只有虛無。」" },
    };
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE; ++i) {
        if (strcmp(name, entries[i].english) == 0)
            return entries[i].traditional;
    }
    return NULL;
}
#endif /* ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE_H */
