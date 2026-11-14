/* Exact English item-origin monster label rendering, never partial UTF-8. */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../../app/src/main/cpp/angband/item-origin-display.h"

static unsigned checks;
#define CHECK(e) do { assert(e); ++checks; } while (0)

static void known(const char *source, const char *article, int unique,
                  int comma, const char *expected)
{
    char name[256];
    CHECK(angdroid_item_origin_monster_name(name, sizeof(name), source,
          article, unique, comma) == 1);
    CHECK(strcmp(name, expected) == 0);
}
int main(void)
{
    char exact[6], small[5], utf8_small[10], unknown[256];
    char tiny[1] = {'X'};
    char long_name[400];
    size_t i;

    known("wolf", "a ", 0, 0, "a wolf");
    known("orc", "an ", 0, 1, "an orc,");
    known("Sauron, the Sorcerer", "a ", 1, 0,
          "Sauron, the Sorcerer");
    known("Gothmog, the High Captain of Balrogs", "a ", 1, 1,
          "Gothmog, the High Captain of Balrogs,");
    known("monster lost to history", "a ", 0, 0,
          "a monster lost to history");
    known("Eöl, the Dark Elven Smith", "an ", 1, 1,
          "Eöl, the Dark Elven Smith,");
    known("深淵巨龍", "a ", 0, 1, "a 深淵巨龍,");
    known("灰燼巨龍", "a ", 1, 0, "灰燼巨龍");
    known("", "a ", 0, 0, "a ");

    CHECK(angdroid_item_origin_monster_name(exact, sizeof exact,
          "bat", "a ", 0, 0) == 1);
    CHECK(strcmp(exact, "a bat") == 0);
    CHECK(angdroid_item_origin_monster_name(small, sizeof small,
          "bat", "a ", 0, 0) == 0);
    CHECK(small[0] == '\0');

    /* CJK source uses more bytes: either copy all bytes or none. */
    CHECK(angdroid_item_origin_monster_name(utf8_small, sizeof utf8_small,
          "巨龍族", "", 1, 0) == 1);
    CHECK(strcmp(utf8_small, "巨龍族") == 0);
    CHECK(angdroid_item_origin_monster_name(utf8_small,
          sizeof utf8_small - 1, "巨龍族", "", 1, 0) == 0);
    CHECK(utf8_small[0] == '\0');

    CHECK(angdroid_item_origin_monster_name(tiny, sizeof tiny,
          "anything", "a ", 0, 0) == 0);
    CHECK(tiny[0] == '\0');
    CHECK(angdroid_item_origin_monster_name(NULL, 0,
          "wolf", "a ", 0, 0) == 0);
    CHECK(angdroid_item_origin_monster_name(unknown, sizeof unknown,
          NULL, "a ", 0, 0) == 0);
    CHECK(unknown[0] == '\0');
    CHECK(angdroid_item_origin_monster_name(unknown, sizeof unknown,
          "wolf", NULL, 0, 0) == 0);
    CHECK(unknown[0] == '\0');
    CHECK(angdroid_item_origin_monster_name(unknown, sizeof unknown,
          "wolf", NULL, 1, 0) == 1);
    CHECK(strcmp(unknown, "wolf") == 0);

    /* A future long modded race name must fall back to the complete
     * canonical source, not be displayed as a severed UTF-8 label. */
    for (i = 0; i < 130; ++i)
        memcpy(long_name + 3 * i, "龍", 3);
    long_name[390] = '\0';
    CHECK(angdroid_item_origin_monster_name(unknown, sizeof unknown,
          long_name, "a ", 0, 1) == 0);
    CHECK(unknown[0] == '\0');
    CHECK(strcmp(long_name + 387, "龍") == 0);

    printf("PASS: source-exact origin race article and comma (%u assertions)\n",
           checks);
    return 0;
}
