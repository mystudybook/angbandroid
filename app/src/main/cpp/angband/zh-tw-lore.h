/* Curated, presentation-only zh-TW monster flavor. No gamedata mutation.
 * Exact canonical English race name lookup; NULL means English fallback. */
#ifndef ANGDROID_ZH_TW_LORE_H
#define ANGDROID_ZH_TW_LORE_H
#include <string.h>
#include <stddef.h>
#define ANGDROID_ZH_TW_MONSTER_LORE 59
struct angdroid_lore_entry { const char *race_name; const char *description; };
static inline const char *angdroid_zh_tw_monster_flavor(const char *race_name)
{
    static const struct angdroid_lore_entry entries[ANGDROID_ZH_TW_MONSTER_LORE] = {
        { "filthy street urchin", "他衣衫襤褸，模樣十分邋遢，令人退避三舍。" },
        { "scrawny cat", "這隻瘦巴巴的毛球有著尖銳的爪子和充滿敵意的眼神。" },
        { "scruffy little dog", "這隻瘦小的流浪狗身上滿是跳蚤，見你靠近就低聲咆哮。" },
        { "Farmer Maggot", "他的狗不見了，蘑菇也遭人偷走；這名哈比農夫心情糟透了！" },
        { "blubbering idiot", "他總是不停地哭哭啼啼。" },
        { "boil-covered wretch", "用「醜陋」二字，實在不足以形容他的模樣。" },
        { "village idiot", "他口水直流，舉止滑稽。" },
        { "pitiful-looking beggar", "看著他，你不由得心生憐憫。" },
        { "mangy-looking leper", "你覺得最好別隨便碰觸他。" },
        { "squint-eyed rogue", "這是個見多識廣的街頭惡棍，總能一眼認出好下手的目標。" },
        { "singing, happy drunk", "看到他的模樣，你不禁慶幸自己還清醒。" },
        { "aimless-looking merchant", "這名商人在城裡閒晃，錢袋叮噹作響，還想收購更多裝飾用的護符。" },
        { "mean-looking mercenary", "只要有錢可賺，他什麼骯髒工作都願意接。" },
        { "battle-scarred veteran", "這名傷痕累累的老兵對陌生人可不友善。" },
        { "red-hatted elf", "正逢冬季節慶，這名戴紅帽的精靈似乎喝多了幾杯。" },
        { "Father Christmas", "聖誕老人正在城鎮裡為街童們派送禮物。" },
        { "grey mold", "一團小巧而古怪的灰色菌類。" },
        { "grey mushroom patch", "嗯！看起來似乎挺好吃。" },
        { "giant yellow centipede", "這隻肉食性蜈蚣大約有四英呎長。" },
        { "giant white centipede", "這隻肉食性蜈蚣大約有四英呎長。" },
        { "white icky thing", "一隻不大不小、黏黏糊糊的噁心生物。" },
        { "clear icky thing", "一團半透明、黏稠又令人作嘔的生物。" },
        { "giant white mouse", "這隻白鼠長約三英呎，嘴裡長著大牙。" },
        { "large white snake", "這條白蛇大約有八英呎長。" },
        { "small kobold", "這個矮小醜陋的人形生物，長著一張狗臉。" },
        { "white worm mass", "一大團黏糊糊的蠕蟲。" },
        { "floating eye", "一顆沒有身體的眼球，漂浮在離地幾英呎的空中。" },
        { "rock lizard", "這隻小蜥蜴披著堅硬的鱗皮。" },
        { "wild dog", "這隻野狗不停狂吠、齜牙咧嘴，成群出沒時尤其危險。" },
        { "soldier ant", "一隻長著強壯大顎的大型螞蟻。" },
        { "fruit bat", "這種害獸飛行迅速。" },
        { "kobold", "一個長著狗頭的小型人形生物。" },
        { "shrieker mushroom patch", "看起來挺好吃，但它發出的聲響可沒那麼討喜……" },
        { "blubbering icky thing", "一隻黏糊糊又飢腸轆轆的噁心生物。" },
        { "metallic green centipede", "這隻帶著金屬綠光澤的肉食性蜈蚣大約有四英呎長。" },
        { "soldier", "一名經驗不足、卻相當強悍的戰士。" },
        { "cutpurse", "一個鬼鬼祟祟、行跡可疑的傢伙。" },
        { "acolyte", "他那身祭司長袍似乎讓他走路都不太方便。" },
        { "apprentice", "他走過的地方，留下一路掉落的施法材料。" },
        { "yellow mushroom patch", "嗯！看起來似乎挺好吃。" },
        { "white jelly", "一大團白色的肉狀物。" },
        { "giant green frog", "一隻和狼差不多大的綠色青蛙。" },
        { "giant black ant", "這隻巨型黑蟻大約有三英呎長。" },
        { "salamander", "一隻身上有黑色與橘色斑紋的小蜥蜴。" },
        { "white harpy", "一隻擁有女性面孔、會飛翔且不斷尖叫的鳥形怪物。" },
        { "blue yeek", "一個矮小的人形生物。" },
        { "Grip, Farmer Maggot's Dog", "農夫馬戈特養的一隻兇猛惡犬，牠以為你想偷走蘑菇。" },
        { "Fang, Farmer Maggot's Dog", "農夫馬戈特養的一隻兇猛惡犬，牠以為你想偷走蘑菇。" },
        { "green worm mass", "一大團黏糊糊的蠕蟲。" },
        { "cave spider", "一隻黑色的蜘蛛，動作忽快忽慢。" },
        { "wild cat", "這隻野貓比一般貓大得多，嘶嘶作響；柔軟腳掌裡藏著尖利的爪子。" },
        { "crow", "一隻灰色的兜帽烏鴉，翅膀與頭部卻是黑色。" },
        { "Sméagol", "他通常被稱為咕嚕，鬼鬼祟祟地四處遊蕩，心裡只想著他的「寶貝」。" },
        { "green ooze", "它通體綠色，還不停滲出黏液。" },
        { "poltergeist", "一團陰森可怕的幽靈形體。" },
        { "metallic blue centipede", "這隻泛著金屬藍色光澤的肉食性蜈蚣大約有四英呎長。" },
        { "giant white louse", "這隻白色巨蝨長約六英吋。" },
        { "black naga", "牠有一副龐大的黑色蛇身，上半身卻像女性。" },
        { "spotted mushroom patch", "嗯！看起來似乎挺好吃。" },
    };
    size_t i;
    if (!race_name) return NULL;
    for (i = 0; i < ANGDROID_ZH_TW_MONSTER_LORE; ++i)
        if (strcmp(race_name, entries[i].race_name) == 0)
            return entries[i].description;
    return NULL;
}
#endif
