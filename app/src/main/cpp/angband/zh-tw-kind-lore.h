/* Known item-kind descriptive text: canonical English name lookup only.
 * Incomplete batches retain original English description when not mapped.
 * Do not alter game data, unidentified flavors, save files, or exports.
 */
#ifndef ANGDROID_ZH_TW_KIND_LORE_H
#define ANGDROID_ZH_TW_KIND_LORE_H
#include <stddef.h>
#include <string.h>
#define ANGDROID_ZH_TW_KIND_LORE 42
static inline const char *angdroid_zh_tw_kind_flavor(const char *name)
{
    static const struct { const char *en; const char *zh; }
        entries[ANGDROID_ZH_TW_KIND_LORE] = {
        { "& Flask~ of oil", "一壺油能讓提燈燃燒七千五百回合，最多可補充至一萬五千回合。投擲時它會被視為已點燃，因此能造成比一般物品更高的傷害，即使目標具有火焰抗性也一樣。" },
        { "& Hard Biscuit~", "這塊主要以黑麥製成的餅乾相當耐飽，只是硬得幾乎會崩掉牙齒。" },
        { "& Apple~", "雖然表皮皺巴巴，這顆蘋果仍然完好且香甜。" },
        { "& Handful~ of Dried Fruits", "這些水果略經切碎，再以陽光曬乾，是伊西力安遊俠的行軍口糧。" },
        { "& Slime Mold~", "這種奇異的真菌增生物，在沒有人注視時似乎會自行移動。味道不怎麼樣，卻相當能填飽肚子。" },
        { "& Ration~ of Food", "這種被湖人稱為「克蘭姆」的乾糧，營養充足卻十分平淡，準備踏上漫長旅途的人都很熟悉。它能維持體力，但吃起來除了不停咀嚼，幾乎沒有任何樂趣。" },
        { "& Scrap~ of Flesh", "一塊灰色的乾肉，你不敢猜它究竟出自什麼生物。除非迫不得已，否則絕不會想吃。" },
        { "& Slice~ of Meat", "這是一片厚厚的豬肉，經過醃鹽以防腐敗，十分耐飽。" },
        { "& Honey-cake~", "比翁一族以人類之中最傑出的烘焙技藝聞名，他們的蜂蜜蛋糕更是名不虛傳。以小麥粉、堅果和蜂蜜製成，經過兩次烘烤，足以支撐漫長的行軍。" },
        { "& Piece~ of Elvish Waybread", "精靈的食物總是美味，蘭巴斯旅糧也不例外。這些薄餅以樹葉包裹，即使保存數月仍十分可口。" },
        { "& Flask~ of Whisky", "這種酒能讓你的精神振奮。" },
        { "& Pint~ of Fine Wine", "這是用來沖淡難吃口糧的絕佳飲品。" },
        { "& Sip~ of Miruvor", "只要小啜一口這種溫暖而芳香的美酒，你便能重拾希望與活力。" },
        { "& Swig~ of Orcish Liquor", "這種酒呈髒兮兮的棕色，讓你很難信任它。" },
        { "& Draught~ of the Ents", "這飲品看起來像清水，香氣卻彷彿清涼夜風吹來的遙遠森林氣息。" },
        { "Terror", "它能讓你拔腿逃命。" },
        { "Stoneskin", "其中的毒性能短暫地將你的皮膚化為石頭。" },
        { "Sprinting", "它會暫時使你加速，隨後卻又讓你慢上一段時間。" },
        { "Purging", "它會讓你飢餓難耐，但除此之外會讓身體恢復健康。" },
        { "Shadows", "它能在短時間內大幅提高你的潛行能力。" },
        { "& Dagger~", "這把短小的雙刃武器十分適合突刺或投擲。" },
        { "& Main~ Gauche~", "這柄短刺劍配有寬大的護手，主要用於決鬥。" },
        { "& Rapier~", "這把又長又細的刺劍，主要用於決鬥。" },
        { "& Short Sword~", "一把短小的刺劍。" },
        { "& Cutlass~", "這把刀刃比匕首略厚略長，彎度不大，只有單面開鋒，適合揮砍。" },
        { "& Tulwar~", "這把刀是彎刀的一種，刃身更寬，弧度則較小。" },
        { "& Scimitar~", "這柄長軍刀的刀身彎曲得非常明顯。" },
        { "& Long Sword~", "這把長直雙刃劍的握柄處伸出兩道護手，形成完美的十字形。" },
        { "& Broad Sword~", "這把厚重的單刃劍配有結實的握柄。" },
        { "& Bastard Sword~", "這把劍也稱「一手半劍」，劍身比一般直劍更長，兩側都開有鋒刃。" },
        { "& Katana~", "這把長柄單刃武士刀略微彎曲，刀尖呈鑿形，適合劈砍。" },
        { "& Zweihander~", "這把巨劍專為雙手揮舞而設計。" },
        { "& Executioner's Sword~", "這把巨大而彎曲的劍，專門用來乾淨俐落地斬下頭顱。" },
        { "& Blade~ of Chaos", "這把變幻莫測的武器沉醉於混亂之中，嗜血地砍殺與撕裂敵人。" },
        { "& Whip~", "這條厚重的鞭子需從肩膀甩出，力道足以擊倒一名魁梧男子。" },
        { "& Ball-and-Chain~", "一顆沉重的鐵球以鎖鏈懸掛在堅固的橡木手柄末端。" },
        { "& Morning Star~", "堅固木桿透過鎖鏈連著一顆鋼球，球面往往布滿尖刺。" },
        { "& Flail~", "兩根厚實木棒以短鎖鏈相連，能造成強大的鈍擊傷害。" },
        { "& Two-Handed Great Flail~", "這件武器以粗大的鎖鏈連接兩根巨型木棒，必須用雙手才能妥善揮舞。" },
        { "& Mace~", "這是一柄鐵頭戰棍，棍頭通常帶有棱角或尖刺。" },
        { "& Lead-Filled Mace~", "這柄沉重戰棍的鋼製棍頭灌滿鉛，能造成極強的粉碎打擊。" },
        { "& Quarterstaff~", "這根六英呎長的橡木棍可用雙手揮舞，既能痛擊敵人，也能格擋攻勢。" },
    };
    size_t i;
    if (!name) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_KIND_LORE; ++i)
        if (strcmp(name, entries[i].en) == 0)
            return entries[i].zh;
    return NULL;
}
#endif
