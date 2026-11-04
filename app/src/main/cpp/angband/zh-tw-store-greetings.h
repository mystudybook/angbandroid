/* Display-only Traditional Chinese greetings for Angband shopkeepers.
 * Source English format strings stay canonical, unchanged and index-independent.
 * Neither player names nor in-game hint content is translated by this table.
 */
#ifndef ANGDROID_ZH_TW_STORE_GREETINGS_H
#define ANGDROID_ZH_TW_STORE_GREETINGS_H

#include <stddef.h>
#include <string.h>

#define ANGDROID_ZH_TW_STORE_GREETINGS 9

static inline const char *angdroid_store_greeting_format(const char *english,
                                                        int zh_tw)
{
    static const struct {
        const char *english;
        const char *traditional;
    } greetings[ANGDROID_ZH_TW_STORE_GREETINGS] = {
        { "%s nods to you.", "%s 向你點頭致意。" },
        { "%s says hello.", "%s 向你問好。" },
        { "%s: \"See anything you like, adventurer?\"",
          "%s：「冒險者，有看到喜歡的東西嗎？」" },
        { "%s: \"How may I help you, %s?\"",
          "%s：「%s，需要我幫忙嗎？」" },
        { "%s: \"Welcome back, %s.\"",
          "%s：「歡迎回來，%s。」" },
        { "%s: \"A pleasure to see you again, %s.\"",
          "%s：「很高興再次見到你，%s。」" },
        { "%s: \"How may I be of assistance, good %s?\"",
          "%s：「親愛的%s，需要什麼協助？」" },
        { "%s: \"You do honour to my humble store, noble %s.\"",
          "%s：「尊貴的%s，您光臨小店真是蓬蓽生輝。」" },
        { "%s: \"I and my family are entirely at your service, %s.\"",
          "%s：「%s，我和家人隨時樂意為你效勞。」" }
    };
    size_t i;

    if (!english || !zh_tw) return english;
    for (i = 0; i < ANGDROID_ZH_TW_STORE_GREETINGS; ++i)
        if (strcmp(english, greetings[i].english) == 0)
            return greetings[i].traditional;
    return english;
}

#endif /* ANGDROID_ZH_TW_STORE_GREETINGS_H */
