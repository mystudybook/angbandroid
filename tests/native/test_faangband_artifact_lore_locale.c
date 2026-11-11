/* All supplemental FAangband artifact stories are optional UI-only text. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/zh-tw-artifact-lore.h"
#include "../../app/src/main/cpp/angband/zh-tw-faangband-artifact-lore.h"

static unsigned long checks;
#define CHECK(cond) do { assert(cond); ++checks; } while (0)

static void test_catalog(void)
{
    static const char *names[] = {
        "of Gwindor",
        "'Nimphelos'",
        "of Avallónë",
        "of Idril",
        "of Egalmoth",
        "'Narsil'",
        "of Manwë",
        "of Rog",
        "of Boldog",
        "of the Marches",
        "'Dramborleg'",
        "of Bregor",
        "of Hithlum",
        "of Mablung",
        "of Vinyamar",
        "of Ecthelion",
        "of Thuringwethil",
        "of Nargothrond",
        "of the Hidden Kingdom",
        "(Dwarven) of the Mormegil",
        "of Nevrast",
        "of Valinor",
        "of Glorfindel",
        "of Maeglin",
        "of the Elements",
        "of Khazad-dum",
        "of Haleth",
        "of the Swan",
        "'Ilkorin'",
        "of Beguiling",
        "of Unmaking",
        "of Ossë",
        "of Starfire",
        "of Winds",
        "of Holding",
        "of the Kelvar",
        "of Delving",
        "'Mantle of Shadow'",
        "of Air",
        "of Portals",
        "of Ungoliant",
    };
    size_t i, j;
    CHECK(ANGDROID_ZH_TW_FAANGBAND_ARTIFACT_LORE == 41);
    CHECK(sizeof(names) / sizeof(names[0]) == 41);
    for (i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        const char *text = angdroid_zh_tw_faangband_artifact_flavor(names[i]);
        CHECK(text != NULL);
        CHECK(*text != '\0');
        CHECK(strlen(text) > 12);
        CHECK(angdroid_zh_tw_artifact_flavor(names[i]) == NULL);
        for (j = 0; j < i; ++j) CHECK(strcmp(names[i], names[j]) != 0);
    }
    CHECK(strcmp(angdroid_zh_tw_faangband_artifact_flavor(
        "of Beguiling"), "每個生物都有自身的弱點……") == 0);
    CHECK(strcmp(angdroid_zh_tw_faangband_artifact_flavor(
        "of Nargothrond"), "這頂王冠曾由芬羅德·費拉剛佩戴，後來傳給他的弟弟歐羅隹斯。") == 0);
    CHECK(strstr(angdroid_zh_tw_faangband_artifact_flavor(
        "'Narsil'"), "魔苟斯") != NULL);
    CHECK(strstr(angdroid_zh_tw_faangband_artifact_flavor(
        "'Mantle of Shadow'"), "傳奇盜賊") != NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor(NULL) == NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor("") == NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor("of Unknown") == NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor("'Sting'") == NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor("of Galadriel") == NULL);
    CHECK(angdroid_zh_tw_faangband_artifact_flavor("of narsil") == NULL);
    CHECK(angdroid_zh_tw_artifact_flavor("of Galadriel") != NULL);
}

int main(void)
{
    test_catalog();
    printf("PASS: 41 FAangband artifact lore entries and Angband fallback (%lu assertions)\n", checks);
    return 0;
}
