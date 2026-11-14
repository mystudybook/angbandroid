/* Starting item tabs: never prefer an empty allowed page to an
 * eligible nonempty one. Preserve the old empty browsing fallback. */
#include <assert.h>
#include <stdio.h>
#include "../../app/src/main/cpp/angband/item-picker-start-source.h"

static unsigned long assertions;
#define CHECK(p) do { assert(p); ++assertions; } while (0)

static int first_source(unsigned mask, const int source[4])
{
    int i;
    for (i = 0; i < 4; ++i)
        if (mask & (1u << i))
            return source[i];
    return source[0];
}

/* Independent policy oracle using masks and priority, not the production
 * loop that returns as soon as it discovers an eligible source. */
static int reference_start(const angdroid_item_picker_start_options *o)
{
    const unsigned filled = o->allowed & o->nonempty & 15u;
    const unsigned candidates = filled ? filled : (o->allowed & 15u);
    int i;
    if (!candidates) return o->source[0];

    for (i = 0; i < 4; ++i)
        if (o->current == o->source[i] && (candidates & (1u << i)))
            return o->current;

    if (o->prefer_quiver && (candidates & ANGDROID_PICKER_QUIVER))
        return o->source[2];
    if (candidates & o->requested)
        return first_source(candidates & o->requested, o->source);
    return first_source(candidates, o->source);
}

static void all_availability_combinations(void)
{
    const int sources[4] = { 11, 22, 33, 44 };
    const int current_choices[5] = { 11, 22, 33, 44, 55 };
    int allowed, nonempty, preferred, selected, quiver_hint;
    for (allowed = 0; allowed < 16; ++allowed)
        for (nonempty = 0; nonempty < 16; ++nonempty)
            for (preferred = 0; preferred < 16; ++preferred)
                for (selected = 0; selected < 5; ++selected)
                    for (quiver_hint = 0; quiver_hint <= 1; ++quiver_hint) {
                        const unsigned present = allowed & nonempty;
                        angdroid_item_picker_start_options options = {
                            current_choices[selected],
                            {11, 22, 33, 44},
                            (unsigned)allowed,
                            (unsigned)nonempty,
                            (unsigned)preferred,
                            quiver_hint
                        };
                        int actual = angdroid_item_picker_initial_source(&options);
                        int expected = reference_start(&options);
                        int slot = -1, i;
                        CHECK(actual == expected);
                        for (i = 0; i < 4; ++i)
                            if (sources[i] == actual)
                                slot = i;
                        CHECK(slot >= 0);
                        if (allowed) CHECK((allowed & (1 << slot)) != 0);
                        if (present) CHECK((present & (1 << slot)) != 0);
                        if (present && selected < 4 &&
                            (present & (1 << selected)))
                            CHECK(actual == current_choices[selected]);
                        if (!present && selected < 4 &&
                            (allowed & (1 << selected)))
                            CHECK(actual == current_choices[selected]);
                    }
}

static void important_user_scenarios(void)
{
    angdroid_item_picker_start_options opts = {
        11, {11, 22, 33, 44}, 15, ANGDROID_PICKER_EQUIP, 15, 0
    };
    CHECK(angdroid_item_picker_initial_source(&opts) == 22); /* empty bag */
    opts.nonempty = ANGDROID_PICKER_FLOOR;
    CHECK(angdroid_item_picker_initial_source(&opts) == 44); /* floor item */
    opts.nonempty = ANGDROID_PICKER_INVEN | ANGDROID_PICKER_EQUIP;
    CHECK(angdroid_item_picker_initial_source(&opts) == 11); /* keep tab */
    opts.nonempty = ANGDROID_PICKER_EQUIP | ANGDROID_PICKER_QUIVER;
    opts.prefer_quiver = 1;
    CHECK(angdroid_item_picker_initial_source(&opts) == 33); /* quiver tags */
    opts.nonempty = 0;
    CHECK(angdroid_item_picker_initial_source(&opts) == 11); /* empty view */
    opts.current = 55;
    opts.prefer_quiver = 0;
    opts.requested = ANGDROID_PICKER_EQUIP;
    CHECK(angdroid_item_picker_initial_source(&opts) == 22); /* requested */
    opts.requested = ANGDROID_PICKER_FLOOR;
    CHECK(angdroid_item_picker_initial_source(&opts) == 44);
    opts.allowed = ANGDROID_PICKER_INVEN;
    opts.nonempty = ANGDROID_PICKER_FLOOR; /* forbidden floor item */
    CHECK(angdroid_item_picker_initial_source(&opts) == 11);
    CHECK(angdroid_item_picker_initial_source(NULL) == 0);
}

int main(void)
{
    all_availability_combinations();
    important_user_scenarios();
    printf("PASS: item picker start-source policy (%lu assertions)\n", assertions);
    return 0;
}
