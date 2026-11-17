/* C99: FAangband's 155 source flavor spellings, knowledge and title privacy. */
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/faangband/obj-desc-flavor-zh-tw.h"

static unsigned long checks;
#define CHECK(x) do { assert(x); ++checks; } while (0)

int main(void)
{
    static const char *appearances[] = {
        "Aspen",
        "Balsa",
        "Banyan",
        "Birch",
        "Cedar",
        "Cottonwood",
        "Cypress",
        "Dogwood",
        "Elm",
        "Eucalyptus",
        "Hemlock",
        "Hickory",
        "Ironwood",
        "Locust",
        "Mahogany",
        "Maple",
        "Mulberry",
        "Oak",
        "Pine",
        "Redwood",
        "Rosewood",
        "Spruce",
        "Sycamore",
        "Teak",
        "Walnut",
        "Mistletoe",
        "Hawthorn",
        "Bamboo",
        "Silver Birch",
        "Hazel",
        "Golden",
        "Blackthorn",
        "Gnarled",
        "Ivory",
        "Willow",
        "Adamantium",
        "Damascene Steel",
        "Aluminium",
        "Cast Iron",
        "Chromium",
        "Copper",
        "Gold",
        "Iron",
        "Magnesium",
        "Molybdenum",
        "Nickel",
        "Rusty",
        "Silver",
        "Steel",
        "Tin",
        "Titanium",
        "Tungsten",
        "Zirconium",
        "Zinc",
        "Aluminium-Plated",
        "Copper-Plated",
        "Gold-Plated",
        "Nickel-Plated",
        "Silver-Plated",
        "Steel-Plated",
        "Tin-Plated",
        "Zinc-Plated",
        "Mithril-Plated",
        "Mithril",
        "Runed",
        "Bronze",
        "Brass",
        "Platinum",
        "Lead",
        "Lead-Plated",
        "Pewter",
        "Rusty Iron",
        "Engraved Gold",
        "Silver-Gilt",
        "Polished Steel",
        "White Gold",
        "Rusted Steel",
        "Spotted",
        "Rubbery",
        "Crumbly",
        "Moldy",
        "Mottled",
        "Firm",
        "Woody",
        "Wrinkled",
        "Furry",
        "Fragile",
        "Marbled",
        "Gelatinous",
        "Speckled",
        "Glowing",
        "Striped",
        "Slimy",
        "Moist",
        "Withered",
        "Waxy",
        "Smelly",
        "Clear",
        "Light Brown",
        "Icky Green",
        "Azure",
        "Blue",
        "Cerulean",
        "Black",
        "Brown",
        "Ochre",
        "Bubbling",
        "Chartreuse",
        "Cloudy",
        "Copper Speckled",
        "Crimson",
        "Cyan",
        "Ultramarine",
        "Dark Green",
        "Dark Red",
        "Light Yellow",
        "Green",
        "Muddy",
        "Grey",
        "Vespertine",
        "Hazy",
        "Indigo",
        "Light Blue",
        "Light Green",
        "Magenta",
        "Metallic Blue",
        "Metallic Red",
        "Metallic Green",
        "Metallic Purple",
        "Misty",
        "Orange",
        "Orange Speckled",
        "Pink",
        "Light Teal",
        "Light Violet",
        "Purple",
        "Purple Speckled",
        "Red",
        "Teal",
        "Mustard Yellow",
        "Smoky",
        "Tangerine",
        "Violet",
        "Vermilion",
        "White",
        "Yellow",
        "Light Purple",
        "Pungent",
        "Lavender",
        "Viscous Pink",
        "Oily Yellow",
        "Gloopy Green",
        "Shimmering",
        "Coagulated Crimson",
        "Light Pink",
    };
    char text[256], tiny[8];
    const char *translated;
    size_t i;
    CHECK(sizeof(appearances) / sizeof(appearances[0]) == 155);
    for (i = 0; i < sizeof(appearances) / sizeof(appearances[0]); ++i) {
        translated = angdroid_faangband_ui_flavor_name(appearances[i]);
        CHECK(translated != NULL);
        CHECK(strcmp(appearances[i], translated) != 0);
    }
    CHECK(strcmp(angdroid_faangband_ui_flavor_name("Adamantium"),
        "精金") == 0);
    CHECK(strcmp(angdroid_faangband_ui_flavor_name("Damascene Steel"),
        "大馬士革鋼") == 0);
    CHECK(strcmp(angdroid_faangband_ui_flavor_name("Azure"),
        "天藍色") == 0);
    CHECK(angdroid_faangband_ui_flavor_name(NULL) == NULL);
    CHECK(strcmp(angdroid_faangband_ui_flavor_name("unlisted flavor"),
        "unlisted flavor") == 0);

    /* The secret effect 'Healing' MUST NOT appear while unknown. */
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "potion", "Azure", "Healing", false, false, 1));
    CHECK(strcmp(text, "天藍色藥水") == 0);
    CHECK(strstr(text, "治療") == NULL);
    CHECK(strstr(text, "Healing") == NULL);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "potion", "Azure", "Healing", true, false, 1));
    CHECK(strcmp(text, "天藍色藥水（治療）") == 0);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "potion", "Azure", "Healing", true, true, 2));
    CHECK(strcmp(text, "2 瓶天藍色藥水（治療）") == 0);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "wand", "Adamantium", "Recall", false, true, 1));
    CHECK(strcmp(text, "精金魔杖") == 0);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "wand", "Damascene Steel", "Recall", true, false, 1));
    CHECK(strcmp(text, "大馬士革鋼魔杖（回返）") == 0);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "rod", "Unlisted Metal", "Recall", false, false, 1));
    CHECK(strcmp(text, "Unlisted Metal魔棒") == 0);
    CHECK(angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "mushroom", "Spotted", "very secret effect", false, false, 1));
    CHECK(strstr(text, "secret") == NULL);
    CHECK(!angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "sword", "Azure", "Healing", false, false, 1));
    CHECK(!angdroid_faangband_ui_flavored_object(tiny, sizeof(tiny),
        "potion", "Azure", "Healing", true, false, 1));
    CHECK(tiny[0] == '\0');
    CHECK(!angdroid_faangband_ui_flavored_object(text, sizeof(text),
        "potion", NULL, "Healing", true, false, 1));
    CHECK(text[0] == '\0');
    CHECK(!angdroid_faangband_ui_flavored_object(NULL, 0,
        "potion", "Azure", "Healing", true, false, 1));

    /* Runtime scroll title stays identical; only the wrapper is Chinese. */
    CHECK(angdroid_faangband_ui_scroll(text, sizeof(text),
        "\"foo bar\"", "Recall", false, false, 1));
    CHECK(strcmp(text, "題為「foo bar」的卷軸") == 0);
    CHECK(strstr(text, "回返") == NULL);
    CHECK(angdroid_faangband_ui_scroll(text, sizeof(text),
        "\"foo bar\"", "Recall", true, false, 1));
    CHECK(strcmp(text, "題為「foo bar」的卷軸（回返）") == 0);
    CHECK(angdroid_faangband_ui_scroll(text, sizeof(text),
        "\"foo bar\"", "Recall", true, true, 2));
    CHECK(strcmp(text, "2 張題為「foo bar」的卷軸（回返）") == 0);
    CHECK(!angdroid_faangband_ui_scroll(text, sizeof(text),
        "unquoted", "Recall", false, false, 1));
    CHECK(text[0] == '\0');
    CHECK(!angdroid_faangband_ui_scroll(tiny, sizeof(tiny),
        "\"foo bar\"", "Recall", true, false, 1));
    CHECK(tiny[0] == '\0');
    CHECK(angdroid_faangband_ui_scroll(text, sizeof(text),
        "\"foo bar\"", "Recall", true, false, 1));
    CHECK(!angdroid_faangband_ui_scroll(text, 0,
        "\"foo bar\"", "Recall", true, false, 1));
    CHECK(!angdroid_faangband_ui_scroll(NULL, 0,
        "\"foo bar\"", "Recall", true, false, 1));
    printf("PASS: 155 FA flavors, identified/unknown effects, scroll title (%lu checks)\n", checks);
    return 0;
}
