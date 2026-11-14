/* Compile the actual FAangband trait mapping as plain C99, using the
 * same PF/OF enum sources as the game (no game runtime or Android needed).
 * The real player.h is intentionally guarded here: its structs are not
 * relevant to this presentation-only helper.
 */
#include <stdio.h>
#include <string.h>
#define PLAYER_H 1

enum {
#define PF(name) PF_##name,
#include "../app/src/main/cpp/faangband/list-player-flags.h"
#undef PF
    PF_MAX
};

enum {
    OF_NONE,
#define OF(name, label) OF_##name,
#include "../app/src/main/cpp/faangband/list-object-flags.h"
#undef OF
};

#include "../app/src/main/cpp/faangband/ui-birth-abilities-zh-tw.h"

static int check(const char *input, const char *type, int index,
        int lang, const char *expected)
{
    const char *out = angdroid_fa_birth_ability_name(
        input, type, index, lang);
    if ((!out && expected) || (out && !expected) ||
            (out && expected && strcmp(out, expected) != 0)) {
        fprintf(stderr, "Trait %s (%s, %d, locale %d) mismatch\n",
            input ? input : "(null)",
            type ? type : "(null)", index, lang);
        return 1;
    }
    return 0;
}

int main(void)
{
    /* Player and object flags have independent namespaces, and values
     * may overlap; the type is essential to avoiding false translations. */
    if (check("Hardy", "player", PF_HARDY, 1, "堅韌體質")) return 1;
    if (check("Shield bash", "player", PF_SHIELD_BASH, 1, "盾擊"))
        return 1;
    if (check("Armsman", "player", PF_ARMSMAN, 1, "武器專家"))
        return 1;
    if (check("Telepathy", "object", OF_TELEPATHY, 1, "心靈感應"))
        return 1;
    if (check("Free action", "object", OF_FREE_ACT, 1, "行動自由"))
        return 1;
    /* Negative and equipment-dependent flags must not accidentally
     * display positive effects. All use the native OF index. */
    if (check("Cannot teleport", "object", OF_NO_TELEPORT, 1, "無法傳送"))
        return 1;
    if (check("Drains experience", "object", OF_DRAIN_EXP, 1, "流失經驗"))
        return 1;
    if (check("Fragile", "object", OF_FRAGILE, 1, "容易損壞"))
        return 1;
    if (check("Two hands required", "object", OF_TWO_HANDED_REQ, 1,
            "必須雙手持用")) return 1;
    if (check("Darkness", "object", OF_DARKNESS, 1, "散發黑暗"))
        return 1;

    /* Never force Chinese in English, and don't invent translations
     * for unrecognized types and flags. */
    if (check("Hardy", "player", PF_HARDY, 0, "Hardy")) return 1;
    if (check("Hardy", "unknown", PF_HARDY, 1, "Hardy")) return 1;
    if (check("Unmapped", "player", PF_NONE, 1, "Unmapped")) return 1;
    if (check("Unmapped", "object", OF_NONE, 1, "Unmapped")) return 1;
    if (check(NULL, "player", PF_HARDY, 1, NULL)) return 1;
    if (check("Hardy", NULL, PF_HARDY, 1, "Hardy")) return 1;

    /* A known generic UI string can still translate through fallback. */
    if (check("Help", "unknown", 0, 1, "說明")) return 1;
    if (check("Help", "unknown", 0, 0, "Help")) return 1;

    puts("PASS: FA birth trait PF/OF type-safe Chinese labels and fallback");
    return 0;
}
