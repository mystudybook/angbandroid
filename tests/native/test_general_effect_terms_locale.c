/* Stat adjectives from object_property.txt and canonical weapon brand
 * display terms. None of these replace the English identifiers in gamedata.
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-general-effect-terms.h"
static int checks;
static void verify(const char *(*lookup)(const char *, int),
    const char *en, const char *zh)
{
    assert(strcmp(lookup(en, 1), zh) == 0);
    assert(lookup(en, 0) == en);
    checks += 2;
}
int main(void)
{
    verify(angdroid_general_stat_adjective, "weak", "虛弱");
    verify(angdroid_general_stat_adjective, "stupid", "愚鈍");
    verify(angdroid_general_stat_adjective, "naive", "天真");
    verify(angdroid_general_stat_adjective, "clumsy", "笨拙");
    verify(angdroid_general_stat_adjective, "sickly", "病弱");
    verify(angdroid_general_stat_adjective, "strong", "強壯");
    verify(angdroid_general_stat_adjective, "smart", "聰明");
    verify(angdroid_general_stat_adjective, "wise", "睿智");
    verify(angdroid_general_stat_adjective, "dextrous", "靈巧");
    verify(angdroid_general_stat_adjective, "healthy", "健康");
    verify(angdroid_general_brand_name, "Flame", "火焰");
    verify(angdroid_general_brand_name, "Frost", "冰霜");
    verify(angdroid_general_brand_name, "Venom", "毒素");
    verify(angdroid_general_item_type, "staff", "法杖");
    verify(angdroid_general_item_type, "wand", "魔杖");
    {
        const char *future = "new stat adjective";
        assert(angdroid_general_stat_adjective(future, 1) == future);
        assert(angdroid_general_brand_name(future, 1) == future);
        assert(angdroid_general_item_type(future, 1) == future);
        assert(angdroid_general_stat_adjective(NULL, 1) == NULL);
        checks += 4;
    }
    printf("PASS: 15 original stat, brand and device display terms (%d checks)\n",
        checks);
    return 0;
}
