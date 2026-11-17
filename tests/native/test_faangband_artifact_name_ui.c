/* C99 regression: FAangband UI-only artifact suffix and UTF-8 safety. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-artifact-zh-tw.h"
#include "../../app/src/main/cpp/angband/obj-desc-suffix-utf8.h"

static unsigned long checks;
#define CHECK(condition) do { assert(condition); ++checks; } while (0)

int main(void)
{
    static const char *fa_artifacts[] = {
        "of Gwindor",
        "'Nimphelos'",
        "of Avallónë",
        "'Angrist'",
        "'Narthanc'",
        "'Nimthanc'",
        "'Dethanc'",
        "of Rilia",
        "'Belangil'",
        "of Maedhros",
        "'Forasgil'",
        "'Careth Asdriag'",
        "of Idril",
        "'Dagmor'",
        "of Egalmoth",
        "'Ringil'",
        "'Narsil'",
        "'Anguirel'",
        "'Elvagil'",
        "'Aranrúth'",
        "'Glamdring'",
        "'Aeglin'",
        "'Orcrist'",
        "'Calris'",
        "'Aglarang'",
        "'Gurthang'",
        "'Crisdurian'",
        "'Doomcaller'",
        "of Manwë",
        "'Bloodspike'",
        "'Firestar'",
        "'Totila'",
        "'Thunderfist'",
        "'Taratol'",
        "'Nar-i-vagil'",
        "'Eriril'",
        "of Olórin",
        "of Aulë",
        "'Deathwreaker'",
        "'Turmil'",
        "of Rog",
        "'Aiglos'",
        "of Oromë",
        "'Nimloth'",
        "of Boldog",
        "of Wrath",
        "of Ulmo",
        "'Osondir'",
        "'Til-i-arc'",
        "of Húrin",
        "'Barukkheled'",
        "of Balli Stonehand",
        "'Lotharang'",
        "of the Marches",
        "'Dramborleg'",
        "of Durin",
        "of Eönwë",
        "'Avavir'",
        "of Eöl",
        "of Pain",
        "of Amrod",
        "of Celegorm",
        "'Belthronding'",
        "of Bregor",
        "'Cubragol'",
        "of Fëanor",
        "'Dal-i-thalion'",
        "of Hithlum",
        "of Mablung",
        "'Holhenneth'",
        "of Vinyamar",
        "of Gorlim",
        "of Dor-Lómin",
        "of Ecthelion",
        "of Thuringwethil",
        "of Nargothrond",
        "of the Hidden Kingdom",
        "'Hithlomir'",
        "of Himring",
        "'Thalkettoth'",
        "(Dwarven) of the Mormegil",
        "of Caspanion",
        "of Nevrast",
        "'Belegennon'",
        "of Celeborn",
        "'Soulkeeper'",
        "of Valinor",
        "'Holcolleth'",
        "of Thingol",
        "'Colannon'",
        "of Lúthien",
        "of Tuor",
        "of Glorfindel",
        "'Cambeleg'",
        "'Cammithrim'",
        "'Paurnimmen'",
        "'Pauraegen'",
        "'Paurnen'",
        "'Camlost'",
        "of Maeglin",
        "of Fingolfin",
        "of the Elements",
        "of Khazad-dum",
        "of Haleth",
        "of the Swan",
        "of Eärendil",
        "of Gil-galad",
        "of the Dwarves",
        "'Elessar'",
        "'Razorback'",
        "'Bladeturner'",
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
        "'Grond'",
        "of Morgoth",
    };
    size_t i;
    char enough[64] = "Sword";
    char small[20] = "Sword";
    char exact[22] = "Sword";
    size_t end = 5;
    const char *localized;

    CHECK(sizeof(fa_artifacts) / sizeof(fa_artifacts[0]) == 126);
    for (i = 0; i < sizeof(fa_artifacts) / sizeof(fa_artifacts[0]); ++i) {
        const char *english = fa_artifacts[i];
        localized = angdroid_faangband_artifact_ui_name(english, true);
        CHECK(localized != NULL);
        CHECK(strcmp(localized, english) != 0);
        CHECK(angdroid_faangband_artifact_ui_name(english, false) == english);
        CHECK(angdroid_zh_tw_artifact_name(english) != NULL ||
              angdroid_zh_tw_faangband_artifact_name(english) != NULL);
    }

    CHECK(strcmp(angdroid_faangband_artifact_ui_name(
        "'Narsil'", true), "「納西爾」") == 0);
    CHECK(strcmp(angdroid_faangband_artifact_ui_name(
        "of Gwindor", true), "格溫多之物") == 0);
    CHECK(strcmp(angdroid_faangband_artifact_ui_name(
        "of Eöl", true), "埃歐爾之物") == 0);
    CHECK(strcmp(angdroid_faangband_artifact_ui_name(
        "'Sting'", true), "「刺針」") == 0);
    CHECK(angdroid_faangband_artifact_ui_name(
        "not an artifact", true) != NULL);
    CHECK(strcmp(angdroid_faangband_artifact_ui_name(
        "not an artifact", true), "not an artifact") == 0);
    CHECK(angdroid_faangband_artifact_ui_name(NULL, true) == NULL);
    CHECK(angdroid_faangband_artifact_ui_name(NULL, false) == NULL);

    /* Atomic append cannot split Chinese bytes or punctuation. */
    localized = angdroid_faangband_artifact_ui_name("'Narsil'", true);
    CHECK(!angdroid_obj_desc_append_suffix_atomic(
        small, sizeof(small), &end, localized, true));
    CHECK(end == 5);
    CHECK(strcmp(small, "Sword") == 0);
    end = 5;
    CHECK(angdroid_obj_desc_append_suffix_atomic(
        enough, sizeof(enough), &end, localized, true));
    CHECK(strcmp(enough, "Sword 「納西爾」") == 0);
    CHECK(end == strlen(enough));

    /* Exact-fit allocation: five English bytes, one separator,
     * fifteen UTF-8 bytes and one NUL. */
    end = 5;
    CHECK(angdroid_obj_desc_append_suffix_atomic(
        exact, sizeof(exact), &end, localized, true));
    CHECK(strcmp(exact, "Sword 「納西爾」") == 0);
    CHECK(end == sizeof(exact) - 1);

    /* Unknown suffixes also follow the same buffer and UTF-8 rules. */
    end = 5;
    CHECK(!angdroid_obj_desc_append_suffix_atomic(
        small, sizeof(small), &end, "This label is too long", true));
    CHECK(end == 5);
    CHECK(strcmp(small, "Sword") == 0);

    /* An invalid UTF-8 sequence must not alter the buffer. */
    end = 5;
    CHECK(!angdroid_obj_desc_append_suffix_atomic(
        enough, sizeof(enough), &end, "A\xC0\xAF", true));
    CHECK(end == 5);
    CHECK(strcmp(enough, "Sword 「納西爾」") == 0);

    printf("PASS: 126 FA artifact names, 41 FA-exclusive, UI gate and UTF-8 (%lu checks)\n",
           checks);
    return 0;
}
