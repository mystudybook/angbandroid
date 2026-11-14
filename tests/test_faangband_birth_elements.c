/* Host-buildable C99 smoke test of FA birth elemental classifications.
 * Use the game's canonical ELEM enum and official percentage constants.
 * The game-only player.h structs are not needed by this display helper. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#define PLAYER_H 1
enum {
#define ELEM(name) ELEM_##name,
#include "../app/src/main/cpp/faangband/list-elements.h"
#undef ELEM
    ELEM_MAX
};
#define RES_LEVEL_BASE 100
#define RES_LEVEL_MAX 0
#include "../app/src/main/cpp/faangband/ui-birth-elements-zh-tw.h"

static int expect_name(int id, const char *expected)
{
    const char *name = angdroid_fa_birth_element_name(id);
    if ((!name && expected) || (name && !expected) ||
            (name && expected && strcmp(name, expected) != 0)) {
        fprintf(stderr, "wrong FA element %d\n", id);
        return 1;
    }
    return 0;
}

static int expect_tier(int percent, int expected)
{
    int found = angdroid_fa_birth_element_resistance(percent);
    if (found != expected) {
        fprintf(stderr, "wrong resistance tier %d: got %d expected %d\n",
            percent, found, expected);
        return 1;
    }
    return 0;
}

int main(void)
{
    int i;
    for (i = 0; i < ELEM_MAX; ++i) {
        const char *name = angdroid_fa_birth_element_name(i);
        if (!name || !*name) {
            fprintf(stderr, "missing FA element %d\n", i);
            return 1;
        }
    }
    if (expect_name(ELEM_ACID, "強酸")) return 1;
    if (expect_name(ELEM_COLD, "寒冷")) return 1;
    if (expect_name(ELEM_NEXUS, "空間連結")) return 1;
    if (expect_name(ELEM_DRAGONFIRE, "龍焰")) return 1;
    if (expect_name(ELEM_HOLY_ORB, "神聖能量")) return 1;
    if (expect_name(ELEM_MAX, NULL)) return 1;
    if (expect_name(-1, NULL)) return 1;

    if (expect_tier(0, 2)) return 1;   /* Immune, 0% damage. */
    if (expect_tier(20, 1)) return 1;  /* Strong resistance. */
    if (expect_tier(80, 1)) return 1;  /* Standard resistance. */
    if (expect_tier(99, 1)) return 1;  /* Minor resistance. */
    if (expect_tier(100, 0)) return 1; /* Normal damage. */
    if (expect_tier(130, -1)) return 1;/* Vulnerability. */
    if (expect_tier(200, -1)) return 1;/* Maximum vulnerability. */
    if (expect_tier(-1, 0)) return 1;  /* Unknown/invalid. */

    puts("PASS: FA elemental labels, damage resistance and weakness");
    return 0;
}
