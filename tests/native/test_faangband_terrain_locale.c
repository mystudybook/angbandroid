/* FAangband wilderness terrain, exact prompts and non-fiery hurt locale. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-terrain-trap.h"
static unsigned checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)
int main(void)
{
    static const struct {const char *en, *zh, *lore;} features[] = {
        { "Travelling Merchant", "流動商人",
          "這位商人四處旅行，收購各式有價值的物品，並販售基本物資、武器與防具。" },
        { "easy path north", "北方平緩小徑",
          "通往相鄰地區的小徑。" },
        { "hard path north", "北方崎嶇小徑",
          "通往相鄰地區的小徑。" },
        { "easy path east", "東方平緩小徑",
          "通往相鄰地區的小徑。" },
        { "hard path east", "東方崎嶇小徑",
          "通往相鄰地區的小徑。" },
        { "easy path south", "南方平緩小徑",
          "通往相鄰地區的小徑。" },
        { "hard path south", "南方崎嶇小徑",
          "通往相鄰地區的小徑。" },
        { "easy path west", "西方平緩小徑",
          "通往相鄰地區的小徑。" },
        { "hard path west", "西方崎嶇小徑",
          "通往相鄰地區的小徑。" },
        { "water", "水域",
          "火焰性質且無法飛行的怪物不能穿過水域，多數其他怪物會減速；攜帶物品過重時，你也會減速。水中雙方更容易被擊中，也更難閃避；多數法術，尤其水系法術會增強，但火系法術威力大幅降低。水也可能蒸發。" },
        { "lowland trees", "低地樹林",
          "會讓不熟悉德魯伊技巧的角色，以及不會飛行或穿牆的非自然怪物移動緩慢。樹木會阻擋視線、飛射武器、魔法飛彈與射線，縮小範圍法術半徑，並為林中的角色與怪物提供近戰、遠程及魔法防護。樹木可能燃燒。" },
        { "highland trees", "高地樹林",
          "會讓不熟悉德魯伊技巧的角色，以及不會飛行或穿牆的非自然怪物移動緩慢。樹木會阻擋視線、飛射武器、魔法飛彈與射線，縮小範圍法術半徑，並為林中的角色與怪物提供近戰、遠程及魔法防護。樹木可能燃燒。" },
        { "grass", "草地",
          "所有生物都能輕鬆穿越的開闊草地，只會出現在荒野，也可能藏有陷阱。" },
        { "road", "道路",
          "開闊的道路，任何生物都能通行，也可放置陷阱或物品，不會遮擋視線。" },
        { "empty space", "虛空",
          "下方是虛空；只有能飛行的生物才能安全穿越，不會墜落。法術與飛射武器都能通過。" },
        { "sand dune", "沙丘",
          "會阻擋視線、飛射武器、魔法飛彈與射線，也會縮小範圍法術的影響半徑。" },
    };
    static const struct {const char *en, *zh;} warnings[] = {
        { "The lava will scald you!  Really step in? ", "熔岩會燙傷你！確定要踏入嗎？ " },
        { "Lava blocks your path.  Step into it? ", "熔岩擋住了前方的路。確定要踏入嗎？ " },
        { "It's a cliff! Really step off it?", "前方是懸崖！確定要踏出去嗎？" },
        { "You have come to a cliff.  Step off it?", "你已來到懸崖邊。確定要踏出去嗎？" },
    };
    size_t i;
    CHECK(sizeof(features) / sizeof(features[0]) == 16);
    CHECK(sizeof(warnings) / sizeof(warnings[0]) == 4);
    for (i = 0; i < sizeof(features) / sizeof(features[0]); ++i) {
        CHECK(strcmp(angdroid_terrain_name(features[i].en, 1), features[i].zh) == 0);
        CHECK(angdroid_terrain_name(features[i].en, 0) == features[i].en);
        CHECK(strcmp(angdroid_terrain_lore(features[i].en, "CANONICAL", 1),
                     features[i].lore) == 0);
        CHECK(strcmp(angdroid_terrain_lore(features[i].en, "CANONICAL", 0),
                     "CANONICAL") == 0);
    }
    for (i = 0; i < sizeof(warnings) / sizeof(warnings[0]); ++i) {
        CHECK(strcmp(angdroid_terrain_warning(warnings[i].en, 1),
                     warnings[i].zh) == 0);
        CHECK(angdroid_terrain_warning(warnings[i].en, 0) == warnings[i].en);
    }
    CHECK(strcmp(angdroid_terrain_hurt_message("You fall into darkness!", 1),
                 "你墜入黑暗之中！") == 0);
    CHECK(strcmp(angdroid_terrain_hurt_message("You fall into darkness!", 0),
                 "You fall into darkness!") == 0);
    CHECK(strcmp(angdroid_terrain_name("lava", 1), "熔岩") == 0);
    CHECK(strcmp(angdroid_terrain_lore("lava", "CANONICAL", 1),
                 "一池閃耀火光的滾燙熔岩；踏入其中非常危險！") == 0);
    CHECK(strcmp(angdroid_terrain_name("unknown future terrain", 1),
                 "unknown future terrain") == 0);
    CHECK(strcmp(angdroid_terrain_lore("unknown future terrain", "ORIGINAL", 1),
                 "ORIGINAL") == 0);
    CHECK(strcmp(angdroid_terrain_warning("Unknown cliff warning", 1),
                 "Unknown cliff warning") == 0);
    CHECK(strcmp(angdroid_terrain_hurt_message("Unknown hurt message", 1),
                 "Unknown hurt message") == 0);
    CHECK(angdroid_terrain_name(NULL, 1) == NULL);
    CHECK(angdroid_terrain_warning(NULL, 1) == NULL);
    CHECK(angdroid_terrain_hurt_message(NULL, 1) == NULL);
    printf("PASS: FAangband terrain displays and cliff confirmations (%u checks)\n", checks);
    return 0;
}
